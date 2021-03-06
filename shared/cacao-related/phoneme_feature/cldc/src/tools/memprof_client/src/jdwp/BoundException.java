/*
 *   
 *
 * Copyright  1990-2006 Sun Microsystems, Inc. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation. 
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt). 
 * 
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA 
 * 
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 or visit www.sun.com if you need additional
 * information or have any questions. 
 */


package com.sun.cldchi.tools.memoryprofiler.jdwp;

/**
 * This exception is raised by methods of <code>Packet</code> and its subclasses
 * when JDWP packet is malformed and we try to read some data but see end of
 * packet (for example, if we try to read int value from packet by
 * <code>ByteBuffer.getInt()</code> but there is no more data in packet).
 * Usually raising of this exception indicates fatal error.
 */
public class BoundException extends Exception {
    
    /**
     * Constructs an Exception with no specified detail message
     */
    public BoundException() {
    }
    
    /**
     * Constructs an Exception with the specified detail message
     *
     * @param msg the detail message
     */
    public BoundException(String msg) {
        super(msg);
    }
}
