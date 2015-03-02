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

#define BEXKAT1_REG    0
#define BEXKAT1_REGIND 1
#define BEXKAT1_IMM    2
#define BEXKAT1_DIR    4

/*
 * REG:    000ooooooooaaaaa bbbbbccccc000000
 * REGIND: 001ooooooooaaaaa bbbbbvvvvvvvvvvv
 * IMM:    010ooooooooaaaaa vvvvvvvvvvvvvvvv
 * DIR:    100ooooooooaaaaa bbbbbccccc000000 vvvvvvvvvvvvvvvv vvvvvvvvvvvvvvvv
 */

typedef struct bexkat1_opcode
{
  uint8_t opcode;
  unsigned int args;
  unsigned int mode;
  const char *name;
} bexkat1_opc_info_t;

extern const bexkat1_opc_info_t bexkat1_opc_info[];
extern const int bexkat1_opc_count;

#endif /* _OPCODE_BEXKAT1_H */
