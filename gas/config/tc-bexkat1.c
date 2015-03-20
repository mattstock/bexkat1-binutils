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

const char comment_chars[] = "#";
const char line_comment_chars[] = "#";
const char line_separator_chars[] = ";";

const char EXP_CHARS[] = "eE";
const char FLT_CHARS[] = "rRsSfFdDxXpP";

static int pending_reloc;
static struct hash_control *opcode_hash_control;

extern int target_big_endian;

const pseudo_typeS md_pseudo_table[] =
  {
    { 0, 0, 0}
  };

void
md_operand (expressionS *op __attribute__((unused)))
{
}

void
md_begin (void)
{
  int count;
  const bexkat1_opc_info_t *opcode;
  opcode_hash_control = hash_new();

  bfd_set_arch_mach(stdoutput, TARGET_ARCH, 0);

  opcode = bexkat1_opc_info;
  for (count=0; count < bexkat1_opc_count; count++) {
    hash_insert(opcode_hash_control, opcode->name, (char *)opcode);
    opcode++;
  }
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

  if (*s != '%') {
    as_bad(_("not a register: %s"), s);
    return -1;
  }
  s++;

  // optional f to mark a floating point register
  if (*s == 'f') {
    s++;
    *ptr += 1;
  }

  reg = *s - '0';
  if ((reg < 0) || (reg > 9)) {
    as_bad(_("illegal register number 1 %s"), s);
    ignore_rest_of_line();
    return -1;
  }
  s++;

  if ((reg > 0) || (reg < 4)) {
    int r2 = *s - '0';
    if ((r2 >= 0) && (r2 <= 9)) {
      reg = reg*10 + r2;
      if (reg > 31) {
	as_bad(_("illegal register number %d"), reg);
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
  char op_name[10];
  const bexkat1_opc_info_t *opcode;
  char *p;
  char pend;
  unsigned int iword;
  int nlen = 0;
  expressionS arg;
  int regnum;
  
  while (*str == ' ')
    str++;
  
  // mark opcode
  op_start = str;
  for (op_end = str;
       *op_end && !is_end_of_line[*op_end & 0xff] && *op_end != ' ';
       op_end++)
    nlen++;
  
  pend = *op_end;
  *op_end = 0;
  strncpy(op_name, op_start, 10);
  *op_end = pend;

  if (nlen == 0)
    as_bad(_("can't find opcode "));
  
  while (ISSPACE(*op_end))
    op_end++;
  
  opcode = (bexkat1_opc_info_t *) hash_find(opcode_hash_control, op_name);
  if (opcode == NULL) {
    as_bad(_("unknown opcode %s"), op_name);
    return;
  }

  switch (opcode->mode) {
  case BEXKAT1_REG:
    iword = (BEXKAT1_REG << 29) | (opcode->opcode << 21);
    if (opcode->args > 0) {
      regnum = parse_regnum(&op_end);
      if (regnum == -1)
	return; 
      while (ISSPACE(*op_end))
	op_end++;
      iword |= (regnum & 0x1f) << 16;
    }
    if (opcode->args > 1) {
      if (*op_end != ',') {
	as_bad(_("missing comma: %s"), op_end);
	return;
      }
      op_end++;
      while (ISSPACE(*op_end))
	op_end++;
      regnum = parse_regnum(&op_end);
      if (regnum == -1)
	return;
      iword |= ((regnum & 0x1f) << 11);
    }
    if (opcode->args > 2) {
      if (*op_end != ',') {
	as_bad(_("missing comma: %s"), op_end);
	return;
      }
      op_end++;
      while (ISSPACE(*op_end))
	op_end++;
      regnum = parse_regnum(&op_end);
      if (regnum == -1)
	return;
      iword |= ((regnum & 0x1f) << 6);
    }

    p = frag_more(4);  
    md_number_to_chars(p, iword, 4);
    break;
  case BEXKAT1_IMM:
    iword = (BEXKAT1_IMM << 29) | (opcode->opcode << 21);
    p = frag_more(4);

    if (opcode->args == 2) {
      regnum = parse_regnum(&op_end);
      if (regnum == -1)
	return; 
      while (ISSPACE(*op_end))
	op_end++;
      if (*op_end != ',') {
	as_bad(_("missing comma: %s"), op_end);
	return;
      }
      op_end++;
      while (ISSPACE(*op_end))
	op_end++;
      iword |= (regnum & 0x1f) << 16;
      op_end = parse_exp_save_ilp(op_end, &arg);
      if (target_big_endian)
	fix_new_exp(frag_now,
		    (p + 2 - frag_now->fr_literal),
		    2,
		    &arg,
		    0,
		    BFD_RELOC_16);
      else
	fix_new_exp(frag_now,
		    (p - frag_now->fr_literal),
		    2,
		    &arg,
		    0,
		    BFD_RELOC_16);
    } else {
      op_end = parse_exp_save_ilp(op_end, &arg);
      if (target_big_endian)
	fix_new_exp(frag_now,
		    (p + 2 - frag_now->fr_literal),
		    2,
		    &arg,
		    TRUE,
		    BFD_RELOC_16_PCREL);
      else
	fix_new_exp(frag_now,
		    (p - frag_now->fr_literal),
		    2,
		    &arg,
		    TRUE,
		    BFD_RELOC_16_PCREL);
    }
    md_number_to_chars(p, iword, 4);
    break;
  case BEXKAT1_REGIND:
    iword = (BEXKAT1_REGIND << 29) | (opcode->opcode << 21);

    if (opcode->args == 3) {
      regnum = parse_regnum(&op_end);
      if (regnum == -1)
	return; 
      while (ISSPACE(*op_end))
	op_end++;
      if (*op_end != ',') {
	as_bad(_("missing comma: %s"), op_end);
	return;
      }
      op_end++;
      while (ISSPACE(*op_end))
	op_end++;
      iword |= (regnum & 0x1f) << 16;
    }
    op_end = parse_exp_save_ilp(op_end, &arg);
    if (*op_end != '(') {
      as_bad(_("missing open paren: %s"), op_end);
      ignore_rest_of_line();
      return;
    }
    op_end++; // burn paren
    while (ISSPACE(*op_end))
      op_end++;
    regnum = parse_regnum(&op_end);
    if (regnum == -1)
      return;
    if (*op_end != ')') {
      as_bad(_("missing close paren: %s"), op_end);
      ignore_rest_of_line();
      return;
    }
    op_end++;
    iword |= (regnum & 0x1f) << 11;
    p = frag_more(4);
    if (target_big_endian)
      fix_new_exp(frag_now,
		  (p + 2 - frag_now->fr_literal),
		  2,
		  &arg,
		  0,
		  BFD_RELOC_BEXKAT_11);
    else
      fix_new_exp(frag_now,
		  (p - frag_now->fr_literal),
		  2,
		  &arg,
		  0,
		  BFD_RELOC_BEXKAT_11);
    md_number_to_chars(p, iword, 4);
    break;
  case BEXKAT1_DIR:
    iword = (BEXKAT1_DIR << 29) | (opcode->opcode << 21);
    if (opcode->args > 1) {
      regnum = parse_regnum(&op_end);
      if (regnum == -1)
	return; 
      while (ISSPACE(*op_end))
	op_end++;
      if (*op_end != ',') {
	as_bad(_("missing comma: %s"), op_end);
	return;
      }
      op_end++;
      iword |= (regnum & 0x1f) << 16;
    }
    if (opcode->args > 2) {
      regnum = parse_regnum(&op_end);
      if (regnum == -1)
	return; 
      while (ISSPACE(*op_end))
	op_end++;
      if (*op_end != ',') {
	as_bad(_("missing comma: %s"), op_end);
	return;
      }
      op_end++;
      iword |= (regnum & 0x1f) << 11;
    }
    p = frag_more(4);
    md_number_to_chars(p, iword, 4);
    op_end = parse_exp_save_ilp(op_end, &arg);
    p = frag_more(4);
    fix_new_exp(frag_now,
		(p - frag_now->fr_literal),
		4,
		&arg,
		0,
		BFD_RELOC_32);
    break;
  }

  while (ISSPACE(*op_end))
    op_end++;
  if (*op_end != 0)
    as_warn("extra stuff on line ignored %s %c", op_start, *op_end);
  if (pending_reloc)
    as_bad("Something forgot to clean up\n");
  return;
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

enum options {
  OPTION_EB = OPTION_MD_BASE,
  OPTION_EL
};

struct option md_longopts[] =
{
  { "EB", no_argument, NULL, OPTION_EB},
  { "EL", no_argument, NULL, OPTION_EL},
  {  NULL, no_argument, NULL, 0}
};

size_t md_longopts_size = sizeof(md_longopts);

int
md_parse_option(int c, char *arg ATTRIBUTE_UNUSED)
{
  target_big_endian = 1;
  switch (c) {
  case OPTION_EB:
    target_big_endian = 1;
    break;
  case OPTION_EL:
    target_big_endian = 0;
    break;
  default:
    return 0;
  }
  return 1;
}

void
md_show_usage(FILE *stream)
{
  fprintf(stream, _("\
    -EB   big endian (default)\n\
    -EL   little endian\n"));
}

void
md_apply_fix(fixS *fixP, valueT *valP, segT seg ATTRIBUTE_UNUSED)
{
  char *buf = fixP->fx_where + fixP->fx_frag->fr_literal;
  long val = *valP;

  switch (fixP->fx_r_type) {
  case BFD_RELOC_BEXKAT_11:
    if (val < -1024 || val > 1024)
      as_bad_where(fixP->fx_file, fixP->fx_line,
		   _("Constant out of 11 bit range for BFD_RELOC_BEXKAT_11"));
    if (target_big_endian) {
      buf[0] |= ((val >> 8) & 0x07);
      buf[1] = (val & 0xff);
    } else {
      buf[1] |= ((val >> 8) & 0x07);
      buf[0] = (val & 0xff);
    }
    
    break;
  case BFD_RELOC_16:
    if (target_big_endian) {
      buf[0] = val >> 8;
      buf[1] = val >> 0;
    } else {
      buf[1] = val >> 8;
      buf[0] = val >> 0;
    }
    break;
  case BFD_RELOC_32:
    if (target_big_endian) {
      buf[0] = val >> 24;
      buf[1] = val >> 16;
      buf[2] = val >> 8;
      buf[3] = val >> 0;
    } else {
      buf[3] = val >> 24;
      buf[2] = val >> 16;
      buf[1] = val >> 8;
      buf[0] = val >> 0;
    }
    break;
  case BFD_RELOC_16_PCREL:
    if (!val)
      break;
    if (val < -32766 || val > 32767)
      as_bad_where(fixP->fx_file, fixP->fx_line,
		   _("pcrel too far BFD_RELOC_16_PCREL"));
    md_number_to_chars(buf, (short)val, 2);
    break;
  default:
    as_fatal (_("Line %d: unknown relocation type: 0x%x."),
	      fixP->fx_line, fixP->fx_r_type);
  }

  if (fixP->fx_addsy == NULL && fixP->fx_pcrel == 0)
    fixP->fx_done = 1;
}

void
md_number_to_chars(char *ptr, valueT use, int nbytes)
{
  if (target_big_endian) {
    number_to_chars_bigendian(ptr, use, nbytes);
  } else {
    number_to_chars_littleendian(ptr, use, nbytes);
  }
}

long
md_pcrel_from(fixS *fixP)
{
  valueT addr = fixP->fx_where + fixP->fx_frag->fr_address;

  switch (fixP->fx_r_type) {
  case BFD_RELOC_16_PCREL:
    if (target_big_endian)
      return addr + 2;
    else
      return addr + 4;
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
  rel->addend = fixp->fx_offset; 
  rel->howto = bfd_reloc_type_lookup(stdoutput, r_type);

  if (rel->howto == NULL) {
    as_bad_where(fixp->fx_file, fixp->fx_line,
                 _("Cannot represent relocation type %s"),
                 bfd_get_reloc_code_name(r_type));
    rel->howto = bfd_reloc_type_lookup(stdoutput, BFD_RELOC_32);
    gas_assert(rel->howto != NULL);
  }

  /* Since we use Rel instead of Rela, encode the vtable entry to be
     used in the relocation's section offset.  */
  if (fixp->fx_r_type == BFD_RELOC_VTABLE_INHERIT
      || fixp->fx_r_type == BFD_RELOC_VTABLE_ENTRY)
    rel->address = fixp->fx_offset;

  return rel;
}
