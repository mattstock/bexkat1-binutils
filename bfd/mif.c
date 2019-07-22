/* BFD back-end for Quartus Memory Initialization File objects.
   Copyright (C) 1995-2015 Free Software Foundation, Inc.
   Written by Matt Stock

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
#include "libbfd.h"
#include "libiberty.h"
#include "safe-ctype.h"

/* Macros for converting between hex and binary.  */

#define NIBBLE(x)    (hex_value (x))
#define HEX2(buffer) ((NIBBLE ((buffer)[0]) << 4) + NIBBLE ((buffer)[1]))
#define HEX4(buffer) ((HEX2 (buffer) << 8) + HEX2 ((buffer) + 2))
#define ISHEX(x)     (hex_p (x))

/* When we write out a MIF value, the values can not be output as
   they are seen.  Instead, we hold them in memory in this structure.  */

struct mif_data_list
{
  struct mif_data_list *next;
  bfd_byte *data;
  bfd_vma where;
  bfd_size_type size;
};

/* The mif tdata information.  */

struct mif_data_struct
{
  struct mif_data_list *head;
  struct mif_data_list *tail;
};

/* Create a mif object.  */

static bfd_boolean
mif_mkobject (bfd *abfd)
{
  struct mif_data_struct *tdata;

  tdata = (struct mif_data_struct *) bfd_alloc (abfd, sizeof (* tdata));
  if (tdata == NULL)
    return FALSE;

  abfd->tdata.mif_data = tdata;
  tdata->head = NULL;
  tdata->tail = NULL;
  return TRUE;
}

/* Try to recognize a MIF file.  */

static const bfd_target *
mif_object_p (bfd *abfd ATTRIBUTE_UNUSED)
{
  return NULL;
}

/* Write a record out to a MIF file.  */

static bfd_boolean
mif_write_record (bfd *abfd,
		  unsigned int count,
		  unsigned int addr,
		  bfd_byte *data)
{
  static const char digs[] = "0123456789ABCDEF";
  char buf[100];
  unsigned int i,j;

  snprintf(buf, 100, "%x : ", addr >> 2);

  for (i=0,j=strlen(buf); i < count; i++, data++) {
    buf[j++] = digs[(*data >> 4) & 0xf];
    buf[j++] = digs[*data & 0xf];
  }
  buf[j++] = ';';
  buf[j++] = '\n';
  buf[j] = '\0';

  if (bfd_bwrite (buf, (bfd_size_type) strlen(buf), abfd) != strlen(buf))
    return FALSE;

  return TRUE;
}

/* Write out a MIF file.  */

static bfd_boolean
mif_write_object_contents (bfd *abfd)
{
  struct mif_data_list *l;
  char begin[] = "DEPTH = 32768;\nWIDTH = 32;\nADDRESS_RADIX = HEX;\nDATA_RADIX = HEX;\nCONTENT\nBEGIN\n";
  char end[] = "END;\n";

  // This is hacked to solve a specific problem.  It can be adapted to
  // support additional MIF depths, but right now I just need 32k x 32.
  if (bfd_bwrite (begin, (bfd_size_type) strlen(begin), abfd) != strlen(begin))
    return FALSE;

  for (l = abfd->tdata.mif_data->head; l != NULL; l = l->next)
    {
      bfd_vma where;
      bfd_byte *p;
      bfd_size_type count;

      where = l->where;
      p = l->data;
      count = l->size;

      while (count > 0)
	{
	  size_t now;

	  now = count;
	  if (count > 4)
	    now = 4;

	  if (! mif_write_record (abfd, now, where - abfd->start_address, p))
	    return FALSE;

	  where += now;
	  p += now;
	  count -= now;
	}
    }

if (bfd_bwrite (end, (bfd_size_type) strlen(end), abfd) != strlen(end))
    return FALSE;
  
  return TRUE;
}

/* Set the architecture for the output file.  The architecture is
   irrelevant, so we ignore errors about unknown architectures.  */

static bfd_boolean
mif_set_arch_mach (bfd *abfd,
		    enum bfd_architecture arch,
		    unsigned long mach)
{
  if (! bfd_default_set_arch_mach (abfd, arch, mach))
    {
      if (arch != bfd_arch_unknown)
	return FALSE;
    }
  return TRUE;
}

/* Get the size of the headers, for the linker.  */

static int
mif_sizeof_headers (bfd *abfd ATTRIBUTE_UNUSED,
		     struct bfd_link_info *info ATTRIBUTE_UNUSED)
{
  return 0;
}

static bfd_boolean
mif_set_section_contents (bfd *abfd,
                           asection *section,
                           const void * location,
                           file_ptr offset,
                           bfd_size_type count)
{
  struct mif_data_list *n;
  bfd_byte *data;
  struct mif_data_struct *tdata;

  if (count == 0
      || (section->flags & SEC_ALLOC) == 0
      || (section->flags & SEC_LOAD) == 0)
    return TRUE;

  n = (struct mif_data_list *) bfd_alloc (abfd, sizeof (* n));
  if (n == NULL)
    return FALSE;

  data = (bfd_byte *) bfd_alloc (abfd, count);
  if (data == NULL)
    return FALSE;
  memcpy (data, location, (size_t) count);

  n->data = data;
  n->where = section->lma + offset;
  n->size = count;

  /* Sort the records by address.  Optimize for the common case of
     adding a record to the end of the list.  */
  tdata = abfd->tdata.mif_data;
  if (tdata->tail != NULL
      && n->where >= tdata->tail->where)
    {
      tdata->tail->next = n;
      n->next = NULL;
      tdata->tail = n;
    }
  else
    {
      struct mif_data_list **pp;

      for (pp = &tdata->head;
	   *pp != NULL && (*pp)->where < n->where;
	   pp = &(*pp)->next)
	;
      n->next = *pp;
      *pp = n;
      if (n->next == NULL)
	tdata->tail = n;
    }

  return TRUE;
}

static bfd_boolean
mif_get_section_contents (bfd *abfd ATTRIBUTE_UNUSED,
                           asection *section ATTRIBUTE_UNUSED,
                           void * location ATTRIBUTE_UNUSED,
                           file_ptr offset ATTRIBUTE_UNUSED,
			  bfd_size_type count ATTRIBUTE_UNUSED)
{
  return FALSE;
}

/* Some random definitions for the target vector.  */

#define	mif_close_and_cleanup                    _bfd_generic_close_and_cleanup
#define mif_bfd_free_cached_info                 _bfd_generic_bfd_free_cached_info
#define mif_new_section_hook                     _bfd_generic_new_section_hook
#define mif_get_section_contents_in_window       _bfd_generic_get_section_contents_in_window
#define mif_get_symtab_upper_bound               _bfd_long_bfd_0
#define mif_canonicalize_symtab                  _bfd_nosymbols_canonicalize_symtab
#define mif_make_empty_symbol                    _bfd_generic_make_empty_symbol
#define mif_print_symbol                         _bfd_nosymbols_print_symbol
#define mif_get_symbol_info                      _bfd_nosymbols_get_symbol_info
#define mif_get_symbol_version_string		  _bfd_nosymbols_get_symbol_version_string
#define mif_bfd_is_target_special_symbol         _bfd_bool_bfd_asymbol_false
#define mif_bfd_is_local_label_name              _bfd_nosymbols_bfd_is_local_label_name
#define mif_get_lineno                           _bfd_nosymbols_get_lineno
#define mif_find_nearest_line                    _bfd_nosymbols_find_nearest_line
#define mif_find_line                            _bfd_nosymbols_find_line
#define mif_find_inliner_info                    _bfd_nosymbols_find_inliner_info
#define mif_bfd_make_debug_symbol                _bfd_nosymbols_bfd_make_debug_symbol
#define mif_read_minisymbols                     _bfd_nosymbols_read_minisymbols
#define mif_minisymbol_to_symbol                 _bfd_nosymbols_minisymbol_to_symbol
#define mif_bfd_get_relocated_section_contents   bfd_generic_get_relocated_section_contents
#define mif_bfd_relax_section                    bfd_generic_relax_section
#define mif_bfd_gc_sections                      bfd_generic_gc_sections
#define mif_bfd_lookup_section_flags             bfd_generic_lookup_section_flags
#define mif_bfd_merge_sections                   bfd_generic_merge_sections
#define mif_bfd_is_group_section                 bfd_generic_is_group_section
#define mif_bfd_discard_group                    bfd_generic_discard_group
#define mif_section_already_linked               _bfd_generic_section_already_linked
#define mif_bfd_define_common_symbol             bfd_generic_define_common_symbol
#define mif_bfd_link_hide_symbol                 _bfd_generic_link_hide_symbol
#define mif_bfd_define_start_stop                bfd_generic_define_start_stop
#define mif_bfd_link_hash_table_create           _bfd_generic_link_hash_table_create
#define mif_bfd_link_add_symbols                 _bfd_generic_link_add_symbols
#define mif_bfd_link_just_syms                   _bfd_generic_link_just_syms
#define mif_bfd_copy_link_hash_symbol_type \
  _bfd_generic_copy_link_hash_symbol_type
#define mif_bfd_final_link                       _bfd_generic_final_link
#define mif_bfd_link_split_section               _bfd_generic_link_split_section
#define mif_bfd_link_check_relocs                _bfd_generic_link_check_relocs


/* The Intel Hex target vector.  */

const bfd_target mif_vec =
{
  "mif",			/* Name.  */
  bfd_target_mif_flavour,
  BFD_ENDIAN_UNKNOWN,		/* Target byte order.  */
  BFD_ENDIAN_UNKNOWN,		/* Target headers byte order.  */
  0,				/* Object flags.  */
  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD),	/* Section flags.  */
  0,				/* Leading underscore.  */
  ' ',				/* AR_pad_char.  */
  16,				/* AR_max_namelen.  */
  0,				/* match priority.  */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
  bfd_getb32, bfd_getb_signed_32, bfd_putb32,
  bfd_getb16, bfd_getb_signed_16, bfd_putb16,	/* Data.  */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
  bfd_getb32, bfd_getb_signed_32, bfd_putb32,
  bfd_getb16, bfd_getb_signed_16, bfd_putb16,	/* Headers. */

  {
    _bfd_dummy_target,
    mif_object_p,		/* bfd_check_format.  */
    _bfd_dummy_target,
    _bfd_dummy_target,
  },
  {
    _bfd_bool_bfd_false_error,
    mif_mkobject,
    _bfd_generic_mkarchive,
    _bfd_bool_bfd_false_error,
  },
  {				/* bfd_write_contents.  */
    _bfd_bool_bfd_false_error,
    mif_write_object_contents,
    _bfd_write_archive_contents,
    _bfd_bool_bfd_false_error,
  },

  BFD_JUMP_TABLE_GENERIC (mif),
  BFD_JUMP_TABLE_COPY (_bfd_generic),
  BFD_JUMP_TABLE_CORE (_bfd_nocore),
  BFD_JUMP_TABLE_ARCHIVE (_bfd_noarchive),
  BFD_JUMP_TABLE_SYMBOLS (mif),
  BFD_JUMP_TABLE_RELOCS (_bfd_norelocs),
  BFD_JUMP_TABLE_WRITE (mif),
  BFD_JUMP_TABLE_LINK (mif),
  BFD_JUMP_TABLE_DYNAMIC (_bfd_nodynamic),

  NULL,

  NULL
};
