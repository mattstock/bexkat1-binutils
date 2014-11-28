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
  int count;
  const bexkat1_opc_info_t *opcode;
  opcode_hash_control = hash_new();

  /* Insert names into the hash table */
  for (count=0, opcode = bexkat1_form0_opc_info; count++ < 6; opcode++) {
    hash_insert(opcode_hash_control, opcode->name, (char *)opcode);
    fprintf(stderr, "adding opcode %s (%x)\n", opcode->name, opcode->opcode);
  }
  for (count=0, opcode = bexkat1_form1_opc_info; count++ < 10; opcode++) {
    hash_insert(opcode_hash_control, opcode->name, (char *)opcode);
    fprintf(stderr, "adding opcode %s (%x)\n", opcode->name, opcode->opcode);
  }
  for (count=0, opcode = bexkat1_form2_opc_info; count++ < 32; opcode++) {
    hash_insert(opcode_hash_control, opcode->name, (char *)opcode);
    fprintf(stderr, "adding opcode %s (%x)\n", opcode->name, opcode->opcode);
  }
  for (count=0, opcode = bexkat1_form3_opc_info; count++ < 9; opcode++) {
    hash_insert(opcode_hash_control, opcode->name, (char *)opcode);
    fprintf(stderr, "adding opcode %s (%x)\n", opcode->name, opcode->opcode);
  }

  bfd_set_arch_mach(stdoutput, TARGET_ARCH, 0);
}

static char *
parse_exp_save_ilp (char *s, expressionS *op)
{
  char *save = input_line_pointer;
  
  input_line_pointer = s;
  expression(op);
  s = input_line_pointer;
  input_line_pointer = save;
  return s;
}

static int
parse_regnum(char **ptr)
{
  int reg;
  char *s = *ptr;
  reg = s[1] - '0';
  if ((reg < 0) || (reg > 9)) {
    as_bad(_("illegal register number 1"));
    ignore_rest_of_line();
    return -1;
  }
  if ((reg > 0) || (reg < 4)) {
    int r2 = s[2] - '0';
    if ((r2 >= 0) && (r2 <= 9)) {
      reg = reg*10 + r2;
      if (reg > 31) {
	as_bad(_("illegal register number 2"));
	ignore_rest_of_line();
	return -1;
      }
      *ptr += 1;
    }
  } 
  
  *ptr += 2;
  return reg;
}

/* Convert the instructions into frags and bytes */
void
md_assemble(char *str)
{
  char *op_start;
  char *op_end;
  bexkat1_opc_info_t *opcode;
  char *p;
  char pend;
  unsigned short iword = 0;
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

  fprintf(stderr, "assemble string = %s\n", str);
  fprintf(stderr, "opcode found, opcode = %s (%x)\n", opcode->name, opcode->opcode);

  p = frag_more(2);

  switch (opcode->itype) {
  case BEXKAT1_F0_NARG:
    iword = opcode->opcode << 5;
    while (ISSPACE(*op_end))
      op_end++;
    if (*op_end != 0)
      as_warn("extra stuff on line ignored");
    break;
  case BEXKAT1_F0_A:
    iword = opcode->opcode << 5;
    while (ISSPACE (*op_end))
      op_end++;
    {
      int regnum;

      if (*op_end != 'r') {
	as_bad("expecting register");
	ignore_rest_of_line();
	return;
      }
      regnum = parse_regnum(&op_end);
      if (regnum == -1)
	return;
      
      iword |= regnum;
    }
    break;
  case BEXKAT1_F1_AB:
    iword = 0x4000 | (opcode->opcode << 5);
    while (ISSPACE (*op_end))
      op_end++;
    {
      int regnum;
      char *where;
      unsigned short w2;

      if (*op_end != 'r') {
	as_bad("expecting register");
	ignore_rest_of_line();
	return;
      }
      regnum = parse_regnum(&op_end);
      if (regnum == -1)
	return;
      
      iword |= regnum;

      while (ISSPACE(*op_end))
	op_end++;

      if (*op_end != ',') {
	as_bad("expecting comma delimited operands");
	ignore_rest_of_line();
	return;
      }
      op_end++;

      while (ISSPACE(*op_end))
	op_end++;

      if (*op_end != 'r') {
	as_bad("expecting register");
	ignore_rest_of_line();
	return;
      }
      regnum = parse_regnum(&op_end);
      if (regnum == -1)
	return;

      w2 = (regnum << 8);

      where = frag_more(2);
      md_number_to_chars(where, w2, 2);
    }
    break;
  case BEXKAT1_F1_ABC:
    iword = 0x4000 | (opcode->opcode << 5);
    while (ISSPACE (*op_end))
      op_end++;
    {
      int regnum;
      char *where;
      unsigned short w2;

      if (*op_end != 'r') {
	as_bad("expecting register");
	ignore_rest_of_line();
	return;
      }
      regnum = parse_regnum(&op_end);
      if (regnum == -1)
	return;
      
      iword |= regnum;

      while (ISSPACE(*op_end))
	op_end++;

      if (*op_end != ',') {
	as_bad("expecting comma delimited operands");
	ignore_rest_of_line();
	return;
      }
      op_end++;

      while (ISSPACE(*op_end))
	op_end++;

      if (*op_end != 'r') {
	as_bad("expecting register");
	ignore_rest_of_line();
	return;
      }
      regnum = parse_regnum(&op_end);
      if (regnum == -1)
	return;

      w2 = (regnum << 8);

      while (ISSPACE(*op_end))
	op_end++;

      if (*op_end != ',') {
	as_bad("expecting comma delimited operands");
	ignore_rest_of_line();
	return;
      }
      op_end++;

      while (ISSPACE(*op_end))
	op_end++;

      if (*op_end != 'r') {
	as_bad("expecting register");
	ignore_rest_of_line();
	return;
      }
      regnum = parse_regnum(&op_end);
      if (regnum == -1)
	return;

      w2 |= regnum;
      where = frag_more(2);
      md_number_to_chars(where, w2, 2);
    }
    break;
  case BEXKAT1_F2_A_16V:
    iword = 0x8000 | (opcode->opcode << 5);
    while (ISSPACE (*op_end))
      op_end++;
    {
      expressionS arg;
      char *where;
      int regnum;

      if (*op_end != 'r') {
	as_bad("expecting register");
	ignore_rest_of_line();
	return;
      }
      regnum = parse_regnum(&op_end);
      if (regnum == -1)
	return;
      
      iword |= regnum;
 
      while (ISSPACE(*op_end))
	op_end++;
      
      if (*op_end != ',') {
	as_bad("expecting comma delimited operands");
	ignore_rest_of_line();
	return;
      }
      op_end++;
      op_end = parse_exp_save_ilp(op_end, &arg);
      where = frag_more(2);
      fix_new_exp(frag_now,
		  (where - frag_now->fr_literal),
		  2,
		  &arg,
		  0,
		  BFD_RELOC_16);
    }
    break;
  case BEXKAT1_F2_A_RELADDR:
    iword = 0x8000 | (opcode->opcode << 5);
    break;
  case BEXKAT1_F3_A_32V:
    iword = 0xc000 | (opcode->opcode << 5);
    break;
  case BEXKAT1_F3_A_ABSADDR:
    iword = 0xc000 | (opcode->opcode << 5);
    break;
  }

  md_number_to_chars(p, iword, 2);

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
  char *buf = fixP->fx_where + fixP->fx_frag->fr_literal;
  long val = *valP;
  long max, min;
  // int shift;

  max = min = 0;
  //  shift = 0;
  switch (fixP->fx_r_type) {
  case BFD_RELOC_16:
    *buf++ = val >> 8;
    *buf++ = val >> 0;
    break;
  default:
    abort();
  }

  if (max != 0 && (val < min || val > max))
    as_bad_where(fixP->fx_file, fixP->fx_line, _("offset out of range"));
  if (fixP->fx_addsy == NULL && fixP->fx_pcrel == 0)
    fixP->fx_done = 1;
}

void
md_number_to_chars(char *ptr, valueT use, int nbytes)
{
  number_to_chars_bigendian(ptr, use, nbytes);
}

long
md_pcrel_from(fixS *fixP)
{
  valueT addr = fixP->fx_where + fixP->fx_frag->fr_address;

  fprintf(stderr, "md_pcrel_from 0x%d\n", fixP->fx_r_type);

  switch (fixP->fx_r_type) {
  case BFD_RELOC_16:
    return addr + 2;
  default:
    abort();
  }
  return addr;
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
