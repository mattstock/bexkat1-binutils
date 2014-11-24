/* tc-bexkat1.c -- Assembler code for the Bexkat 1.
   Copyright (C) 1999-2014 Free Software Foundation, Inc.
   Written by Matt Stock (stock@csgeeks.org)

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 51 Franklin Street - Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include "as.h"
#include "safe-ctype.h"
#include "subsegs.h"
#include "opcode/bexkat1.h"

extern const bexkat1_opc_info_t bexkat1_opc_info[128];

const char comment_chars[] = ";!";
const char line_comment_chars[] = "#*";
const char line_separator_chars[] = "";

const char EXP_CHARS[] = "eE";
const char FLT_CHARS[] = "rRsSfFdDxXpP";

static int pending_reloc;
static struct hash_control *opcode_hash_control;

const pseudo_typeS md_pseudo_table[] =
{
  { 0, 0, 0}
};

void
md_operand (expressionS *op __attribute__((unused)))
{
}

/* Initialize the assembler.  Create the opcode hash table
   (sorted on the names) with the opcode table
   (from opcode library).  */
void
md_begin (void)
{
  const bexkat1_opc_info_t *opcode;
  opcode_hash_control = hash_new();

  /* Insert names into the hash table */
  for (opcode = bexkat1_opc_info; opcode->name; opcode++)
    hash_insert(opcode_hash_control, opcode->name, (char *)opcode);

  bfd_set_arch_mach(stdoutput, TARGET_ARCH, 0);
}

/* Convert the instructions into frags and bytes */
void
md_assemble(char *str)
{
  char *op_start;
  char *op_end;

  bexkat1_opc_info_t *opcode;
  char *output;
  int idx = 0;
  char pend;

  int nlen = 0;

  while (*str == ' ')
    str++;

  op_start = str;
  for (op_end = str;
       *op_end && !is_end_of_line[*op_end & 0xff] && *op_end != ' ';
       op_end++)
    nlen++;

  pend = *op_end;
  *op_end = 0;

  if (nlen == 0)
    as_bad(_("can't find opcode "));

  opcode = (bexkat1_opc_info_t *) hash_find(opcode_hash_control, op_start);
  *op_end = pend;

  if (opcode == NULL) {
    as_bad(_("unknown opcode %s"), op_start);
    return;
  }

  output = frag_more(1);
  output[idx++] = opcode->opcode;

  while (ISSPACE(*op_end))
    op_end++;

  if (*op_end != 0)
    as_warn("extra stuff on line ignored");

  if (pending_reloc)
    as_bad("Something forgot to clean up\n");
}

/* Turn a string in input_line_pointer into a floating point constant of type
   type. */

char *
md_atof(int type, char *litP, int *sizeP)
{
  int prec;
  LITTLENUM_TYPE words[4];
  char *t;
  int i;

  switch (type) {
  case 'f':
    prec = 2;
    break;
  case 'd':
    prec = 4;
  default:
    *sizeP = 0;
    return _("bad call to md_atof");
  }

  t = atof_ieee(input_line_pointer, type, words);
  if (t)
    input_line_pointer = t;

  *sizeP = prec * 2;

  for (i = prec-1; i >= 0; i--) {
    md_number_to_chars(litP, (valueT) words[i], 2);
    litP += 2;
  }
  return NULL;
}

const char *md_shortopts = "";

struct option md_longopts[] =
{
  { NULL, no_argument, NULL, 0}
};

size_t md_longopts_size = sizeof(md_longopts);

int
md_parse_option(int c ATTRIBUTE_UNUSED, char *arg ATTRIBUTE_UNUSED)
{
  return 0;
}

void
md_show_usage(FILE *stream ATTRIBUTE_UNUSED)
{
}

void
md_apply_fix(fixS *fixP ATTRIBUTE_UNUSED, valueT *valP ATTRIBUTE_UNUSED, segT seg ATTRIBUTE_UNUSED)
{
}

void
md_number_to_chars(char *ptr,valueT use, int nbytes)
{
  number_to_chars_bigendian(ptr, use, nbytes);
}

arelent *
tc_gen_reloc(asection *section ATTRIBUTE_UNUSED, fixS *fixp)
{
  arelent *rel;
  bfd_reloc_code_real_type r_type;

  rel = xmalloc(sizeof(arelent));
  rel->sym_ptr_ptr = xmalloc(sizeof(asymbol *));
  *rel->sym_ptr_ptr = symbol_get_bfdsym(fixp->fx_addsy);
  rel->address = fixp->fx_frag->fr_address + fixp->fx_where;

  r_type = fixp->fx_r_type;
  rel->addend = fixp->fx_addnumber;
  rel->howto = bfd_reloc_type_lookup(stdoutput, r_type);

  if (rel->howto == NULL) {
    as_bad_where(fixp->fx_file, fixp->fx_line,
                 _("Cannot represent relocation type %s"),
                 bfd_get_reloc_code_name(r_type));
    rel->howto = bfd_reloc_type_lookup(stdoutput, BFD_RELOC_32);
    gas_assert(rel->howto != NULL);
  }

  return rel;
}
