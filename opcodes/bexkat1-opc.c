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

const bexkat1_opc_info_t bexkat1_form0_opc_info[6] =
  {
    { 0x00, BEXKAT1_F0_NARG, "nop" },
    { 0x01, BEXKAT1_F0_A,    "neg" },
    { 0x02, BEXKAT1_F0_NARG, "rts" },
    { 0x03, BEXKAT1_F0_NARG, "rti" },
    { 0x04, BEXKAT1_F0_A,    "inc" },
    { 0x05, BEXKAT1_F0_A,    "dec" }
  };

const bexkat1_opc_info_t bexkat1_form1_opc_info[10] =
  {
    { 0x00, BEXKAT1_F1_ABC,  "add" },
    { 0x01, BEXKAT1_F1_ABC,  "sub" },
    { 0x02, BEXKAT1_F1_ABC,  "and" },
    { 0x03, BEXKAT1_F1_ABC,  "or"  },
    { 0x04, BEXKAT1_F1_AB,   "lsl" },
    { 0x05, BEXKAT1_F1_AB,   "lsr" },
    { 0x06, BEXKAT1_F1_ABC,  "xor" },
    { 0x07, BEXKAT1_F1_AB,   "asr" },
    { 0x08, BEXKAT1_F1_AB,   "mov" },
    { 0x09, BEXKAT1_F1_AB,   "cmp" }
  };

const bexkat1_opc_info_t bexkat1_form2_opc_info[32] =
  {
    { 0x00, BEXKAT1_F2_A_16V,     "addi" },
    { 0x01, BEXKAT1_F2_A_16V,     "subi" },
    { 0x02, BEXKAT1_F2_A_16V,     "andi" },
    { 0x03, BEXKAT1_F2_A_16V,     "ori"  },
    { 0x04, BEXKAT1_F2_A_16V,     "bad" },
    { 0x05, BEXKAT1_F2_A_16V,     "bad" },
    { 0x06, BEXKAT1_F2_A_16V,     "xori" },
    { 0x07, BEXKAT1_F2_A_16V,     "ldi" },
    { 0x08, BEXKAT1_F2_A_RELADDR, "str" },
    { 0x09, BEXKAT1_F2_A_16V,     "bad" },
    { 0x0a, BEXKAT1_F2_A_16V,     "bad" },
    { 0x0b, BEXKAT1_F2_A_16V,     "bad" },
    { 0x0c, BEXKAT1_F2_A_16V,     "bad" },
    { 0x0d, BEXKAT1_F2_A_16V,     "bad" },
    { 0x0e, BEXKAT1_F2_A_16V,     "bad" },
    { 0x0f, BEXKAT1_F2_A_16V,     "bad" },
    { 0x10, BEXKAT1_F2_A_RELADDR, "bra" },
    { 0x11, BEXKAT1_F2_A_RELADDR, "beq" },
    { 0x12, BEXKAT1_F2_A_RELADDR, "bne" },
    { 0x13, BEXKAT1_F2_A_RELADDR, "bsr" },
    { 0x14, BEXKAT1_F2_A_RELADDR, "bgt" },
    { 0x15, BEXKAT1_F2_A_RELADDR, "bge" },
    { 0x16, BEXKAT1_F2_A_RELADDR, "ble" },
    { 0x17, BEXKAT1_F2_A_RELADDR, "blt" },
    { 0x18, BEXKAT1_F2_A_RELADDR, "bhi" },
    { 0x19, BEXKAT1_F2_A_RELADDR, "bls" },
    { 0x1a, BEXKAT1_F2_A_RELADDR, "bhs" },
    { 0x1b, BEXKAT1_F2_A_RELADDR, "blo" },
    { 0x1c, BEXKAT1_F2_A_RELADDR, "brn" },
    { 0x1d, BEXKAT1_F2_A_RELADDR, "bmi" },
    { 0x1e, BEXKAT1_F2_A_RELADDR, "bvs" },
    { 0x1f, BEXKAT1_F2_A_RELADDR, "bpl" }
  };

const bexkat1_opc_info_t bexkat1_form3_opc_info[9] =
  {
    { 0x00, BEXKAT1_F3_A_ABSADDR, "add" },
    { 0x01, BEXKAT1_F3_A_ABSADDR, "sub" },
    { 0x02, BEXKAT1_F3_A_ABSADDR, "and" },
    { 0x03, BEXKAT1_F3_A_ABSADDR, "or"  },
    { 0x06, BEXKAT1_F3_A_ABSADDR, "xor" },
    { 0x07, BEXKAT1_F3_A_ABSADDR, "ld" },
    { 0x08, BEXKAT1_F3_A_ABSADDR, "st" },
    { 0x09, BEXKAT1_F3_ABSADDR,   "jmp" },
    { 0x0a, BEXKAT1_F3_ABSADDR,   "jsr" }
  };
