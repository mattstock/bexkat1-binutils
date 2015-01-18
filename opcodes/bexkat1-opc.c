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
    { 0x00, 0, BEXKAT1_INH,    "nop" },
    { 0x20, 0, BEXKAT1_INH,    "rts" },
    { 0x40, 2, BEXKAT1_INH,    "cmp" },
    { 0x60, 1, BEXKAT1_INH,    "inc" },
    { 0x80, 1, BEXKAT1_INH,    "dec" },
    { 0xa0, 1, BEXKAT1_INH,    "push" },
    { 0xc0, 1, BEXKAT1_INH,    "pop" },
    { 0xe0, 2, BEXKAT1_INH,    "mov" },
    { 0x02, 2, BEXKAT1_INH2,   "com" },
    { 0x22, 2, BEXKAT1_INH2,   "neg" },
    { 0x41, 2, BEXKAT1_INH2,   "cmp.d" },
    { 0x42, 0, BEXKAT1_INH2,   "rti" },
    { 0x43, 2, BEXKAT1_INH2,   "cmp.s" },
    { 0x23, 2, BEXKAT1_INH2,   "neg.s" },
    { 0xa1, 1, BEXKAT1_INH2,   "push.d" },
    { 0xa3, 1, BEXKAT1_INH2,   "push.s" },
    { 0xc1, 1, BEXKAT1_INH2,   "pop.d" },
    { 0xc3, 1, BEXKAT1_INH2,   "pop.s" },
    { 0xe3, 2, BEXKAT1_INH2,   "mov.s" },
    { 0x00, 3, BEXKAT1_IMM3,    "andi" },
    { 0x01, 3, BEXKAT1_IMM3,    "ori"  },
    { 0x02, 3, BEXKAT1_IMM3,    "addi" },
    { 0x03, 3, BEXKAT1_IMM3,    "subi" },
    { 0x04, 3, BEXKAT1_IMM3,    "lsli" },
    { 0x05, 3, BEXKAT1_IMM3,    "asri" },
    { 0x06, 3, BEXKAT1_IMM3,    "lsri" },
    { 0x07, 3, BEXKAT1_IMM3,    "xori" },
    { 0x28, 3, BEXKAT1_IMM3,    "muli" },
    { 0x29, 3, BEXKAT1_IMM3,    "divi" },
    { 0x2a, 3, BEXKAT1_IMM3,    "modi" },
    { 0x48, 3, BEXKAT1_IMM3,    "muliu" },
    { 0x49, 3, BEXKAT1_IMM3,    "diviu" },
    { 0x4a, 3, BEXKAT1_IMM3,    "modiu" },
    { 0x00, 2, BEXKAT1_IMM3a,   "ldi" },
    { 0x00, 2, BEXKAT1_DIR,     "std.l" },
    { 0x01, 2, BEXKAT1_DIR,     "ldd.l" }, 
    { 0x02, 2, BEXKAT1_DIR,     "std" }, 
    { 0x03, 2, BEXKAT1_DIR,     "ldd" }, 
    { 0x04, 2, BEXKAT1_DIR,     "std.b" },
    { 0x05, 2, BEXKAT1_DIR,     "ldd.b" }, 
    { 0x06, 2, BEXKAT1_DIR,     "std.s" },
    { 0x07, 2, BEXKAT1_DIR,     "ldd.s" }, 
    { 0x08, 2, BEXKAT1_DIR,     "std.d" },
    { 0x09, 2, BEXKAT1_DIR,     "ldd.d" }, 
    { 0x80, 1, BEXKAT1_DIR,     "jmpd" },
    { 0x81, 1, BEXKAT1_DIR,     "jsrd" },
    { 0x00, 1, BEXKAT1_IMM2,    "bra" },  
    { 0x01, 1, BEXKAT1_IMM2,    "beq" },  
    { 0x02, 1, BEXKAT1_IMM2,    "bne" },  
    { 0x03, 1, BEXKAT1_IMM2,    "bgtu" },  
    { 0x04, 1, BEXKAT1_IMM2,    "bgt" },  
    { 0x05, 1, BEXKAT1_IMM2,    "bge" },  
    { 0x06, 1, BEXKAT1_IMM2,    "ble" },  
    { 0x07, 1, BEXKAT1_IMM2,    "blt" },  
    { 0x08, 1, BEXKAT1_IMM2,    "bgeu" },  
    { 0x09, 1, BEXKAT1_IMM2,    "bltu" },  
    { 0x0a, 1, BEXKAT1_IMM2,    "bleu" },  
    { 0x0b, 1, BEXKAT1_IMM2,    "brn" },
    { 0x10, 2, BEXKAT1_IMM2,    "ldis" },
    { 0x11, 2, BEXKAT1_IMM2,    "ldiu" },
    { 0x00, 3, BEXKAT1_REG,     "and" },
    { 0x01, 3, BEXKAT1_REG,     "or"  },
    { 0x02, 3, BEXKAT1_REG,     "add" },
    { 0x03, 3, BEXKAT1_REG,     "sub" },
    { 0x04, 3, BEXKAT1_REG,     "lsl" },
    { 0x05, 3, BEXKAT1_REG,     "asr" },
    { 0x06, 3, BEXKAT1_REG,     "lsr" },
    { 0x07, 3, BEXKAT1_REG,     "xor" },
    { 0x28, 3, BEXKAT1_REG,     "mul" },
    { 0x29, 3, BEXKAT1_REG,     "div" },
    { 0x2a, 3, BEXKAT1_REG,     "mod" },
    { 0x48, 3, BEXKAT1_REG,     "mulu" },
    { 0x49, 3, BEXKAT1_REG,     "divu" },
    { 0x4a, 3, BEXKAT1_REG,     "modu" },
    { 0x62, 3, BEXKAT1_REG,     "add.s" },
    { 0x63, 3, BEXKAT1_REG,     "sub.s" },
    { 0x68, 3, BEXKAT1_REG,     "mul.s" },
    { 0x69, 3, BEXKAT1_REG,     "div.s" },
    { 0x81, 3, BEXKAT1_REG,     "add.d" },
    { 0x82, 3, BEXKAT1_REG,     "sub.d" },
    { 0x88, 3, BEXKAT1_REG,     "mul.d" },
    { 0x89, 3, BEXKAT1_REG,     "div.d" },
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
    { 0xa0, 2, BEXKAT1_REGIND,  "jmp" },
    { 0xa1, 2, BEXKAT1_REGIND,  "jsr" }
  };

const int bexkat1_opc_count = (sizeof(bexkat1_opc_info)/sizeof(bexkat1_opc_info[0]));
