/* src/vm/jit/i386/cygwin/md-asm.h - assembler defines for Cygwin i386 ABI

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

   Authors: Michael Starzinger

   Changes:

   $Id: md-asm.h 5107 2006-07-11 18:35:27Z twisti $

*/


#ifndef _MD_ASM_H
#define _MD_ASM_H

/* register defines ***********************************************************/

#define v0       %eax
#define itmp1    v0

#define itmp2    %ecx
#define itmp3    %edx

#define t0       %ebx

#define sp       %esp
#define s0       %ebp
#define s1       %esi
#define s2       %edi

#define bp       s0

#define itmp1b   %al

#define xptr     itmp1
#define xpc      itmp2
#define mptr     itmp2


/* save and restore macros ****************************************************/

#define SAVE_ARGUMENT_REGISTERS(off) \
    /* no argument registers */

#define SAVE_TEMPORARY_REGISTERS(off) \
	mov     t0,(0+(off))*4(sp) ;


#define RESTORE_ARGUMENT_REGISTERS(off) \
    /* no argument registers */

#define RESTORE_TEMPORARY_REGISTERS(off) \
	mov     (0+(off))*4(sp),t0 ;


/* defines for cygwin's old gnu assembler *************************************/

#define asm_md_init                           _asm_md_init

#define asm_vm_call_method                    _asm_vm_call_method
#define asm_vm_call_method_int                _asm_vm_call_method_int
#define asm_vm_call_method_long               _asm_vm_call_method_long
#define asm_vm_call_method_float              _asm_vm_call_method_float
#define asm_vm_call_method_double             _asm_vm_call_method_double

#define asm_vm_call_method_exception_handler  _asm_vm_call_method_exception_handler

#define asm_call_jit_compiler                 _asm_call_jit_compiler

#define asm_handle_nat_exception              _asm_handle_nat_exception
#define asm_handle_exception                  _asm_handle_exception

#define asm_abstractmethoderror               _asm_abstractmethoderror

#define asm_patcher_wrapper                   _asm_patcher_wrapper

#define asm_replacement_out                   _asm_replacement_out
#define asm_replacement_in                    _asm_replacement_in

#define asm_builtin_f2i                       _asm_builtin_f2i
#define asm_builtin_f2l                       _asm_builtin_f2l
#define asm_builtin_d2i                       _asm_builtin_d2i
#define asm_builtin_d2l                       _asm_builtin_d2l

#define asm_criticalsections                  _asm_criticalsections
#define asm_getclassvalues_atomic             _asm_getclassvalues_atomic


/* external defines ***********************************************************/

#define exceptions_get_and_clear_exception    _exceptions_get_and_clear_exception

#define builtin_throw_exception               _builtin_throw_exception
#define codegen_get_pv_from_pc                _codegen_get_pv_from_pc
#define exceptions_handle_exception           _exceptions_handle_exception
#define jit_asm_compile                       _jit_asm_compile

#define exceptions_asm_new_abstractmethoderror \
    _exceptions_asm_new_abstractmethoderror

#define patcher_wrapper                       _patcher_wrapper

#define replace_me                            _replace_me

#define abort                                 _abort

#define builtin_f2i                           _builtin_f2i
#define builtin_f2l                           _builtin_f2l
#define builtin_d2i                           _builtin_d2i
#define builtin_d2l                           _builtin_d2l

#endif /* _MD_ASM_H */


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
 * vim:noexpandtab:sw=4:ts=4:
 */
