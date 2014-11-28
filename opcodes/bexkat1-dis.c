/* bexkat1-dis.c -- Bexkat1 disassembly
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
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include <stdio.h>
#include <stdint.h>

#define STATIC_TABLE
#define DEFINE_TABLE

#include "opcode/bexkat1.h"
#include "dis-asm.h"

static fprintf_ftype fpr;
static void *stream;

/* Prototypes for local functions.  */
int print_insn_bexkat1 (bfd_vma, struct disassemble_info *);

/* Disassemble one instruction at address 'memaddr'.  Returns the number
   of bytes used by that instruction.  */
int print_insn_bexkat1 (bfd_vma memaddr, struct disassemble_info* info) {
  int status;
  int length = 2;
  const bexkat1_opc_info_t *opcode;
  bfd_byte buffer[4];
  unsigned short iword;

  stream = info->stream;
  fpr = info->fprintf_func; 

  if ((status = info->read_memory_func(memaddr, buffer, 2, info)))
    goto fail;
  iword = bfd_getb16(buffer);

  fprintf(stderr, "iword = %x, shifted = %x\n", iword, iword >> 14);

  switch (iword >> 14) {
  case 0:
    opcode = &bexkat1_form0_opc_info[(iword >> 5) & 0xff];
    switch (opcode->itype) {
    case BEXKAT1_F0_NARG:
      fpr(stream, "%s", opcode->name);
      break;
    case BEXKAT1_F0_A:
      fpr(stream, "%s r%d", opcode->name, (iword & 0x1f));
      break;
    default:
      abort();
    }
    break;
  case 1:
    opcode = &bexkat1_form1_opc_info[(iword >> 5) & 0xff];
    switch (opcode->itype) {
    case BEXKAT1_F1_AB:
      {
	unsigned short op2;

	if ((status = info->read_memory_func(memaddr+2, buffer, 2, info)))
	  goto fail;
	op2 = bfd_getb16(buffer);
	fpr(stream, "%s r%d, r%d", opcode->name, (iword & 0x1f),
	    (op2 >> 8) & 0x1f);
	length = 4;
      }
      break;
    case BEXKAT1_F1_ABC:
      {
	unsigned short op2;

	if ((status = info->read_memory_func(memaddr+2, buffer, 2, info)))
	  goto fail;
	op2 = bfd_getb16(buffer);
	fpr(stream, "%s r%d, r%d, r%d", opcode->name, (iword & 0x1f),
	    (op2 >> 8) & 0x1f, op2 & 0x1f);
	length = 4;
      }
      break;
    default:
      abort();
    }
    break;
  case 2:
    opcode = &bexkat1_form2_opc_info[(iword >> 5) & 0xff];
    switch (opcode->itype) {
    case BEXKAT1_F2_A_16V:
      {
	unsigned imm;
	
	if ((status = info->read_memory_func(memaddr+2, buffer, 2, info)))
	  goto fail;
	imm = bfd_getb16(buffer);
	fpr(stream, "%s r%d, 0x%x", opcode->name, (iword & 0x1f), imm);
	length = 4;
      }
      break;
    case BEXKAT1_F2_A_RELADDR:
      fpr(stream, "%s reladdr", opcode->name);
      break;
    default:
      abort();
    }
    break;
  case 3:
    opcode = &bexkat1_form3_opc_info[(iword >> 5) & 0xff];
    switch (opcode->itype) {
    case BEXKAT1_F3_A_32V:
      fpr(stream, "%s r%d, #hhhhllll", opcode->name, (iword & 0x1f));
      break;
    case BEXKAT1_F3_A_ABSADDR:
      fpr(stream, "%s r%d, address", opcode->name, (iword & 0x1f));
      break;
    default:
      abort();
    }
    break;
  }

  return length;

fail:
  info->memory_error_func(status, memaddr, info);
  return -1;
}
