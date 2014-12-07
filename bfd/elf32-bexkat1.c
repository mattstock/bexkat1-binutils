/* Bexkat1-specific support for 32-bit ELF
   Copyright (C) 1999-2014 Free Software Foundation, Inc.
   Contributed by Matt Stock (stock@bexkat.com)

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "bfdlink.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/bexkat1.h"

#define ELF_ARCH		bfd_arch_bexkat1
#define ELF_MACHINE_CODE	EM_BEXKAT1
#define ELF_MAXPAGESIZE		0x1000

#define TARGET_BIG_SYM          bexkat1_elf32_vec
#define TARGET_BIG_NAME		"elf32-bexkat1"

static reloc_howto_type bexkat1_elf_howto_table[] =
  {
    /* No relocation */
    HOWTO(R_BEXKAT1_NONE,         /* type */
	  0,                      /* rightshift */
	  2,                      /* size */
	  32,                     /* bitsize */
	  FALSE,                  /* pc_relative */
	  0,                      /* bitops */
	  complain_overflow_dont, /* complain_on_overflow */
	  bfd_elf_generic_reloc,  /* special_function */
	  "R_BEXKAT1_NONE",       /* name */
	  FALSE,                  /* partial_inplace */
	  0,                      /* src_mask */
	  0,                      /* dst_mask */
	  FALSE),                 /* pcrel_offset */
    HOWTO(R_BEXKAT1_16,           /* type */
	  0,                      /* rightshift */
	  1,                      /* size */
	  16,                     /* bitsize */
	  FALSE,                  /* pc_relative */
	  0,                      /* bitops */
	  complain_overflow_bitfield, /* complain_on_overflow */
	  bfd_elf_generic_reloc,  /* special_function */
	  "R_BEXKAT1_DIR16",      /* name */
	  FALSE,                  /* partial_inplace */
	  0xffff,                 /* src_mask */
	  0xffff,                 /* dst_mask */
	  FALSE),                 /* pcrel_offset */
    HOWTO(R_BEXKAT1_PCREL_16,     /* type */
	  0,                      /* rightshift */
	  1,                      /* size */
	  16,                     /* bitsize */
	  FALSE,                  /* pc_relative */
	  0,                      /* bitops */
	  complain_overflow_bitfield, /* complain_on_overflow */
	  bfd_elf_generic_reloc,  /* special_function */
	  "R_BEXKAT1_PCREL_16",   /* name */
	  FALSE,                  /* partial_inplace */
	  0xffff,                 /* src_mask */
	  0xffff,                 /* dst_mask */
	  TRUE),                  /* pcrel_offset */
    HOWTO(R_BEXKAT1_32,           /* type */
	  0,                      /* rightshift */
	  2,                      /* size */
	  32,                     /* bitsize */
	  FALSE,                  /* pc_relative */
	  0,                      /* bitops */
	  complain_overflow_bitfield, /* complain_on_overflow */
	  bfd_elf_generic_reloc,  /* special_function */
	  "R_BEXKAT1_32",         /* name */
	  FALSE,                  /* partial_inplace */
	  0xffffffff,             /* src_mask */
	  0xffffffff,             /* dst_mask */
	  FALSE)                  /* pcrel_offset */
  };

struct elf_bexkat1_reloc_map
  {
    bfd_reloc_code_real_type  bfd_reloc_val;
    unsigned char             elf_reloc_val;
  };

static const struct elf_bexkat1_reloc_map bexkat1_reloc_map[] =
  {
    { BFD_RELOC_NONE,  R_BEXKAT1_NONE },
    { BFD_RELOC_16,    R_BEXKAT1_16 },
    { BFD_RELOC_16_PCREL,    R_BEXKAT1_PCREL_16 },
    { BFD_RELOC_32,    R_BEXKAT1_32 }
  };

static reloc_howto_type *
bexkat1_elf_reloc_type_lookup(bfd *abfd ATTRIBUTE_UNUSED,
			      bfd_reloc_code_real_type code)
{
  unsigned int i;

  for (i=0;
       i < sizeof(bexkat1_reloc_map) / sizeof(struct elf_bexkat1_reloc_map);
       i++)
    if (bexkat1_reloc_map[i].bfd_reloc_val == code)
      return &bexkat1_elf_howto_table[(int)bexkat1_reloc_map[i].elf_reloc_val];

  return NULL;
}

static reloc_howto_type *
bexkat1_elf_reloc_name_lookup(bfd *abfd ATTRIBUTE_UNUSED,
			      const char *r_name)
{
  unsigned int i;

  for (i=0;
       i < sizeof(bexkat1_elf_howto_table) / sizeof(bexkat1_elf_howto_table[0]);
       i++)
    if (bexkat1_elf_howto_table[i].name != NULL
	&& strcasecmp(bexkat1_elf_howto_table[i].name, r_name) == 0)
      return &bexkat1_elf_howto_table[i];

  return NULL;
}

static void
bexkat1_elf_info_to_howto(bfd *abfd ATTRIBUTE_UNUSED,
			  arelent *cache_ptr,
			  Elf_Internal_Rela *dst)
{
  unsigned int r;

  r = ELF32_R_TYPE(dst->r_info);

  BFD_ASSERT(r < (unsigned int) R_BEXKAT1_max);

  cache_ptr->howto = &bexkat1_elf_howto_table[r];
}

#define bfd_elf32_bfd_reloc_type_lookup bexkat1_elf_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup bexkat1_elf_reloc_name_lookup
#define elf_info_to_howto	bexkat1_elf_info_to_howto

#include "elf32-target.h"
