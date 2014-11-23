/* Bexkat1-specific support for 32-bit ELF
   Copyright (C) 1999-2014 Free Software Foundation, Inc.
   Contributed by Stephane Carrez (stcarrez@nerim.fr)
   (Heavily copied from the D10V port by Martin Hunt (hunt@cygnus.com))

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

#define ELF_ARCH		bfd_arch_bexkat1
#define ELF_MACHINE_CODE	EM_BEXKAT1
#define ELF_MAXPAGESIZE		0x1000

#define TARGET_BIG_SYM          bexkat1_elf32_vec
#define TARGET_BIG_NAME		"elf32-bexkat1"

#define bfd_elf32_bfd_reloc_type_lookup bfd_default_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup _bfd_norelocs_bfd_reloc_name_lookup
#define elf_info_to_howto	_bfd_elf_no_info_to_howto

#include "elf32-target.h"
