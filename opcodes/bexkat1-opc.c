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

const bexkat1_opc_info_t bexkat1_opc_info[BEXKAT1_OPC_COUNT] =
  {
    { 0x00, BEXKAT1_F0_NARG, BEXKAT1_ADDR_INH, "nop" },
    { 0x02, BEXKAT1_F0_NARG, BEXKAT1_ADDR_INH, "rts" },
    { 0x03, BEXKAT1_F0_NARG, BEXKAT1_ADDR_INH, "rti" },
    { 0x04, BEXKAT1_F0_A,    BEXKAT1_ADDR_REG, "inc" },
    { 0x05, BEXKAT1_F0_A,    BEXKAT1_ADDR_REG, "dec" }, 
    { 0x00, BEXKAT1_F1_ABC,  BEXKAT1_ADDR_REG, "and" },
    { 0x01, BEXKAT1_F1_ABC,  BEXKAT1_ADDR_REG, "or"  },
    { 0x02, BEXKAT1_F1_ABC,  BEXKAT1_ADDR_REG, "add" },
    { 0x03, BEXKAT1_F1_ABC,  BEXKAT1_ADDR_REG, "sub" },
    { 0x04, BEXKAT1_F1_AB,   BEXKAT1_ADDR_REG, "lsl" },
    { 0x05, BEXKAT1_F1_AB,   BEXKAT1_ADDR_REG, "asr" },
    { 0x06, BEXKAT1_F1_ABC,  BEXKAT1_ADDR_REG, "lsr" },
    { 0x07, BEXKAT1_F1_AB,   BEXKAT1_ADDR_REG, "xor" },
    { 0x10, BEXKAT1_F1_AB,   BEXKAT1_ADDR_REG, "mov" },
    { 0x11, BEXKAT1_F1_AB,   BEXKAT1_ADDR_REG, "cmp" },
    { 0x20, BEXKAT1_F1_AB,   BEXKAT1_ADDR_IND, "st.l" },
    { 0x21, BEXKAT1_F1_AB,   BEXKAT1_ADDR_IND, "st" },
    { 0x22, BEXKAT1_F1_AB,   BEXKAT1_ADDR_IND, "st.b" },
    { 0x23, BEXKAT1_F1_AB,   BEXKAT1_ADDR_IND, "ld.l" },
    { 0x24, BEXKAT1_F1_AB,   BEXKAT1_ADDR_IND, "ld" },
    { 0x25, BEXKAT1_F1_AB,   BEXKAT1_ADDR_IND, "ld.b" },
    { 0x20, BEXKAT1_F2_RELADDR, BEXKAT1_ADDR_PCREL, "bra" },  
    { 0x21, BEXKAT1_F2_RELADDR, BEXKAT1_ADDR_PCREL, "beq" },  
    { 0x22, BEXKAT1_F2_RELADDR, BEXKAT1_ADDR_PCREL, "bne" },  
    { 0x24, BEXKAT1_F2_RELADDR, BEXKAT1_ADDR_PCREL, "bgt" },  
    { 0x25, BEXKAT1_F2_RELADDR, BEXKAT1_ADDR_PCREL, "bge" },  
    { 0x26, BEXKAT1_F2_RELADDR, BEXKAT1_ADDR_PCREL, "ble" },  
    { 0x27, BEXKAT1_F2_RELADDR, BEXKAT1_ADDR_PCREL, "blt" },  
    { 0x2c, BEXKAT1_F2_RELADDR, BEXKAT1_ADDR_PCREL, "brn" },
    { 0x10, BEXKAT1_F3_A_ABSADDR, BEXKAT1_ADDR_DIR, "ld.l" }, 
    { 0x11, BEXKAT1_F3_A_ABSADDR, BEXKAT1_ADDR_DIR, "ld" }, 
    { 0x12, BEXKAT1_F3_A_ABSADDR, BEXKAT1_ADDR_DIR, "ld.b" }, 
    { 0x13, BEXKAT1_F3_A_ABSADDR, BEXKAT1_ADDR_DIR, "st.l" },
    { 0x14, BEXKAT1_F3_A_ABSADDR, BEXKAT1_ADDR_DIR, "st" }, 
    { 0x15, BEXKAT1_F3_A_ABSADDR, BEXKAT1_ADDR_DIR, "st.b" },
    { 0x16, BEXKAT1_F3_ABSADDR, BEXKAT1_ADDR_DIR, "jmp" },
    { 0x17, BEXKAT1_F3_ABSADDR, BEXKAT1_ADDR_DIR, "jsr" },
    { 0x20, BEXKAT1_F3_A_32V, BEXKAT1_ADDR_IMM, "ldi" }
  };
