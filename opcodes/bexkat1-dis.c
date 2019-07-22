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
static uint32_t get_offset(unsigned int w);


static const bexkat1_opc_info_t *
find_opcode(uint32_t w) {
  uint8_t type = (w >> 28) & 0xf;
  uint8_t opcode = (w >> 24) & 0xf;
  uint8_t s = w & 0x1;

  int i;
  for (i=0; i < bexkat1_opc_count; i++)
    if (bexkat1_opc_info[i].type == type &&
	bexkat1_opc_info[i].opcode == opcode &&
        bexkat1_opc_info[i].size == s)
      return &bexkat1_opc_info[i];
  return NULL;
}

static char *print_reg_name(int regno) {
  static char *list[] = { "%0", "%1", "%2", "%3",
			  "%4", "%5", "%6", "%7",
			  "%8", "%9", "%10", "%11",
			  "%12", "%13", "%fp", "%sp" };
  
  return list[regno];
}

static uint32_t get_offset(unsigned int w) {
  uint32_t offset = ((w >> 1) & 0x7fff);
  if (offset & 0x00004000)
    offset |=  0xffff8000;
  return offset;
}

/* Disassemble one instruction at address 'memaddr'.  Returns the number
   of bytes used by that instruction.  */
int print_insn_bexkat1 (bfd_vma memaddr, struct disassemble_info* info) {
  int status;
  int length = 4;
  const bexkat1_opc_info_t *opcode;
  bfd_byte buffer[6];
  unsigned int iword;
  int imm32;
  int offset;

  stream = info->stream;
  fpr = info->fprintf_func; 

  if ((status = info->read_memory_func(memaddr, buffer, 4, info)))
    goto fail;
  if (info->endian == BFD_ENDIAN_BIG)
    iword = bfd_getb32(buffer);
  else
    iword = bfd_getl32(buffer);    

  opcode = find_opcode(iword);
  if (opcode == NULL)
    abort();

  offset = get_offset(iword);

  /* grab the second word if it's a two word instruction */
  if (opcode->size) {
    if ((status = info->read_memory_func(memaddr+4, buffer, 4, info)))
      goto fail;
    if (info->endian == BFD_ENDIAN_BIG)
      imm32 = bfd_getb32(buffer);
    else
      imm32 = bfd_getl32(buffer);
    length = 8;
  } else {
    imm32 = 0;
    length = 4;
  }

  switch (opcode->type) {
  case BEXKAT1_INH:
    if (opcode->args == 0)
      fpr(stream, "%s", opcode->name);
    if (opcode->args == 1) {
      fpr(stream, "%s ", opcode->name);
      if (opcode->size)
        info->print_address_func((bfd_vma) imm32, info);
      else
	fpr(stream, "%d", offset);
    }
    break;
  case BEXKAT1_PUSH:
    if (opcode->opcode == 2 && opcode->args == 1) {
      fpr(stream, "%s ", opcode->name);
      info->print_address_func(memaddr + (offset << 2) + 4, info);
      break;
    }
    if (opcode->args == 2) {
      if (offset == 0)
	fpr(stream, "%s (%s)", opcode->name,
	    print_reg_name((iword >> 20) & 0xf));
      else
	fpr(stream, "%s %d(%s)", opcode->name, offset,
	    print_reg_name((iword >> 20) & 0xf));
      break;
    }
    __attribute__ ((fallthrough));
  case BEXKAT1_POP:
  case BEXKAT1_JUMP:
    if (opcode->args == 0) {
      fpr(stream, "%s", opcode->name);
    }
    if (opcode->args == 1) {
      fpr(stream, "%s ", opcode->name);
      if (opcode->size)
        info->print_address_func((bfd_vma) imm32, info);
      else
	fpr(stream, "%s", print_reg_name((iword >> 20) & 0xf));
    }
    if (opcode->args == 2) {
      if (offset == 0)
	fpr(stream, "%s (%s)", opcode->name,
	    print_reg_name((iword >> 16) & 0xf));
      else
	fpr(stream, "%s %d(%s)", opcode->name, offset,
	    print_reg_name((iword >> 16) & 0xf));
    }
    break;
  case BEXKAT1_MOV:
    if (opcode->opcode == 0) {
      fpr(stream, "%s %s", opcode->name, print_reg_name((iword >> 16) & 0xf));
      break;
    }
    if (opcode->opcode == 4) {
      fpr(stream, "%s %s", opcode->name, print_reg_name((iword >> 20) & 0xf));
      break;
    }
    __attribute__ ((fallthrough)); 
  case BEXKAT1_CMP:
  case BEXKAT1_FPU:
  case BEXKAT1_INTU:
  case BEXKAT1_FP:
  case BEXKAT1_ALU:
  case BEXKAT1_INT:
  case BEXKAT1_BRANCH:
    if (opcode->args == 1)
      fpr(stream, "%s %d", opcode->name, offset);
    if (opcode->args == 2)
      fpr(stream, "%s %s, %s", opcode->name,
	  print_reg_name((iword >> 20) & 0xf),
	  print_reg_name((iword >> 16) & 0xf));
    if (opcode->args == 3) {
      if (opcode->opcode < 8)
	fpr(stream, "%s %s, %s, %s", opcode->name,
	    print_reg_name((iword >> 20) & 0xf),
	    print_reg_name((iword >> 16) & 0xf),
	    print_reg_name((iword >> 12) & 0xf));
      else
	fpr(stream, "%s %s, %s, %d", opcode->name,
	    print_reg_name((iword >> 20) & 0xf),
	    print_reg_name((iword >> 16) & 0xf),
	    offset);
    }
    break;
  case BEXKAT1_STORE:
  case BEXKAT1_LOAD:
    if (opcode->args == 2) {
      fpr(stream, "%s %s, ", opcode->name,
	  print_reg_name((iword >> 20) & 0xf));
      info->print_address_func((bfd_vma) imm32, info);
    }
    if (opcode->args == 3) {
      if (offset == 0)
	fpr(stream, "%s %s, (%s)", opcode->name,
	    print_reg_name((iword >> 20) & 0xf),
	    print_reg_name((iword >> 16) & 0xf));
      else
	fpr(stream, "%s %s, %d(%s)", opcode->name,
	    print_reg_name((iword >> 20) & 0xf),
	    offset,
	    print_reg_name((iword >> 16) & 0xf));
    }
    break;
  case BEXKAT1_LDI:
    if (opcode->args == 2) {
      fpr(stream, "%s %s, ", opcode->name,
	  print_reg_name((iword >> 20) & 0xf));
      if (opcode->size)
	fpr(stream, "%d", imm32);
      else
	fpr(stream, "%d", offset & 0x7fff);
    }
    break;
  }

  return length;

fail:
  info->memory_error_func(status, memaddr, info);
  return -1;
}
