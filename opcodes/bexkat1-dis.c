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
find_opcode(unsigned mode, uint8_t opcode) {
  int i;
  for (i=0; i < bexkat1_opc_count; i++)
    if (bexkat1_opc_info[i].addr_mode == mode &&
	bexkat1_opc_info[i].opcode == opcode)
      return &bexkat1_opc_info[i];
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
  unsigned short op2;
  short imm;
  unsigned int addr;
  int imm32;

  stream = info->stream;
  fpr = info->fprintf_func; 

  if ((status = info->read_memory_func(memaddr, buffer, 2, info)))
    goto fail;
  iword = bfd_getb16(buffer);

  opcode = find_opcode((iword >> 13), (iword >> 5) & 0xff);
  if (opcode == NULL)
    abort();

  switch (opcode->addr_mode) {
  case BEXKAT1_ADDR_INH:
    fpr(stream, "%s", opcode->name);
    length = 2;
    break;
  case BEXKAT1_ADDR_IMM:
    if (opcode->size == 2) {
      if ((status = info->read_memory_func(memaddr+2, buffer, 2, info)))
	goto fail;
      imm = bfd_getb16(buffer);
      fpr(stream, "%s %%%d, 0x%04x", opcode->name, (iword & 0x1f), imm & 0xffff);
      length = 4;
    } else {
      if (!strcmp("ldi",opcode->name)) {
	if ((status = info->read_memory_func(memaddr+2, buffer, 4, info)))
	  goto fail;
	imm32 = bfd_getb32(buffer);
	fpr(stream, "%s %%%d, 0x%08x", opcode->name, (iword & 0x1f),
	    imm32 & 0xffffffff);
	length = 6;
      } else {
	if ((status = info->read_memory_func(memaddr+2, buffer, 2, info)))
	  goto fail;
	op2 = bfd_getb16(buffer);
	if ((status = info->read_memory_func(memaddr+4, buffer, 2, info)))
	  goto fail;
	imm = bfd_getb16(buffer);
	fpr(stream, "%s %%%d, %%%d, 0x%04x", opcode->name, (iword & 0x1f),
	    (op2 >> 8) & 0x1f, imm & 0xffff);
	length = 6;
      }
    }
    break;
  case BEXKAT1_ADDR_REGIND:
    if ((status = info->read_memory_func(memaddr+2, buffer, 2, info)))
      goto fail;
    imm = bfd_getb16(buffer);
    if (!strcmp("jsr", opcode->name) ||
        !strcmp("jmp", opcode->name)) {
      fpr(stream, "%s %d(%%%d)", opcode->name,
  	  (short)(imm & 0x400 ? 0xf800 | (imm & 0x7ff) : imm & 0x7ff),
	  (imm >> 11) & 0x1f);
    } else {
      fpr(stream, "%s %%%d, %d(%%%d)", opcode->name, (iword & 0x1f),
  	  (short)(imm & 0x400 ? 0xf800 | (imm & 0x7ff) : imm & 0x7ff),
	  (imm >> 11) & 0x1f);
    }
    length = 4;
    break;
  case BEXKAT1_ADDR_REG:
    if (opcode->size == 1) {
      fpr(stream, "%s %%%d", opcode->name, (iword & 0x1f));
      length = 2;
    } else {
      if ((status = info->read_memory_func(memaddr+2, buffer, 2, info)))
	goto fail;
      op2 = bfd_getb16(buffer);
      if (!strcmp("mov", opcode->name) ||
	  !strcmp("cmp", opcode->name) ||
	  !strcmp("com", opcode->name) ||
	  !strcmp("neg", opcode->name)) {
	fpr(stream, "%s %%%d, %%%d", opcode->name, (iword & 0x1f),
	    (op2 >> 8) & 0x1f);
      } else {
	fpr(stream, "%s %%%d, %%%d, %%%d", opcode->name, (iword & 0x1f),
	    (op2 >> 8) & 0x1f, op2 & 0x1f);
      }
      length = 4;
    }
    break;
  case BEXKAT1_ADDR_DIR:
    if (!strcmp("jmp", opcode->name) ||
	!strcmp("jsr", opcode->name)) {
      if ((status = info->read_memory_func(memaddr+2, buffer, 4, info)))
	goto fail;
      addr = bfd_getb32(buffer);
      fpr(stream, "%s 0x%08x", opcode->name, addr);
      length = 6;
    } else {
      if ((status = info->read_memory_func(memaddr+2, buffer, 4, info)))
	goto fail;
      imm32 = bfd_getb32(buffer);
      fpr(stream, "%s %%%d, 0x%08x", opcode->name, (iword & 0x1f), imm32);
      length = 6;
    }
    break;
  case BEXKAT1_ADDR_PCIND:
    if ((status = info->read_memory_func(memaddr+2, buffer, 2, info)))
      goto fail;
    imm = bfd_getb16(buffer);
    fpr(stream, "%s %d", opcode->name, imm);
    length = 4;
    break;
  default:
    abort();
    break;
  }

  return length;

fail:
  info->memory_error_func(status, memaddr, info);
  return -1;
}
