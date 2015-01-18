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
    if (bexkat1_opc_info[i].mode == mode &&
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
  unsigned short iword, iword2;
  short imm;
  int imm32;

  stream = info->stream;
  fpr = info->fprintf_func; 

  if ((status = info->read_memory_func(memaddr, buffer, 2, info)))
    goto fail;
  iword = bfd_getb16(buffer);

  iword2 = 0; // just to shut up the warnings

  // parse the opcode, sometimes part of it is in the 2nd word
  switch (iword >> 13) {
  case BEXKAT1_INH:
    opcode = find_opcode((iword >> 13), (iword >> 5) & 0xe0);
    break;
  case BEXKAT1_IMM2:
  case BEXKAT1_DIR:
  case BEXKAT1_IMM3a:
    opcode = find_opcode((iword >> 13), (iword >> 5) & 0xff);
    break;
  case BEXKAT1_INH2:
  case BEXKAT1_IMM3:
  case BEXKAT1_REGIND:
  case BEXKAT1_REG:
    if ((status = info->read_memory_func(memaddr+2, buffer, 2, info)))
      goto fail;
    iword2 = bfd_getb16(buffer);
    opcode = find_opcode((iword >> 13), ((iword >> 5) & 0xe0) | (iword2 >> 11));
    break;
  }

  if (opcode == NULL)
    abort();

  switch (opcode->mode) {
  case BEXKAT1_INH:
    if (opcode->args == 0)
      fpr(stream, "%s", opcode->name);
    if (opcode->args == 1)
      fpr(stream, "%s %%%d", opcode->name, (iword & 0x1f));
    if (opcode->args == 2)
      fpr(stream, "%s %%%d, %%%d", opcode->name, (iword & 0x1f),
	  ((iword >> 5) & 0x1f));
    length = 2;
    break;
  case BEXKAT1_INH2:
    if (opcode->args == 0)
      fpr(stream, "%s", opcode->name);
    if (opcode->args == 1)
      fpr(stream, "%s %%%d", opcode->name, (iword & 0x1f));
    if (opcode->args == 2)
      fpr(stream, "%s %%%d, %%%d", opcode->name, (iword & 0x1f),
	  ((iword >> 5) & 0x1f));
    length = 4;
    break;
  case BEXKAT1_IMM3:
    if ((status = info->read_memory_func(memaddr+4, buffer, 2, info)))
      goto fail;
    imm = bfd_getb16(buffer);
    fpr(stream, "%s %%%d, %%%d, 0x%04x", opcode->name, (iword & 0x1f), 
	((iword >> 5) & 0x1f), imm & 0xffff);
    length = 6;
    break;
  case BEXKAT1_IMM3a:
    if ((status = info->read_memory_func(memaddr+2, buffer, 4, info)))
      goto fail;
    imm32 = bfd_getb32(buffer);
    fpr(stream, "%s %%%d, 0x%08x", opcode->name, (iword & 0x1f),
	imm32 & 0xffffffff);
    length = 6;
    break;
  case BEXKAT1_DIR:
    if ((status = info->read_memory_func(memaddr+2, buffer, 4, info)))
      goto fail;
    imm32 = bfd_getb32(buffer);
    if (opcode->args == 1)
      fpr(stream, "%s 0x%08x", opcode->name, imm32);
    else
      fpr(stream, "%s %%%d, 0x%08x", opcode->name, (iword & 0x1f), imm32);
    length = 6;
    break;
  case BEXKAT1_IMM2:
    if ((status = info->read_memory_func(memaddr+2, buffer, 2, info)))
      goto fail;
    imm = bfd_getb16(buffer);
    if (opcode->args == 1)
      fpr(stream, "%s %d", opcode->name, imm);
    else
      fpr(stream, "%s %%%d, %d", opcode->name, (iword & 0x1f), imm);
    length = 4;
    break;
  case BEXKAT1_REG:
    fpr(stream, "%s %%%d, %%%d, %%%d", opcode->name, (iword & 0x1f),
	(iword >> 5) & 0x1f, iword2 & 0x1f);
    length = 4;
    break;
  case BEXKAT1_REGIND:
    if (opcode->args == 2)
      fpr(stream, "%s %d(%%%d)", opcode->name,
  	  (short)(iword2 & 0x400 ? 0xf800 | (iword2 & 0x7ff) : iword2 & 0x7ff),
	  (iword >> 5) & 0x1f);
    else 
      fpr(stream, "%s %%%d, %d(%%%d)", opcode->name, (iword & 0x1f),
  	  (short)(iword2 & 0x400 ? 0xf800 | (iword2 & 0x7ff) : iword2 & 0x7ff),
	  (iword >> 5) & 0x1f);
    length = 4;
    break;
  }

  return length;

fail:
  info->memory_error_func(status, memaddr, info);
  return -1;
}
