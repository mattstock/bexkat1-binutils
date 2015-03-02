/* bexkat1-opc.c -- Bexkat 1 opcode list
   Copyright (C) 1999-2014 Free Software Foundation, Inc.
   Written by Matt Stock (stock@bexkat.com)

   This file is part of the GNU opcodes library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this file; see the file COPYING.  If not, write to the
   Free Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include <stdio.h>
#include "ansidecl.h"
#include "opcode/bexkat1.h"

const bexkat1_opc_info_t bexkat1_opc_info[] =
  {
    { 0x00, 0, BEXKAT1_REG,    "nop" },
    { 0x01, 0, BEXKAT1_REG,    "rts" },
    { 0x02, 2, BEXKAT1_REG,    "cmp" },
    { 0x03, 1, BEXKAT1_REG,    "inc" },
    { 0x04, 1, BEXKAT1_REG,    "dec" },
    { 0x05, 1, BEXKAT1_REG,    "push" },
    { 0x06, 1, BEXKAT1_REG,    "pop" },
    { 0x07, 2, BEXKAT1_REG,    "mov" },
    { 0x08, 2, BEXKAT1_REG,   "com" },
    { 0x09, 2, BEXKAT1_REG,   "neg" },
    { 0x0a, 2, BEXKAT1_REG,   "cmp.d" },
    { 0x0b, 0, BEXKAT1_REG,   "rti" },
    { 0x0c, 2, BEXKAT1_REG,   "cmp.s" },
    { 0x0d, 2, BEXKAT1_REG,   "neg.s" },
    { 0x0e, 1, BEXKAT1_REG,   "push.d" },
    { 0x0f, 1, BEXKAT1_REG,   "push.s" },
    { 0x10, 1, BEXKAT1_REG,   "pop.d" },
    { 0x11, 1, BEXKAT1_REG,   "pop.s" },
    { 0x12, 2, BEXKAT1_REG,   "mov.s" },
    { 0x13, 2, BEXKAT1_REG,   "cvtis" },
    { 0x14, 2, BEXKAT1_REG,   "cvtsi" },
    { 0x20, 3, BEXKAT1_REG,     "and" },
    { 0x21, 3, BEXKAT1_REG,     "or"  },
    { 0x22, 3, BEXKAT1_REG,     "add" },
    { 0x23, 3, BEXKAT1_REG,     "sub" },
    { 0x24, 3, BEXKAT1_REG,     "lsl" },
    { 0x25, 3, BEXKAT1_REG,     "asr" },
    { 0x26, 3, BEXKAT1_REG,     "lsr" },
    { 0x27, 3, BEXKAT1_REG,     "xor" },
    { 0x30, 3, BEXKAT1_REG,     "mul" },
    { 0x31, 3, BEXKAT1_REG,     "div" },
    { 0x32, 3, BEXKAT1_REG,     "mod" },
    { 0x33, 3, BEXKAT1_REG,     "mulu" },
    { 0x34, 3, BEXKAT1_REG,     "divu" },
    { 0x35, 3, BEXKAT1_REG,     "modu" },
    { 0x40, 3, BEXKAT1_REG,     "add.s" },
    { 0x41, 3, BEXKAT1_REG,     "sub.s" },
    { 0x42, 3, BEXKAT1_REG,     "mul.s" },
    { 0x43, 3, BEXKAT1_REG,     "div.s" },
    { 0x44, 3, BEXKAT1_REG,     "add.d" },
    { 0x45, 3, BEXKAT1_REG,     "sub.d" },
    { 0x46, 3, BEXKAT1_REG,     "mul.d" },
    { 0x47, 3, BEXKAT1_REG,     "div.d" },
    { 0x00, 3, BEXKAT1_DIR,    "andi" },
    { 0x01, 3, BEXKAT1_DIR,    "ori"  },
    { 0x02, 3, BEXKAT1_DIR,    "addi" },
    { 0x03, 3, BEXKAT1_DIR,    "subi" },
    { 0x04, 3, BEXKAT1_DIR,    "lsli" },
    { 0x05, 3, BEXKAT1_DIR,    "asri" },
    { 0x06, 3, BEXKAT1_DIR,    "lsri" },
    { 0x07, 3, BEXKAT1_DIR,    "xori" },
    { 0x20, 3, BEXKAT1_DIR,    "muli" },
    { 0x21, 3, BEXKAT1_DIR,    "divi" },
    { 0x22, 3, BEXKAT1_DIR,    "modi" },
    { 0x23, 3, BEXKAT1_DIR,    "muliu" },
    { 0x24, 3, BEXKAT1_DIR,    "diviu" },
    { 0x25, 3, BEXKAT1_DIR,    "modiu" },
    { 0x30, 2, BEXKAT1_DIR,     "std.l" },
    { 0x31, 2, BEXKAT1_DIR,     "ldd.l" }, 
    { 0x32, 2, BEXKAT1_DIR,     "std" }, 
    { 0x33, 2, BEXKAT1_DIR,     "ldd" }, 
    { 0x34, 2, BEXKAT1_DIR,     "std.b" },
    { 0x35, 2, BEXKAT1_DIR,     "ldd.b" }, 
    { 0x36, 2, BEXKAT1_DIR,     "std.s" },
    { 0x37, 2, BEXKAT1_DIR,     "ldd.s" }, 
    { 0x38, 2, BEXKAT1_DIR,     "std.d" },
    { 0x39, 2, BEXKAT1_DIR,     "ldd.d" }, 
    { 0x3a, 1, BEXKAT1_DIR,     "jmpd" },
    { 0x3b, 1, BEXKAT1_DIR,     "jsrd" },
    { 0x3c, 2, BEXKAT1_DIR,   "ldi" },
    { 0x00, 1, BEXKAT1_IMM,    "bra" },  
    { 0x01, 1, BEXKAT1_IMM,    "beq" },  
    { 0x02, 1, BEXKAT1_IMM,    "bne" },  
    { 0x03, 1, BEXKAT1_IMM,    "bgtu" },  
    { 0x04, 1, BEXKAT1_IMM,    "bgt" },  
    { 0x05, 1, BEXKAT1_IMM,    "bge" },  
    { 0x06, 1, BEXKAT1_IMM,    "ble" },  
    { 0x07, 1, BEXKAT1_IMM,    "blt" },  
    { 0x08, 1, BEXKAT1_IMM,    "bgeu" },  
    { 0x09, 1, BEXKAT1_IMM,    "bltu" },  
    { 0x0a, 1, BEXKAT1_IMM,    "bleu" },  
    { 0x0b, 1, BEXKAT1_IMM,    "brn" },
    { 0x0c, 2, BEXKAT1_IMM,    "ldis" },
    { 0x0d, 2, BEXKAT1_IMM,    "ldiu" },
    { 0x00, 3, BEXKAT1_REGIND,  "st.l" },
    { 0x01, 3, BEXKAT1_REGIND,  "ld.l" },
    { 0x02, 3, BEXKAT1_REGIND,  "st" },
    { 0x03, 3, BEXKAT1_REGIND,  "ld" },
    { 0x04, 3, BEXKAT1_REGIND,  "st.b" },
    { 0x05, 3, BEXKAT1_REGIND,  "ld.b" },
    { 0x06, 3, BEXKAT1_REGIND,  "st.s" },
    { 0x07, 3, BEXKAT1_REGIND,  "ld.s" },
    { 0x08, 3, BEXKAT1_REGIND,  "st.d" },
    { 0x09, 3, BEXKAT1_REGIND,  "ld.d" },
    { 0x0a, 3, BEXKAT1_REGIND,  "lda" },
    { 0x0b, 2, BEXKAT1_REGIND,  "jmp" },
    { 0x0c, 2, BEXKAT1_REGIND,  "jsr" }
  };

const int bexkat1_opc_count = (sizeof(bexkat1_opc_info)/sizeof(bexkat1_opc_info[0]));
