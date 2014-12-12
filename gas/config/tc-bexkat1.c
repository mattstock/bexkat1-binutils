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

const pseudo_typeS md_pseudo_table[] =
  {
    { 0, 0, 0}
  };

void
md_operand (expressionS *op __attribute__((unused)))
{
}

static const bexkat1_opc_info_t *
find_opcode(const char *name) {
  int i;
  for (i=0; i < BEXKAT1_OPC_COUNT; i++)
    if (!strncmp(bexkat1_opc_info[i].name, name, strlen(name)))
      return &bexkat1_opc_info[i];
  return NULL;
}

static const bexkat1_opc_info_t *
find_opcode_form(const char *name, unsigned form) {
  int i;
  for (i=0; i < BEXKAT1_OPC_COUNT; i++)
    if (bexkat1_opc_info[i].itype == form &&
	!strncmp(bexkat1_opc_info[i].name, name, strlen(name)))
      return &bexkat1_opc_info[i];
  return NULL;
}

/*
static const bexkat1_opc_info_t *
find_opcode_addr(const char *name, unsigned addr) {
  int i;
  for (i=0; i < BEXKAT1_OPC_COUNT; i++)
    if (bexkat1_opc_info[i].addr_mode == addr &&
	!strncmp(bexkat1_opc_info[i].name, name, strlen(name)))
      return &bexkat1_opc_info[i];
  return NULL;
  } */

void
md_begin (void)
{
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
  char op_name[10];
  const bexkat1_opc_info_t *opcode;
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

  strncpy(op_name, op_start, 10);

  if (nlen == 0)
    as_bad(_("can't find opcode "));
  
  opcode = find_opcode(op_start);
  *op_end = pend;
  if (opcode == NULL) {
    as_bad(_("unknown opcode %s"), op_start);
    return;
  }
  
  p = frag_more(2);
  
  if (opcode->itype == BEXKAT1_F0_NARG) {
    iword = opcode->opcode << 5;
    while (ISSPACE(*op_end))
      op_end++;
    if (*op_end != 0)
      as_warn("extra stuff on line ignored");
  } else { // need to parse the rest of the line to see address mode
    while (ISSPACE (*op_end))
      op_end++;
    
    if (*op_end != 'r') {
      expressionS arg;
      char *where;

      if (opcode->addr_mode == BEXKAT1_ADDR_PCREL) {
	opcode = find_opcode_form(op_name, BEXKAT1_F2_RELADDR);
	if (opcode == NULL) {
	  as_bad(_("unexpected args F2_RELADDR for %s"), op_start);
	  return;
	}
	where = frag_more(2);
	op_end = parse_exp_save_ilp(op_end, &arg);
	fix_new_exp(frag_now,
		    (where - frag_now->fr_literal),
		    2,
		    &arg,
		    TRUE,
		    BFD_RELOC_16_PCREL);
	iword = 0x8000 | (opcode->opcode << 5);
      } else {
	opcode = find_opcode_form(op_name, BEXKAT1_F3_ABSADDR);
	if (opcode == NULL) {
	  as_bad(_("unexpected args F3_ABSADDR for %s"), op_start);
	  return;
	}
	op_end = parse_exp_save_ilp(op_end, &arg);
	where = frag_more(4);
	fix_new_exp(frag_now,
		    (where - frag_now->fr_literal),
		    4,
		    &arg,
		    0,
		    BFD_RELOC_32);
	iword = 0xc000 | (opcode->opcode << 5);      
      }
    } else { // first arg is a register
      int regnum;
      unsigned short w2;
      expressionS arg;
      char *where;
      regnum = parse_regnum(&op_end);
      if (regnum == -1)
	return;
      iword = regnum;
      while (ISSPACE(*op_end))
	op_end++;
      
      if (*op_end != ',') { // F0_A
	op_end++;
	opcode = find_opcode_form(op_name, BEXKAT1_F0_A);
	if (opcode == NULL) {
	  as_bad(_("unexpected args F0_A for %s"), op_start);
	  return;
	}
	iword |= (opcode->opcode << 5);
      } else { // two or more args
	op_end++; // burn comma
	while (ISSPACE (*op_end))
	  op_end++;
	switch (*op_end) {
	case 'r':
	  regnum = parse_regnum(&op_end);
	  if (regnum == -1)
	    return;
	  w2 = (regnum << 8);
	  
	  while (ISSPACE(*op_end))
	    op_end++;
	  
	  if (*op_end == ',') { // F1_ABC
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
	    opcode = find_opcode_form(op_name, BEXKAT1_F1_ABC);
	    if (opcode == NULL) {
	      as_bad(_("unexpected args for F1_ABC %s"), op_start);
	      return;
	    }
	  } else { // F1_AB
	    opcode = find_opcode_form(op_name, BEXKAT1_F1_AB);
	    if (opcode == NULL) {
	      as_bad(_("unexpected args for F1_AB %s"), op_start);
	      return;
	    }
	    // should make sure there isn't junk on the end of the line
	  }
	  
	  iword = 0x4000 | (opcode->opcode << 5);
	  where = frag_more(2);
	  md_number_to_chars(where, w2, 2);
	  break;
	case '(':
	  op_end++; // burn paren
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
	  if (*op_end != ')') {
	    as_bad(_("missing close paren"));
	    ignore_rest_of_line();
	    return;
	  }
	  op_end++;
	  
	  w2 = (regnum << 8);
	  
	  while (ISSPACE(*op_end))
	    op_end++;
	  
	  where = frag_more(2);
	  md_number_to_chars(where, w2, 2);
	  
	  opcode = find_opcode_form(op_name, BEXKAT1_F1_AB);
	  if (opcode == NULL) {
	    as_bad(_("invalid arg for F1_AB %s"), op_start);
	    return;
	  }
	  iword = 0x4000 | (opcode->opcode << 5);
	  break;
	default:
	  opcode = find_opcode_form(op_name, BEXKAT1_F3_A_32V);
	  if (opcode == NULL) {
	    opcode = find_opcode_form(op_name, BEXKAT1_F3_A_ABSADDR);
	    if (opcode == NULL) {
	      as_bad(_("unexpected args F3_A_* for %s"), op_start);
	      return;
	    }
	  }
	  while (ISSPACE(*op_end))
	    op_end++;
	  op_end = parse_exp_save_ilp(op_end, &arg);
	  where = frag_more(4);
	  fix_new_exp(frag_now,
		      (where - frag_now->fr_literal),
		      4,
		      &arg,
		      0,
		      BFD_RELOC_32);
	  iword = 0xc000 | (opcode->opcode << 5);
	}
      }
    }
  }

  md_number_to_chars(p, iword, 2);

  while (ISSPACE(*op_end))
    op_end++;

  if (*op_end != 0)
    as_warn("extra stuff on line ignored %s %c", op_start, *op_end);

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
  short sval;

  max = min = 0;
  switch (fixP->fx_r_type) {
  case BFD_RELOC_16:
    *buf++ = val >> 8;
    *buf++ = val >> 0;
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
    sval = val >> 1;
    fprintf(stderr, "pcrel = %ld (%d)\n", val, sval);
    md_number_to_chars(buf, sval, 2);
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

  switch (fixP->fx_r_type) {
  case BFD_RELOC_16_PCREL:
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
