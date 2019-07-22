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
#include "opcode/bexkat1.h"

#define JMP_INSN ((BEXKAT1_JUMP << 28) | 0x1)
#define MAX_STUB_SIZE 8

#define ELF_ARCH		bfd_arch_bexkat1
#define ELF_MACHINE_CODE	EM_BEXKAT1
#define ELF_MAXPAGESIZE		0x1000

#define TARGET_BIG_SYM          bexkat1_elf32_be_vec
#define TARGET_BIG_NAME		"elf32-bexkat1-be"
#define TARGET_LITTLE_SYM       bexkat1_elf32_le_vec
#define TARGET_LITTLE_NAME	"elf32-bexkat1-le"

static bfd_reloc_status_type
bexkat1_elf_reloc (bfd *, arelent *, asymbol *, void *,
			asection *, bfd *, char **);
static bfd_boolean
bexkat1_elf_new_section_hook (bfd *abfd, asection *sec);
static bfd_boolean
bexkat1_elf_relax_section (bfd *abfd,
			   asection *sec,
			   struct bfd_link_info *link_info,
			   bfd_boolean *again);

static reloc_howto_type bexkat1_elf_howto_table[] =
  {
    /* No relocation */
    HOWTO(R_BEXKAT1_NONE,         /* type */
	  0,                      /* rightshift */
	  2,                      /* size */
	  32,                     /* bitsize */
	  FALSE,                  /* pc_relative */
	  0,                      /* bitpos */
	  complain_overflow_dont, /* complain_on_overflow */
	  bexkat1_elf_reloc,      /* special_function */
	  "R_BEXKAT1_NONE",       /* name */
	  FALSE,                  /* partial_inplace */
	  0,                      /* src_mask */
	  0,                      /* dst_mask */
	  FALSE),                 /* pcrel_offset */
    HOWTO(R_BEXKAT1_15,           /* type */
	  0,                      /* rightshift */
	  2,                      /* size */
	  15,                     /* bitsize */
	  FALSE,                  /* pc_relative */
	  1,                      /* bitpos */
	  complain_overflow_bitfield, /* complain_on_overflow */
	  bexkat1_elf_reloc,      /* special_function */
	  "R_BEXKAT1_15",         /* name */
	  FALSE,                  /* partial_inplace */
	  0x0000fffe,             /* src_mask */
	  0x0000fffe,             /* dst_mask */
	  FALSE),                 /* pcrel_offset */
    HOWTO(R_BEXKAT1_PCREL_15,     /* type */
	  2,                      /* rightshift */
	  2,                      /* size */
	  15,                     /* bitsize */
	  TRUE,                   /* pc_relative */
	  1,                      /* bitpos */
	  complain_overflow_signed, /* complain_on_overflow */
	  bexkat1_elf_reloc,      /* special_function */
	  "R_BEXKAT1_PCREL_15",   /* name */
	  FALSE,                  /* partial_inplace */
	  0x0000fffe,             /* src_mask */
	  0x0000fffe,             /* dst_mask */
	  TRUE),                  /* pcrel_offset */
    HOWTO(R_BEXKAT1_32,           /* type */
	  0,                      /* rightshift */
	  2,                      /* size */
	  32,                     /* bitsize */
	  FALSE,                  /* pc_relative */
	  0,                      /* bitops */
	  complain_overflow_bitfield, /* complain_on_overflow */
	  bexkat1_elf_reloc,      /* special_function */
	  "R_BEXKAT1_32",         /* name */
	  FALSE,                  /* partial_inplace */
	  0,                      /* src_mask */
	  0xffffffff,             /* dst_mask */
	  FALSE)                  /* pcrel_offset */
  };

struct elf_bexkat1_reloc_map
{
  bfd_reloc_code_real_type  bfd_reloc_val;
  unsigned char             elf_reloc_val;
};

struct _bexkat1_elf_section_data
{
  struct bfd_elf_section_data elf;
  struct bpo_reloc_section_info *reloc;
  struct stub_info
  {
    bfd_size_type n_relocs;
    bfd_size_type stubs_size_sum;
    bfd_size_type *stub_size;
    bfd_size_type stub_offset;
  } stubs;
};

#define bexkat1_elf_section_data(sec) \
  ((struct _bexkat1_elf_section_data *) elf_section_data (sec))

static const struct elf_bexkat1_reloc_map bexkat1_reloc_map[] =
  {
    { BFD_RELOC_NONE,      R_BEXKAT1_NONE },
    { BFD_RELOC_BEXKAT1_15, R_BEXKAT1_15 },
    { BFD_RELOC_BEXKAT1_15_PCREL,  R_BEXKAT1_PCREL_15 },
    { BFD_RELOC_32,        R_BEXKAT1_32 }
  };

static bfd_boolean
bexkat1_elf_new_section_hook (bfd *abfd, asection *sec)
{
  if (!sec->used_by_bfd)
    {
      struct _bexkat1_elf_section_data *sdata;
      bfd_size_type amt = sizeof (*sdata);

      sdata = bfd_zalloc (abfd, amt);
      if (sdata == NULL)
	return FALSE;
      sec->used_by_bfd = sdata;
    }

  return _bfd_elf_new_section_hook (abfd, sec);
}

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

static bfd_reloc_status_type
bexkat1_elf_perform_relocation (asection *isec,
				reloc_howto_type *howto,
				void *datap,
				bfd_vma addr ATTRIBUTE_UNUSED,
				bfd_vma value,
				char **error_message ATTRIBUTE_UNUSED)
{
  bfd *abfd = isec->owner;
  bfd_reloc_status_type r;

  switch (howto->type)
    {
    case R_BEXKAT1_PCREL_15:
      {
	if ((r = bfd_check_overflow(complain_overflow_signed,
				    howto->bitsize,
				    howto->rightshift,
				    bfd_arch_bits_per_address (abfd),
				    value)) == bfd_reloc_ok)
	  {
	    bfd_vma in1
	      = bfd_get_32 (abfd, (bfd_byte *) datap);
	    
	    value >>= howto->rightshift;
	    value &= 0x7fff;
	    value <<= 1;
	    bfd_put_32 (abfd,
			(in1 & howto->src_mask) | (value & howto->dst_mask),
			(bfd_byte *) datap);
	    
	    return bfd_reloc_ok;
	  }

	/*
	bfd_size_type size = isec->rawsize ? isec->rawsize : isec->size;
	bfd_byte *stubcontents
	  = ((bfd_byte *) datap
	     - (addr - (isec->output_section->vma + isec->output_offset))
	     + size
	     + bexkat1_elf_section_data (isec)->stubs.stub_offset);
	bfd_vma stubaddr;

	if (bexkat1_elf_section_data (isec)->stubs.n_relocs == 0)
	  {
	    fprintf(stderr, "something impossible happening\n");
	    return bfd_reloc_dangerous;
	  }
	  
	stubaddr = isec->output_section->vma
	  + isec->output_offset
	  + size
	  + (bexkat1_elf_section_data (isec)
	     ->stubs.stub_offset)
	  - addr;

	fprintf(stderr, "  addr = %08lx, stubaddr = %08lx\n",
		addr, stubaddr);
  
	r = bexkat1_elf_perform_relocation (isec,
				      &bexkat1_elf_howto_table
				      [R_BEXKAT1_PCREL_15],
				      datap,
				      addr,
				      isec->output_section->vma
				      + isec->output_offset
				      + size
				      + (bexkat1_elf_section_data (isec)
					 ->stubs.stub_offset)
				      - addr,
				      error_message);

	if (r != bfd_reloc_ok)
	  return r;

	stubaddr
	  = (isec->output_section->vma
	     + isec->output_offset
	     + size
	     + bexkat1_elf_section_data (isec)->stubs.stub_offset);
	
	bfd_put_32 (abfd, JMP_INSN, stubcontents);
	r = bexkat1_elf_perform_relocation (isec,
					    &bexkat1_elf_howto_table
					    [R_BEXKAT1_32],
					    stubcontents,
					    stubaddr,
					    value + addr - stubaddr,
					    error_message);
	bexkat1_elf_section_data (isec)->stubs.stub_offset += 4;
	
	if (size + bexkat1_elf_section_data (isec)->stubs.stub_offset
	    > isec->size)
	  abort ();
	*/
	return r;
      }
    case R_BEXKAT1_32:
      bfd_put_32 (abfd, 0, (bfd_byte *) datap);
      return bfd_reloc_ok;
    }

  bfd_put_32 (abfd, 0x12341111, (bfd_byte *) datap);
  return bfd_reloc_ok;
}

static bfd_reloc_status_type
bexkat1_final_link_relocate (reloc_howto_type *howto,
			     asection *input_section,
			     bfd_byte *contents,
			     bfd_vma r_offset,
			     bfd_signed_vma r_addend,
			     bfd_vma relocation,
			     const char *symname ATTRIBUTE_UNUSED,
			     asection *symsec ATTRIBUTE_UNUSED,
			     char **error_message)
{
  bfd_reloc_status_type r = bfd_reloc_ok;
  bfd_vma addr
    = (input_section->output_section->vma
       + input_section->output_offset
       + r_offset);
  bfd_signed_vma srel
    = (bfd_signed_vma) relocation + r_addend;

  switch (howto->type)
    {
    case R_BEXKAT1_PCREL_15:
      contents += r_offset;
      
      srel -= (input_section->output_section->vma
	       + input_section->output_offset
	       + r_offset);
      
      r = bexkat1_elf_perform_relocation (input_section, howto, contents,
					  addr, srel, error_message);
      break;
    default:
      r = _bfd_final_link_relocate (howto, input_section->owner, input_section,
				    contents, r_offset,
				    relocation, r_addend);
    }
  return r;
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

bfd_reloc_status_type
bexkat1_elf_reloc (bfd *abfd ATTRIBUTE_UNUSED,
	       arelent *reloc_entry,
	       asymbol *symbol,
	       void *data ATTRIBUTE_UNUSED,
	       asection *input_section ATTRIBUTE_UNUSED,
	       bfd *output_bfd,
	       char **error_message ATTRIBUTE_UNUSED)
{
  bfd_vma relocation;
  bfd_reloc_status_type r;
  asection *reloc_target_output_section;
  bfd_reloc_status_type flag = bfd_reloc_ok;
  bfd_vma output_base = 0;

  r = bfd_elf_generic_reloc (abfd, reloc_entry, symbol, data,
                             input_section, output_bfd, error_message);

  if (r != bfd_reloc_continue)
    return r;

  if (bfd_is_und_section (symbol->section)
      && (symbol->flags & BSF_WEAK) == 0
      && output_bfd == (bfd *) NULL)
    return bfd_reloc_undefined;

  /* Is the address of the relocation really within the section?  */
  if (reloc_entry->address > bfd_get_section_limit (abfd, input_section))
    return bfd_reloc_outofrange;

  /* Work out which section the relocation is targeted at and the
     initial relocation command value.  */

  /* Get symbol value.  (Common symbols are special.)  */
  if (bfd_is_com_section (symbol->section))
    relocation = 0;
  else
    relocation = symbol->value;

  reloc_target_output_section = bfd_get_output_section (symbol);

  /* Here the variable relocation holds the final address of the symbol we
     are relocating against, plus any addend.  */
  if (output_bfd)
    output_base = 0;
  else
    output_base = reloc_target_output_section->vma;

  relocation += output_base + symbol->section->output_offset;

  if (output_bfd != (bfd *) NULL)
    {
      /* Add in supplied addend.  */
      relocation += reloc_entry->addend;

      /* This is a partial relocation, and we want to apply the
         relocation to the reloc entry rather than the raw data.
         Modify the reloc inplace to reflect what we now know.  */
      reloc_entry->addend = relocation;
      reloc_entry->address += input_section->output_offset;

      return flag;
    }

  return bexkat1_final_link_relocate (reloc_entry->howto, input_section,
                                   data, reloc_entry->address,
                                   reloc_entry->addend, relocation,
                                   bfd_asymbol_name (symbol),
                                   reloc_target_output_section,
                                   error_message);
}

static bfd_boolean
bexkat1_elf_info_to_howto_rela(bfd *abfd,
			       arelent *cache_ptr,
			       Elf_Internal_Rela *dst)
{
  unsigned int r;

  r = ELF32_R_TYPE(dst->r_info);

  if (r >= (unsigned int) R_BEXKAT1_max)
    {
      _bfd_error_handler (_("%pB: unsupported relocation type %#x"),
			  abfd, r);
      bfd_set_error (bfd_error_bad_value);
      return FALSE;
    }

  cache_ptr->howto = &bexkat1_elf_howto_table[r];
  return TRUE;
}

static bfd_boolean
bexkat1_elf_relocate_section (bfd *output_bfd,
			      struct bfd_link_info *info,
			      bfd *input_bfd,
			      asection *input_section,
			      bfd_byte *contents,
			      Elf_Internal_Rela *relocs,
			      Elf_Internal_Sym *local_syms,
			      asection **local_sections)
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  Elf_Internal_Rela *rel;
  Elf_Internal_Rela *relend;

  symtab_hdr = & elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (input_bfd);
  relend     = relocs + input_section->reloc_count;

  for (rel = relocs; rel < relend; rel ++)
    {
      reloc_howto_type *howto;
      unsigned long r_symndx;
      Elf_Internal_Sym *sym;
      asection *sec;
      struct elf_link_hash_entry *h;
      bfd_vma relocation;
      bfd_reloc_status_type r;
      const char *name;
      int r_type;

      r_type = ELF32_R_TYPE (rel->r_info);
      r_symndx = ELF32_R_SYM (rel->r_info);
      howto  = bexkat1_elf_howto_table + r_type;
      h      = NULL;
      sym    = NULL;
      sec    = NULL;

      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sec = local_sections [r_symndx];
	  relocation = _bfd_elf_rela_local_sym (output_bfd, sym, &sec, rel);

	  name = bfd_elf_string_from_elf_section
	    (input_bfd, symtab_hdr->sh_link, sym->st_name);
	  name = (name == NULL) ? bfd_section_name (input_bfd, sec) : name;
	}
      else
	{
	  bfd_boolean unresolved_reloc, warned, ignored;

	  RELOC_FOR_GLOBAL_SYMBOL (info, input_bfd, input_section, rel,
				   r_symndx, symtab_hdr, sym_hashes,
				   h, sec, relocation,
				   unresolved_reloc, warned, ignored);

	  name = h->root.root.string;
	}

      if (sec != NULL && discarded_section (sec))
	RELOC_AGAINST_DISCARDED_SECTION (info, input_bfd, input_section,
					 rel, 1, relend, howto, 0, contents);

      if (bfd_link_relocatable (info))
	{
	  if (sym != NULL && ELF_ST_TYPE (sym->st_info) == STT_SECTION)
	    rel->r_addend += sec->output_offset;

	  if (r_type == R_BEXKAT1_PCREL_15)
	    {
	      fprintf(stderr, "bexkat1 relocate section wtf...\n");
	      abort();
	      // TODO: something
	    } 
	}

      r = bexkat1_final_link_relocate (howto, input_section,
				       contents, rel->r_offset,
				       rel->r_addend, relocation, name,
				       sec, NULL);

      if (r != bfd_reloc_ok)
	{
	  const char * msg = (const char *)NULL;

	  switch (r)
	    {
	    case bfd_reloc_overflow:
	      (*info->callbacks->reloc_overflow)
		(info, (h ? &h->root : NULL), name, howto->name,
		 (bfd_vma) 0, input_bfd, input_section, rel->r_offset);
	      break;

	    case bfd_reloc_undefined:
	      (*info->callbacks->undefined_symbol)
		(info, name, input_bfd, input_section, rel->r_offset,
		 TRUE);
	      break;

	    case bfd_reloc_outofrange:
	      msg = _("internal error: out of range error");
	      break;

	    case bfd_reloc_notsupported:
	      msg = _("internal error: unsupported relocation error");
	      break;

	    case bfd_reloc_dangerous:
	      msg = _("internal error: dangerous relocation");
	      break;

	    default:
	      msg = _("internal error: unknown error");
	      break;
	    }

	  if (msg)
	    (*info->callbacks->warning)
	      (info, msg, name, input_bfd, input_section, rel->r_offset);
	}
    }

  return TRUE;
}

static bfd_boolean
bexkat1_elf_check_relocs (bfd *abfd,
			  struct bfd_link_info *info,
			  asection *sec,
			  const Elf_Internal_Rela *relocs)
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  const Elf_Internal_Rela *rel;
  const Elf_Internal_Rela *rel_end;

  if (bfd_link_relocatable (info))
    return TRUE;

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (abfd);
  
  rel_end = relocs + sec->reloc_count;
  for (rel = relocs; rel < rel_end; rel++)
    {
      struct elf_link_hash_entry *h;
      unsigned long r_symndx;
      
      r_symndx = ELF32_R_SYM (rel->r_info);
      if (r_symndx < symtab_hdr->sh_info)
        h = NULL;
      else
	{
	  h = sym_hashes[r_symndx - symtab_hdr->sh_info];
	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;

	  h->root.non_ir_ref_regular = 1;
	}

      switch (ELF32_R_TYPE (rel->r_info))
	{
	case R_BEXKAT1_PCREL_15:
	  bexkat1_elf_section_data (sec)->stubs.n_relocs++;
	  break;
	}
    }

  if (bexkat1_elf_section_data (sec)->stubs.n_relocs != 0)
    {
      size_t i;
      bexkat1_elf_section_data (sec)->stubs.stub_size
	= bfd_alloc (abfd, bexkat1_elf_section_data (sec)->stubs.n_relocs
		     * sizeof (bexkat1_elf_section_data (sec)
			       ->stubs.stub_size[0]));
      if (bexkat1_elf_section_data (sec)->stubs.stub_size == NULL)
	return FALSE;

      for (i = 0; i < bexkat1_elf_section_data (sec)->stubs.n_relocs; i++)
	bexkat1_elf_section_data (sec)->stubs.stub_size[i] = MAX_STUB_SIZE;

    }
  
  return TRUE;
}

static bfd_boolean
bexkat1_elf_relax_section (bfd *abfd ATTRIBUTE_UNUSED,
			   asection *sec ATTRIBUTE_UNUSED,
			   struct bfd_link_info *link_info ATTRIBUTE_UNUSED,
			   bfd_boolean *again ATTRIBUTE_UNUSED)
{
  fprintf(stderr, "bexkat1_elf_relax_section()\n");
  return TRUE;
}

#define elf_backend_may_use_rel_p       0
#define elf_backend_may_use_rela_p      1
#define elf_backend_default_use_rela_p  1

#define elf_info_to_howto_rel           NULL
#define elf_info_to_howto	        bexkat1_elf_info_to_howto_rela
#define elf_backend_relocate_section    bexkat1_elf_relocate_section
#define elf_backend_check_relocs        bexkat1_elf_check_relocs

#define bfd_elf32_new_section_hook      bexkat1_elf_new_section_hook
#define bfd_elf32_bfd_relax_section     bexkat1_elf_relax_section
#define bfd_elf32_bfd_reloc_type_lookup bexkat1_elf_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup bexkat1_elf_reloc_name_lookup
#include "elf32-target.h"
