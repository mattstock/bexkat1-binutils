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
    { 0x00, 1, BEXKAT1_ADDR_INH,    "nop" },
    { 0x01, 1, BEXKAT1_ADDR_INH,    "rts" },
    { 0x02, 1, BEXKAT1_ADDR_INH,    "rti" },
    { 0x00, 3, BEXKAT1_ADDR_IMM,    "and" },
    { 0x01, 3, BEXKAT1_ADDR_IMM,    "or"  },
    { 0x02, 3, BEXKAT1_ADDR_IMM,    "add" },
    { 0x03, 3, BEXKAT1_ADDR_IMM,    "sub" },
    { 0x04, 3, BEXKAT1_ADDR_IMM,    "lsl" },
    { 0x05, 3, BEXKAT1_ADDR_IMM,    "asr" },
    { 0x06, 3, BEXKAT1_ADDR_IMM,    "lsr" },
    { 0x07, 3, BEXKAT1_ADDR_IMM,    "xor" },
    { 0x08, 3, BEXKAT1_ADDR_IMM,    "mul" },
    { 0x09, 3, BEXKAT1_ADDR_IMM,    "mulu" },
    { 0x50, 3, BEXKAT1_ADDR_IMM,    "div" },
    { 0x51, 3, BEXKAT1_ADDR_IMM,    "mod" },
    { 0x52, 3, BEXKAT1_ADDR_IMM,    "divu" },
    { 0x53, 3, BEXKAT1_ADDR_IMM,    "modu" },
    { 0x10, 2, BEXKAT1_ADDR_IMM,    "and" },
    { 0x11, 2, BEXKAT1_ADDR_IMM,    "or"  },
    { 0x12, 2, BEXKAT1_ADDR_IMM,    "add" },
    { 0x13, 2, BEXKAT1_ADDR_IMM,    "sub" },
    { 0x14, 2, BEXKAT1_ADDR_IMM,    "lsl" },
    { 0x15, 2, BEXKAT1_ADDR_IMM,    "asr" },
    { 0x16, 2, BEXKAT1_ADDR_IMM,    "lsr" },
    { 0x17, 2, BEXKAT1_ADDR_IMM,    "xor" }, 
    { 0x18, 2, BEXKAT1_ADDR_IMM,    "mul" }, 
    { 0x19, 2, BEXKAT1_ADDR_IMM,    "mulu" }, 
    { 0x60, 2, BEXKAT1_ADDR_IMM,    "div" },
    { 0x61, 2, BEXKAT1_ADDR_IMM,    "mod" },
    { 0x62, 2, BEXKAT1_ADDR_IMM,    "divu" },
    { 0x63, 2, BEXKAT1_ADDR_IMM,    "modu" },
    { 0x20, 3, BEXKAT1_ADDR_IMM,    "ldi" },
    { 0x21, 2, BEXKAT1_ADDR_IMM,    "lds" },
    { 0x22, 2, BEXKAT1_ADDR_IMM,    "ldu" },
    { 0x00, 2, BEXKAT1_ADDR_REG,    "and" },
    { 0x01, 2, BEXKAT1_ADDR_REG,    "or"  },
    { 0x02, 2, BEXKAT1_ADDR_REG,    "add" },
    { 0x03, 2, BEXKAT1_ADDR_REG,    "sub" },
    { 0x04, 2, BEXKAT1_ADDR_REG,    "lsl" },
    { 0x05, 2, BEXKAT1_ADDR_REG,    "asr" },
    { 0x06, 2, BEXKAT1_ADDR_REG,    "lsr" },
    { 0x07, 2, BEXKAT1_ADDR_REG,    "xor" },
    { 0x08, 2, BEXKAT1_ADDR_REG,    "mul" },
    { 0x09, 2, BEXKAT1_ADDR_REG,    "mulu" },
    { 0x50, 2, BEXKAT1_ADDR_REG,    "div" },
    { 0x51, 2, BEXKAT1_ADDR_REG,    "mod" },
    { 0x52, 2, BEXKAT1_ADDR_REG,    "divu" },
    { 0x53, 2, BEXKAT1_ADDR_REG,    "modu" },
    { 0x10, 2, BEXKAT1_ADDR_REG,    "mov" },
    { 0x11, 2, BEXKAT1_ADDR_REG,    "cmp" },
    { 0x12, 2, BEXKAT1_ADDR_REG,    "neg" },
    { 0x13, 1, BEXKAT1_ADDR_REG,    "inc" },
    { 0x14, 1, BEXKAT1_ADDR_REG,    "dec" },
    { 0x15, 1, BEXKAT1_ADDR_REG,    "push" },
    { 0x16, 1, BEXKAT1_ADDR_REG,    "pop" },
    { 0x17, 2, BEXKAT1_ADDR_REG,    "com" },
    { 0x20, 2, BEXKAT1_ADDR_PCIND,  "bra" },  
    { 0x21, 2, BEXKAT1_ADDR_PCIND,  "beq" },  
    { 0x22, 2, BEXKAT1_ADDR_PCIND,  "bne" },  
    { 0x23, 2, BEXKAT1_ADDR_PCIND,  "bgtu" },  
    { 0x24, 2, BEXKAT1_ADDR_PCIND,  "bgt" },  
    { 0x25, 2, BEXKAT1_ADDR_PCIND,  "bge" },  
    { 0x26, 2, BEXKAT1_ADDR_PCIND,  "ble" },  
    { 0x27, 2, BEXKAT1_ADDR_PCIND,  "blt" },  
    { 0x28, 2, BEXKAT1_ADDR_PCIND,  "bgeu" },  
    { 0x29, 2, BEXKAT1_ADDR_PCIND,  "bltu" },  
    { 0x2a, 2, BEXKAT1_ADDR_PCIND,  "bleu" },  
    { 0x2c, 2, BEXKAT1_ADDR_PCIND,  "brn" },
    { 0x20, 2, BEXKAT1_ADDR_REGIND, "st.l" },
    { 0x21, 2, BEXKAT1_ADDR_REGIND, "ld.l" },
    { 0x30, 2, BEXKAT1_ADDR_REGIND, "st" },
    { 0x31, 2, BEXKAT1_ADDR_REGIND, "ld" },
    { 0x40, 2, BEXKAT1_ADDR_REGIND, "st.b" },
    { 0x41, 2, BEXKAT1_ADDR_REGIND, "ld.b" },
    { 0x50, 2, BEXKAT1_ADDR_REGIND, "jmp" },
    { 0x51, 2, BEXKAT1_ADDR_REGIND, "jsr" },
    { 0x20, 3, BEXKAT1_ADDR_DIR,    "st.l" },
    { 0x21, 3, BEXKAT1_ADDR_DIR,    "ld.l" }, 
    { 0x30, 3, BEXKAT1_ADDR_DIR,    "st" }, 
    { 0x31, 3, BEXKAT1_ADDR_DIR,    "ld" }, 
    { 0x40, 3, BEXKAT1_ADDR_DIR,    "st.b" },
    { 0x41, 3, BEXKAT1_ADDR_DIR,    "ld.b" }, 
    { 0x50, 3, BEXKAT1_ADDR_DIR,    "jmp" },
    { 0x51, 3, BEXKAT1_ADDR_DIR,    "jsr" }
  };

const int bexkat1_opc_count = (sizeof(bexkat1_opc_info)/sizeof(bexkat1_opc_info[0]));
