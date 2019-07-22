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

#define BEXKAT1_INH    0
#define BEXKAT1_PUSH   1
#define BEXKAT1_POP    2
#define BEXKAT1_CMP    3
#define BEXKAT1_MOV    4
#define BEXKAT1_FP     5
#define BEXKAT1_ALU    6
#define BEXKAT1_INT    7
#define BEXKAT1_LDI    8
#define BEXKAT1_LOAD   9
#define BEXKAT1_STORE  10
#define BEXKAT1_BRANCH 11
#define BEXKAT1_JUMP   12
#define BEXKAT1_INTU   13
#define BEXKAT1_FPU    14

/*
 * tttt oooo aaaa bbbb cccc xxxx xxx0
 * tttt oooo aaaa bbbb vvvv vvvv vvv0
 * tttt oooo aaaa xxxx xxxx xxxx xxx1 vvvvvvvvvvvvvvvv vvvvvvvvvvvvvvvv
 */

typedef struct bexkat1_opcode
{
  uint8_t type;
  uint8_t opcode;
  unsigned int size;
  unsigned int args;
  const char *name;
} bexkat1_opc_info_t;

extern const bexkat1_opc_info_t bexkat1_opc_info[];
extern const int bexkat1_opc_count;

#endif /* _OPCODE_BEXKAT1_H */
