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

#define BEXKAT1_ADDR_INH    0
#define BEXKAT1_ADDR_IMM    1
#define BEXKAT1_ADDR_REGIND 2
#define BEXKAT1_ADDR_REG    3
#define BEXKAT1_ADDR_DIR    4
#define BEXKAT1_ADDR_PCIND  5

/*
 * Examples of different addressing modes and syntax:
 * + Inherent (1 word):         nop
 * Register direct (1 word):  inc  r5
 * + Register direct (2 words): mov  r3,  r5
 * + Register direct (2 words): add  r3,  r5, r18
 * + Register index (2 words):  st.l r3,  -4(r13)
 * + PC index (2 words):        bra  0xff30
 * Immediate (2 words):       add  r3,  0xabcd
 * + Immediate (3 words):       add  r3,  r4, 0xabcd
 * + Immediate (3 words):       ldi  r45, 0xabcd0123

 * +Direct (3 words):          jmp  0xabcd1945
 * + Direct (3 words):          st.l r3,  0xabcd1234
 *
 * Format of opcode words:
 * word 1: aaaoooooooorrrrr (addr mode[15:13], opcode[12:5], regA[4:0])
 * word 2: 000bbbbb000ccccc (regB[12:8], regC[4:0]) register direct
 * word 2: vvvvvvvvvvvvvvvv (value[15:0]) 2 word immediate, PC index
 * word 2: hhhhhhhhhhhhhhhh (high word[15:0]) 3 word immediate, direct
 * word 2: bbbbbvvvvvvvvvvv (regB[15:11], index[10:0] register index
 * word 3: llllllllllllllll (low word[15:0]) 3 word immediate, direct 
 */



typedef struct bexkat1_opcode
{
  uint8_t opcode;
  unsigned int size;
  unsigned int addr_mode;
  const char *name;
} bexkat1_opc_info_t;

extern const bexkat1_opc_info_t bexkat1_opc_info[];
extern const int bexkat1_opc_count;

#endif /* _OPCODE_BEXKAT1_H */
