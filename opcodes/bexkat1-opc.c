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

const bexkat1_opc_info_t bexkat1_form0_opc_info[BEXKAT1_FORM0_COUNT] =
  {
    { 0x00, BEXKAT1_F0_NARG, "nop" }, // done
    { 0x02, BEXKAT1_F0_NARG, "rts" }, // done
    { 0x03, BEXKAT1_F0_NARG, "rti" },
    { 0x04, BEXKAT1_F0_A,    "inc" }, // done
    { 0x05, BEXKAT1_F0_A,    "dec" }  // done
  };

const bexkat1_opc_info_t bexkat1_form1_opc_info[BEXKAT1_FORM1_COUNT] =
  {
    { 0x00, BEXKAT1_F1_ABC,  "and" }, // done
    { 0x01, BEXKAT1_F1_ABC,  "or"  }, // done
    { 0x02, BEXKAT1_F1_ABC,  "add" }, // done
    { 0x03, BEXKAT1_F1_ABC,  "sub" }, // done
    { 0x04, BEXKAT1_F1_AB,   "lsl" }, // done
    { 0x05, BEXKAT1_F1_AB,   "asr" }, // done
    { 0x06, BEXKAT1_F1_ABC,  "lsr" }, // done
    { 0x07, BEXKAT1_F1_AB,   "xor" }, // done
    { 0x10, BEXKAT1_F1_AB,   "mov" }, // done
    { 0x11, BEXKAT1_F1_AB,   "cmp" }  // done
  };

const bexkat1_opc_info_t bexkat1_form2_opc_info[BEXKAT1_FORM2_COUNT] =
  {
    { 0x00, BEXKAT1_F2_A_16V,     "and.i" }, // done
    { 0x01, BEXKAT1_F2_A_16V,     "or.i"  }, // done
    { 0x02, BEXKAT1_F2_A_16V,     "add.i" }, // done
    { 0x03, BEXKAT1_F2_A_16V,     "sub.i" }, // done
    { 0x07, BEXKAT1_F2_A_16V,     "xor.i" }, // done
    { 0x10, BEXKAT1_F2_A_16V,     "ld.i"  }, // done
    { 0x11, BEXKAT1_F2_A_RELADDR, "ld.sp" }, 
    { 0x12, BEXKAT1_F2_A_RELADDR, "st.sp" },
    { 0x20, BEXKAT1_F2_A_RELADDR, "bra" },   // done
    { 0x21, BEXKAT1_F2_A_RELADDR, "beq" },   // done
    { 0x22, BEXKAT1_F2_A_RELADDR, "bne" },   // done
    { 0x23, BEXKAT1_F2_A_RELADDR, "bsr" },
    { 0x24, BEXKAT1_F2_A_RELADDR, "bgt" },   // done
    { 0x25, BEXKAT1_F2_A_RELADDR, "bge" },   // done
    { 0x26, BEXKAT1_F2_A_RELADDR, "ble" },   // done
    { 0x27, BEXKAT1_F2_A_RELADDR, "blt" },   // done
    { 0x28, BEXKAT1_F2_A_RELADDR, "bhi" },
    { 0x29, BEXKAT1_F2_A_RELADDR, "bls" },
    { 0x2a, BEXKAT1_F2_A_RELADDR, "bhs" },
    { 0x2b, BEXKAT1_F2_A_RELADDR, "blo" },
    { 0x2c, BEXKAT1_F2_A_RELADDR, "brn" },
    { 0x2d, BEXKAT1_F2_A_RELADDR, "bmi" },
    { 0x2e, BEXKAT1_F2_A_RELADDR, "bvs" },
    { 0x2f, BEXKAT1_F2_A_RELADDR, "bpl" }
  };

const bexkat1_opc_info_t bexkat1_form3_opc_info[BEXKAT1_FORM3_COUNT] =
  {
    { 0x00, BEXKAT1_F3_A_ABSADDR, "and.a" },
    { 0x01, BEXKAT1_F3_A_ABSADDR, "or.a" },
    { 0x02, BEXKAT1_F3_A_ABSADDR, "add.a" },
    { 0x03, BEXKAT1_F3_A_ABSADDR, "sub.a"  },
    { 0x07, BEXKAT1_F3_A_ABSADDR, "xor.a" },
    { 0x10, BEXKAT1_F3_A_ABSADDR, "ld.la" }, 
    { 0x11, BEXKAT1_F3_A_ABSADDR, "ld" },    // done
    { 0x12, BEXKAT1_F3_A_ABSADDR, "ld.ca" }, 
    { 0x13, BEXKAT1_F3_A_ABSADDR, "st.la" },
    { 0x14, BEXKAT1_F3_A_ABSADDR, "st" },    // done
    { 0x15, BEXKAT1_F3_A_ABSADDR, "st.ca" },
    { 0x16, BEXKAT1_F3_ABSADDR,   "jmp" },  // done
    { 0x17, BEXKAT1_F3_ABSADDR,   "jsr" },  // done
    { 0x20, BEXKAT1_F3_A_32V,     "ld.l"},  // done
    { 0x30, BEXKAT1_F3_A_32V,     "and.l"},
    { 0x31, BEXKAT1_F3_A_32V,     "or.l" },
    { 0x32, BEXKAT1_F3_A_32V,     "add.l" },
    { 0x33, BEXKAT1_F3_A_32V,     "sub.l"  },
    { 0x37, BEXKAT1_F3_A_32V,     "xor.l" }
  };
