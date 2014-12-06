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

static const bexkat1_opc_info_t *
find_opcode(int form, uint8_t opcode) {
  int max;
  const bexkat1_opc_info_t *ops;

  switch (form) {
  case 0:
    max = BEXKAT1_FORM0_COUNT;
    ops = bexkat1_form0_opc_info;
    break;
  case 1:
    max = BEXKAT1_FORM1_COUNT;
    ops = bexkat1_form1_opc_info;
    break;
  case 2:
    max = BEXKAT1_FORM2_COUNT;
    ops = bexkat1_form2_opc_info;
    break;
  case 3:
    max = BEXKAT1_FORM3_COUNT;
    ops = bexkat1_form3_opc_info;
    break;
  default:
    return NULL;
  }

  int i;
  for (i=0; i < max; i++)
    if (ops[i].opcode == opcode)
      return &ops[i];
  return NULL;
}


/* Disassemble one instruction at address 'memaddr'.  Returns the number
   of bytes used by that instruction.  */
int print_insn_bexkat1 (bfd_vma memaddr, struct disassemble_info* info) {
  int status;
  int length = 2;
  const bexkat1_opc_info_t *opcode;
  bfd_byte buffer[6];
  unsigned short iword;

  stream = info->stream;
  fpr = info->fprintf_func; 

  if ((status = info->read_memory_func(memaddr, buffer, 2, info)))
    goto fail;
  iword = bfd_getb16(buffer);

  switch (iword >> 14) {
  case 0:
    opcode = find_opcode(0, (iword >> 5) & 0xff);
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
    opcode = find_opcode(1, (iword >> 5) & 0xff);
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
    opcode = find_opcode(2, (iword >> 5) & 0xff);
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
      {
	short imm;
	
	if ((status = info->read_memory_func(memaddr+2, buffer, 2, info)))
	  goto fail;
	imm = bfd_getb16(buffer);
	fpr(stream, "%s %d", opcode->name, imm);
	length = 4;
      }
      break;
    default:
      abort();
    }
    break;
  case 3:
    opcode = find_opcode(3, (iword >> 5) & 0xff);
    length = 6;
    switch (opcode->itype) {
    case BEXKAT1_F3_ABSADDR:
      {
	unsigned int addr;

	if ((status = info->read_memory_func(memaddr+2, buffer, 4, info)))
	  goto fail;
	addr = bfd_getb32(buffer);
	fpr(stream, "%s r%d, 0x%08x", opcode->name, (iword & 0x1f),
	    addr);
      }
      break;
    case BEXKAT1_F3_A_ABSADDR:
      {
	unsigned int addr;
	
	if ((status = info->read_memory_func(memaddr+2, buffer, 4, info)))
	  goto fail;
	addr = bfd_getb32(buffer);
	fpr(stream, "%s r%d, 0x%08x", opcode->name, (iword & 0x1f),
	    addr);
      }
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
