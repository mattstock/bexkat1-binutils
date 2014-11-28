/* bexkat1.h -- Header file for Bexkat 1 opcode table
   Copyright (C) 1999-2014 Free Software Foundation, Inc.
   Written by Matt Stock (stock@bexkat.com)

   This file is part of GDB, GAS, and the GNU binutils.

   GDB, GAS, and the GNU binutils are free software; you can redistribute
   them and/or modify them under the terms of the GNU General Public
   License as published by the Free Software Foundation; either version 3,
   or (at your option) any later version.

   GDB, GAS, and the GNU binutils are distributed in the hope that they
   will be useful, but WITHOUT ANY WARRANTY; without even the implied
   warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
   the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this file; see the file COPYING3.  If not, write to the Free
   Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#ifndef _OPCODE_BEXKAT1_H
#define _OPCODE_BEXKAT1_H

#include <stdint.h>

/* Opcode format is determined by the high two bits of the first word
   Form 0 is in a single 16-bit word:
   00xo oooo oooa aaaa */
#define BEXKAT1_F0_NARG      0x000  /* Form 0 no args */
#define BEXKAT1_F0_A         0x001  /* Form 0 single register */
/* Form 1 is in two words and has up to 3 registers:
   01xo oooo oooa aaaa xxxb bbbb xxxc cccc */
#define BEXKAT1_F1_AB        0x100  /* Form 1 two registers */
#define BEXKAT1_F1_ABC       0x101  /* Form 1 three register */
/* Form 2 is in two words and uses an immediate value and/or a register:
   10xo oooo oooa aaaa iiii iiii iiii iiii */ 
#define BEXKAT1_F2_A_16V     0x200  /* Form 2 w/16-bit immediate operand */
#define BEXKAT1_F2_A_RELADDR 0x201  /* Form 2 w/PC offset */ 
/* Form 3 is in three words and uses an 32-bit address and/or a register:
   11xo oooo oooa aaaa llll llll llll llll hhhh hhhh hhhh hhhh */
#define BEXKAT1_F3_A_32V     0x300  /* Form 3 w/32-bit immediate operand */
#define BEXKAT1_F3_A_ABSADDR 0x301  /* Form 3 w/absolute address */

typedef struct bexkat1_opcode
{
  uint8_t opcode;
  unsigned itype;
  const char *   name;     /* Op-code name.  */ 
} bexkat1_opc_info_t;

extern const bexkat1_opc_info_t bexkat1_form0_opc_info[6];
extern const bexkat1_opc_info_t bexkat1_form1_opc_info[10];
extern const bexkat1_opc_info_t bexkat1_form2_opc_info[32];
extern const bexkat1_opc_info_t bexkat1_form3_opc_info[9];

#endif /* _OPCODE_BEXKAT1_H */
