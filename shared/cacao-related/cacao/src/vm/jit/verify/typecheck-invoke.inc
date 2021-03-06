/* src/vm/jit/verify/typecheck-invoke.inc - type checking for invocations

   Copyright (C) 1996-2005, 2006 R. Grafl, A. Krall, C. Kruegel,
   C. Oates, R. Obermaisser, M. Platter, M. Probst, S. Ring,
   E. Steiner, C. Thalinger, D. Thuernbeck, P. Tomsich, C. Ullrich,
   J. Wenninger, Institut f. Computersprachen - TU Wien

   This file is part of CACAO.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2, or (at
   your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

   Contact: cacao@cacaojvm.org

   Authors: Edwin Steiner

   $Id$

*/


{
	unresolved_method *um;      /* struct describing the called method */
	constant_FMIref *mref;           /* reference to the called method */
	methoddesc *md;                 /* descriptor of the called method */
	utf *mname;                                         /* method name */
    u1 rtype;                          /* return type of called method */
#if !defined(TYPECHECK_TYPEINFERER)
	methodinfo *mi;                        /* resolved method (if any) */
	utf *mclassname;                     /* name of the method's class */
	bool specialmethod;            /* true if a <...> method is called */
	int opcode;                                   /* invocation opcode */
	bool callinginit;                      /* true if <init> is called */
	instruction *ins;
	classref_or_classinfo initclass;
	typedesc *td;
#if defined(TYPECHECK_VARIABLESBASED)
	s4 argindex;                            /* argument variable index */
	varinfo *av;                                  /* argument variable */
#else
	typedescriptor *av;                         /* argument stack slot */
#endif
	int i;                                                  /* counter */
	resolve_result_t result;
	bool invokestatic;
	bool invokespecial;
#endif /* !defined(TYPECHECK_TYPEINFERER) */

	/* get the FMIref and the unresolved_method struct (if any) */
	/* from the instruction                                     */

	if (INSTRUCTION_IS_UNRESOLVED(state->iptr)) {
		/* unresolved method */
		um = state->iptr->sx.s23.s3.um;
		mref = um->methodref;
	}
	else {
		/* resolved method */
		um = NULL;
		mref = state->iptr->sx.s23.s3.fmiref;
	}

	/* get method descriptor and name */

	md = mref->parseddesc.md;
	mname = mref->name;

#if !defined(TYPECHECK_TYPEINFERER)
	/* get method info (if resolved) and classname */

	if (IS_FMIREF_RESOLVED(mref)) {
		mi = mref->p.method;
		mclassname = mi->class->name;
	}
	else {
		mi = NULL;
		mclassname = mref->p.classref->name;
	}

	/* gather some info about this instruction */

	opcode = state->iptr->opc;
	invokestatic = (opcode == ICMD_INVOKESTATIC);
	invokespecial = (opcode == ICMD_INVOKESPECIAL);
	specialmethod = (mname->text[0] == '<');

	/* prevent compiler warnings */

	ins = NULL;

	/* check whether we are calling <init> */
	
	callinginit = (invokespecial && mname == utf_init);
	if (specialmethod && !callinginit)
		TYPECHECK_VERIFYERROR_bool("Invalid invocation of special method");

	/* allocate parameters if necessary */
	
	if (!md->params)
		if (!descriptor_params_from_paramtypes(md,
					(invokestatic) ? ACC_STATIC : ACC_NONE))
			return false;

	/* check parameter types */

#if defined(TYPECHECK_STACKBASED)
	av = stack - (md->paramslots - 1);
	for (i=0; i<md->paramcount; ++i) {
#else
	i = md->paramcount; /* number of parameters including 'this'*/
	while (--i >= 0) {
		argindex = state->iptr->sx.s23.s2.args[i];
		av = VAR(argindex);
#endif
		LOG1("\t\tparam %d",i);
		td = md->paramtypes + i;

		if (av->type != td->type)
			TYPECHECK_VERIFYERROR_bool("Parameter type mismatch in method invocation");

		if (av->type == TYPE_ADR) {
			LOGSTR("\t\t"); LOGINFO(&(av->typeinfo));
			if (i==0 && callinginit)
			{
				/* first argument to <init> method */
				if (!TYPEINFO_IS_NEWOBJECT(av->typeinfo))
					TYPECHECK_VERIFYERROR_bool("Calling <init> on initialized object");

				/* get the address of the NEW instruction */
				LOGSTR("\t\t"); LOGINFO(&(av->typeinfo));
				ins = (instruction *) TYPEINFO_NEWOBJECT_INSTRUCTION(av->typeinfo);
				if (ins)
					initclass = ins[-1].sx.val.c;
				else
					initclass.cls = state->m->class;
				LOGSTR("\t\tclass: "); LOGNAME(initclass); LOGNL;
			}
		}
		else {
			/* non-adress argument. if this is the first argument and we are */
			/* invoking an instance method, this is an error.                */
			if (i==0 && !invokestatic) {
				TYPECHECK_VERIFYERROR_bool("Parameter type mismatch for 'this' argument");
			}
		}
		LOG("\t\tok");

#if defined(TYPECHECK_STACKBASED)
		av += (IS_2_WORD_TYPE(av->type)) ? 2 : 1;
#endif
	}

	if (callinginit) {
		LOG("\treplacing uninitialized object");
		/* replace uninitialized object type on stack */

		/* for all live-in and live-through variables */ 
#if defined(TYPECHECK_VARIABLESBASED)
		for (i=0; i<state->iptr->s1.argcount; ++i) {
			argindex = state->iptr->sx.s23.s2.args[i];
			av = VAR(argindex);
#else
		for (av=stackfloor; av <= stack; ++av) {
#endif
			if (av->type == TYPE_ADR
					&& TYPEINFO_IS_NEWOBJECT(av->typeinfo)
					&& TYPEINFO_NEWOBJECT_INSTRUCTION(av->typeinfo) == ins)
			{
				LOG("\treplacing uninitialized type");

#if defined(TYPECHECK_VARIABLESBASED)
				/* If this stackslot is in the instack of
				 * this basic block we must save the type(s)
				 * we are going to replace.
				 */
				/* XXX this needs a new check */
				if (state->bptr->invars
						&& argindex >= state->bptr->invars[0]
						&& argindex < state->bptr->varstart 
						&& !state->savedinvars)
				{
					typestate_save_invars(state);
				}
#endif /* defined(TYPECHECK_VARIABLESBASED) */

				if (!typeinfo_init_class(&(av->typeinfo),initclass))
					return false;
			}
		}

		/* replace uninitialized object type in locals */
#if defined(TYPECHECK_VARIABLESBASED)
		if (!typevector_init_object(state->jd->var, ins, initclass,
					state->numlocals))
			return false;
#else
		/* XXX should reuse typevector code */
		for (i=0; i<state->numlocals; ++i) {
			if (state->locals[i].type == TYPE_ADR
				&& TYPEINFO_IS_NEWOBJECT(state->locals[i].typeinfo)
				&& TYPEINFO_NEWOBJECT_INSTRUCTION(state->locals[i].typeinfo) == ins)
			{
				if (!typeinfo_init_class(&(state->locals[i].typeinfo), initclass))
					return false;
			}
		}
#endif

		/* initializing the 'this' reference? */
		if (!ins) {
			classinfo *cls;
			TYPECHECK_ASSERT(state->initmethod);
			/* { we are initializing the 'this' reference }                           */
			/* must be <init> of current class or direct superclass                   */
			/* the current class is linked, so must be its superclass. thus we can be */
			/* sure that resolving will be trivial.                                   */
			if (mi) {
				cls = mi->class;
			}
			else {
				if (!resolve_classref(state->m,mref->p.classref,resolveLazy,false,true,&cls))
					return false; /* exception */
			}

			/* if lazy resolving did not succeed, it's not one of the allowed classes */
			/* otherwise we check it directly                                         */
			if (cls == NULL || (cls != state->m->class && cls != state->m->class->super.cls)) {
				TYPECHECK_VERIFYERROR_bool("<init> calling <init> of the wrong class");
			}

			/* set our marker variable to type int */
			LOG("\tsetting <init> marker");
#if defined(TYPECHECK_VARIABLESBASED)
			typevector_store(jd->var, state->numlocals-1, TYPE_INT, NULL);
#else
			state->locals[state->numlocals-1].type = TYPE_INT;
#endif
		}
		else {
			/* { we are initializing an instance created with NEW } */
			if ((IS_CLASSREF(initclass) ? initclass.ref->name : initclass.cls->name) != mclassname) {
				TYPECHECK_VERIFYERROR_bool("wrong <init> called for uninitialized reference");
			}
		}
	} /* end if (callinginit) */

	/* try to resolve the method lazily */

	result = resolve_method_lazy(state->m, mref, invokespecial);

	/* perform verification checks */

	if (result == resolveSucceeded) {

		assert(IS_FMIREF_RESOLVED(mref));

		mi = mref->p.method;

		result = resolve_method_verifier_checks(state->m, 
												mref,
												mi,
												invokestatic);
	}

	/* check types of parameters */

	if (result == resolveSucceeded && !invokestatic)
		result = resolve_method_instance_type_checks(
				state->m, mi,
				&(OP1->typeinfo),
				invokespecial);

#if defined(TYPECHECK_VARIABLESBASED)
	if (result == resolveSucceeded)
		result = resolve_method_param_type_checks(
				jd, state->m, state->iptr,
				mi, invokestatic);
#else
	if (result == resolveSucceeded)
		result = resolve_method_param_type_checks_stackbased(
				state->m, mi, invokestatic, stack);
#endif

	/* impose loading constraints */

	if (result == resolveSucceeded) {
		/* XXX state->m->class may have to be wrong when inlining */
		if (!resolve_method_loading_constraints(state->m->class, mi))
			return false;
	}

	if (result == resolveFailed)
		return false;

	if (result == resolveSucceeded) {
		/* if this call is monomorphic, turn it into an INVOKESPECIAL */

		if ((state->iptr->opc == ICMD_INVOKEVIRTUAL)
			&& (mi->flags & (ACC_FINAL | ACC_PRIVATE)))
		{
			state->iptr->opc         = ICMD_INVOKESPECIAL;
			state->iptr->flags.bits |= INS_FLAG_CHECK;
		}
	}
	else {
		/* resolution must be deferred */

		if (!um) {
			um = resolve_create_unresolved_method(state->m->class, state->m,
					mref, 
					invokestatic,
					invokespecial);

			if (!um)
				return false;
		}

		/* record subtype constraints for parameters */

		if (!invokestatic && !resolve_constrain_unresolved_method_instance(
					um, state->m, 
					&(OP1->typeinfo),
					invokespecial))
			return false; /* XXX maybe wrap exception */

#if defined(TYPECHECK_VARIABLESBASED)
		if (!resolve_constrain_unresolved_method_params(
					jd, um, state->m, state->iptr))
			return false; /* XXX maybe wrap exception */
#else
		if (!resolve_constrain_unresolved_method_params_stackbased(
					um, state->m, stack))
			return false; /* XXX maybe wrap exception */
#endif

		/* store the unresolved_method pointer */

		state->iptr->sx.s23.s3.um = um;
		state->iptr->flags.bits |= INS_FLAG_UNRESOLVED;
	}
#endif /* !defined(TYPECHECK_TYPEINFERER) */

	/* set the return type */

	rtype = md->returntype.type;
	if (rtype != TYPE_VOID) {
		dv->type = rtype;
		if (!typeinfo_init_from_typedesc(&(md->returntype), NULL, &(dv->typeinfo)))
			return false;
	}
}

/*
 * These are local overrides for various environment variables in Emacs.
 * Please do not remove this and leave it at the end of the file, where
 * Emacs will automagically detect them.
 * ---------------------------------------------------------------------
 * Local variables:
 * mode: c
 * indent-tabs-mode: t
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim:noexpandtab:sw=4:ts=4:filetype=c:
 */
