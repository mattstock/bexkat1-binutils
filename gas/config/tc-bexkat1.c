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

static int
find_opcode_full(const char *name, const unsigned int mode, 
		 const unsigned int size)
{
  int i;
  for (i=0; i < bexkat1_opc_count; i++)
    if (!strcmp(bexkat1_opc_info[i].name, name) &&
	bexkat1_opc_info[i].addr_mode == mode &&
	bexkat1_opc_info[i].size == size)
      return i;
  return -1;
}

static int
find_opcode_mode(const char *name, const unsigned int mode)
{
  int i;
  for (i=0; i < bexkat1_opc_count; i++)
    if (!strcmp(bexkat1_opc_info[i].name, name) &&
	bexkat1_opc_info[i].addr_mode == mode)
      return i;
  return -1;
}

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
    as_bad(_("illegal register number 1 %s"), s);
    ignore_rest_of_line();
    return -1;
  }

  if ((reg > 0) || (reg < 4)) {
    int r2 = s[2] - '0';
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
  int op_idx;
  const bexkat1_opc_info_t *opcode;
  char *p, *where;
  char pend;
  unsigned short iword, iword2;
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

  p = frag_more(2);  

  // INH addressing mode opcodes
  if (*op_end == 0) { // it must be INH
    op_idx = find_opcode_mode(op_name, BEXKAT1_ADDR_INH);
    if (op_idx == -1) {
      as_bad(_("unexpected args or invalid opcode %s"), op_start);
      return;
    }
    opcode = &(bexkat1_opc_info[op_idx]);
    iword = (BEXKAT1_ADDR_INH << 13) | opcode->opcode << 5;
    md_number_to_chars(p, iword, 2);
    return;
  }

  // PCIND, DIR, or REGIND version of jmp/jsr?
  if (*op_end != '%') {
    op_idx = find_opcode_mode(op_name, BEXKAT1_ADDR_PCIND);
    if (op_idx != -1) {
      opcode = &(bexkat1_opc_info[op_idx]);
      iword = (BEXKAT1_ADDR_PCIND << 13) | (opcode->opcode << 5);
      md_number_to_chars(p, iword, 2);
      where = frag_more(2);
      op_end = parse_exp_save_ilp(op_end, &arg);
      fix_new_exp(frag_now,
		  (where - frag_now->fr_literal),
		  2,
		  &arg,
		  TRUE,
		  BFD_RELOC_16_PCREL);
      while (ISSPACE(*op_end))
	op_end++;
      if (*op_end != 0)
	as_warn("extra stuff on line ignored %s %c", op_start, *op_end);
      if (pending_reloc)
	as_bad("Something forgot to clean up\n");
      return;
    }

    // Need to determine if this is REGIND or DIR
    op_end = parse_exp_save_ilp(op_end, &arg);
    while (ISSPACE(*op_end))
      op_end++;
    if (*op_end == 0) { // DIR
      op_idx = find_opcode_mode(op_name, BEXKAT1_ADDR_DIR);
      if (op_idx != -1) {
        opcode = &(bexkat1_opc_info[op_idx]);
        iword = (BEXKAT1_ADDR_DIR << 13) | (opcode->opcode << 5);      
        md_number_to_chars(p, iword, 2);
        where = frag_more(4);
        fix_new_exp(frag_now,
                    (where - frag_now->fr_literal),
		      4,
		      &arg,
		      0,
		      BFD_RELOC_32);
      } else {
        as_bad(_("unexpected args or invalid opcode %s"), op_name);
        return;
      }
    }
    if (*op_end == '(') { // REGIND
      op_end++; // burn paren
      while (ISSPACE(*op_end))
        op_end++;
      if (*op_end != '%') {
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
      op_idx = find_opcode_mode(op_name, BEXKAT1_ADDR_REGIND);
      if (op_idx != -1) {
        opcode = &(bexkat1_opc_info[op_idx]);
        iword = (BEXKAT1_ADDR_REGIND << 13) | (opcode->opcode << 5);      
        md_number_to_chars(p, iword, 2);
        iword2 = (regnum << 11);
        where = frag_more(2);
        fix_new_exp(frag_now,
	      (where - frag_now->fr_literal),
	      2,
	      &arg,
	      0,
	      BFD_RELOC_BEXKAT_11);
        md_number_to_chars(where, iword2, 2);
      } else {
        as_bad(_("unexpected args or invalid opcode %s"), op_name);
        return;
      }
    }
    if (*op_end != 0)
      as_warn("extra stuff on line ignored %s %c", op_start, *op_end);
    if (pending_reloc)
      as_bad("Something forgot to clean up\n");
    return;
  }

  // We have a register as the first arg
  regnum = parse_regnum(&op_end);
  if (regnum == -1)
    return;
  iword = regnum;

  while (ISSPACE(*op_end))
    op_end++;

  // See if this is REG
  if (*op_end != ',') {
    op_idx = find_opcode_mode(op_name, BEXKAT1_ADDR_REG);
    if (op_idx == -1) {
      as_bad(_("unexpected args for %s"), op_start);
      ignore_rest_of_line();
      return;
    }
    opcode = &(bexkat1_opc_info[op_idx]);
    iword |= (BEXKAT1_ADDR_REG << 13) | (opcode->opcode << 5);
    md_number_to_chars(p, iword, 2);

    while (ISSPACE(*op_end))
      op_end++;
    if (*op_end != 0)
      as_warn("extra stuff on line ignored %s %c", op_start, *op_end);
    if (pending_reloc)
      as_bad("Something forgot to clean up\n");
    return;
  }

  op_end++; // burn comma
  while (ISSPACE (*op_end))
    op_end++;

  // See if we have a 2nd reg
  if (*op_end == '%') {
    where = frag_more(2);
    regnum = parse_regnum(&op_end);
    if (regnum == -1)
      return;
    iword2 = (regnum << 8);

    while (ISSPACE(*op_end))
      op_end++;

    if (*op_end == 0) { // two reg varient
      op_idx = find_opcode_full(op_name, BEXKAT1_ADDR_REG, 2);
      if (op_idx == -1) {
	as_bad(_("unexpected args for %s"), op_start);
	ignore_rest_of_line();
	return;
      }
      opcode = &(bexkat1_opc_info[op_idx]);
      iword |= (BEXKAT1_ADDR_REG << 13) | (opcode->opcode << 5);
      md_number_to_chars(p, iword, 2);
      md_number_to_chars(where, iword2, 2);
      
      while (ISSPACE(*op_end))
	op_end++;
      if (*op_end != 0)
	as_warn(_("extra stuff on line ignored %s %c\n"), op_start, *op_end);
      if (pending_reloc)
	as_bad("Something forgot to clean up\n");
      return;
    }

    if (*op_end != ',') {
      as_bad(_("expecting comma after %s"), op_start);
      ignore_rest_of_line();
      return;
    }

    // three args, either REG or IMM
    op_end++;
    while (ISSPACE(*op_end))
      op_end++;
      
    // 3 reg opcode
    if (*op_end == '%') {
      op_idx = find_opcode_mode(op_name, BEXKAT1_ADDR_REG);
      if (op_idx == -1) {
	as_bad(_("unexpected args for %s"), op_start);
	ignore_rest_of_line();
	return;
      }
      opcode = &(bexkat1_opc_info[op_idx]);
      iword |= (BEXKAT1_ADDR_REG << 13) | (opcode->opcode << 5);
      regnum = parse_regnum(&op_end);
      if (regnum == -1)
	return;
      iword2 |= regnum;
      md_number_to_chars(p, iword, 2);
      md_number_to_chars(where, iword2, 2);
      
      while (ISSPACE(*op_end))
	op_end++;
      if (*op_end != 0)
	as_warn("extra stuff on line ignored %s %c", op_start, *op_end);
      if (pending_reloc)
	as_bad("Something forgot to clean up\n");
      return;
    }

    // 3 word IMM
    op_idx = find_opcode_full(op_name, BEXKAT1_ADDR_IMM, 3);
    if (op_idx == -1) {
      as_bad(_("unexpected args for %s"), op_start);
      ignore_rest_of_line();
      return;
    }
    opcode = &(bexkat1_opc_info[op_idx]);
    iword |= (BEXKAT1_ADDR_IMM << 13) | (opcode->opcode << 5);
    md_number_to_chars(p, iword, 2);
    md_number_to_chars(where, iword2, 2);

    op_end = parse_exp_save_ilp(op_end, &arg);
    where = frag_more(2);
    fix_new_exp(frag_now,
		(where - frag_now->fr_literal),
		2,
		&arg,
		0,
		BFD_RELOC_16);
    while (ISSPACE(*op_end))
      op_end++;
    if (*op_end != 0)
      as_warn("extra stuff on line ignored %s %c", op_start, *op_end);
    if (pending_reloc)
      as_bad("Something forgot to clean up\n");
    return;  
  }

  // 2nd arg is non-register
  
  // Things get interesting here, since there are some ambiguities in
  // the ISA here.  For example, there are two add IMM opcodes, depending on
  // the size of the constant.

  // parse expression
  // handle ldi as a special case
  // check for paren
  // check for IMM vs DIR
  op_end = parse_exp_save_ilp(op_end, &arg);
  if (!strcmp("ldi", op_name)) {
    op_idx = find_opcode_mode(op_name, BEXKAT1_ADDR_IMM);
    if (op_idx == -1) {
      as_bad(_("unexpected args or invalid opcode %s"), op_start);
      return;
    }
    opcode = &(bexkat1_opc_info[op_idx]);
    iword |= (BEXKAT1_ADDR_IMM << 13) | (opcode->opcode << 5);
    md_number_to_chars(p, iword, 2);
    where = frag_more(4);
    fix_new_exp(frag_now,
		(where - frag_now->fr_literal),
		4,
		&arg,
		0,
		BFD_RELOC_32);
    while (ISSPACE(*op_end))
      op_end++;
    if (*op_end != 0)
      as_warn("extra stuff on line ignored %s %c", op_start, *op_end);
    if (pending_reloc)
      as_bad("Something forgot to clean up\n");
    return;  
  }
  if (*op_end == '(') { // REGIND
    op_end++; // burn paren
    while (ISSPACE(*op_end))
      op_end++;
    if (*op_end != '%') {
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
    op_idx = find_opcode_mode(op_name, BEXKAT1_ADDR_REGIND);
    if (op_idx == -1) {
      as_bad(_("unexpected args or invalid opcode %s"), op_start);
      return;
    }
    opcode = &(bexkat1_opc_info[op_idx]);
    iword |= (BEXKAT1_ADDR_REGIND << 13) | (opcode->opcode << 5);
    md_number_to_chars(p, iword, 2);
    iword2 = (regnum << 11);
    where = frag_more(2);
    fix_new_exp(frag_now,
		(where - frag_now->fr_literal),
		2,
		&arg,
		0,
		BFD_RELOC_BEXKAT_11);
    md_number_to_chars(where, iword2, 2);
    while (ISSPACE(*op_end))
      op_end++;
    if (*op_end != 0)
      as_warn("extra stuff on line ignored %s %c", op_start, *op_end);
    if (pending_reloc)
      as_bad("Something forgot to clean up\n");
    return;  
  }

  op_idx = find_opcode_full(op_name, BEXKAT1_ADDR_IMM, 2);
  if (op_idx != -1) {
    opcode = &(bexkat1_opc_info[op_idx]);
    iword |= (BEXKAT1_ADDR_IMM << 13) | (opcode->opcode << 5);
    md_number_to_chars(p, iword, 2);
    where = frag_more(2);
    fix_new_exp(frag_now,
		(where - frag_now->fr_literal),
		2,
		&arg,
		0,
		BFD_RELOC_16);
    while (ISSPACE(*op_end))
      op_end++;
    if (*op_end != 0)
      as_warn("extra stuff on line ignored %s %c", op_start, *op_end);
    if (pending_reloc)
      as_bad("Something forgot to clean up\n");
    return;  
  }

  op_idx = find_opcode_full(op_name, BEXKAT1_ADDR_DIR, 3);
  if (op_idx != -1) {
    opcode = &(bexkat1_opc_info[op_idx]);
    iword |= (BEXKAT1_ADDR_DIR << 13) | (opcode->opcode << 5);
    md_number_to_chars(p, iword, 2);
    where = frag_more(4);
    fix_new_exp(frag_now,
		(where - frag_now->fr_literal),
		4,
		&arg,
		0,
		BFD_RELOC_32);
    while (ISSPACE(*op_end))
      op_end++;
    if (*op_end != 0)
      as_warn("extra stuff on line ignored %s %c", op_start, *op_end);
    if (pending_reloc)
      as_bad("Something forgot to clean up\n");
    return;  
  }

  as_bad("failed to parse line");
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
md_apply_fix(fixS *fixP, valueT *valP, segT seg ATTRIBUTE_UNUSED)
{
  char *buf = fixP->fx_where + fixP->fx_frag->fr_literal;
  long val = *valP;

  switch (fixP->fx_r_type) {
  case BFD_RELOC_BEXKAT_11:
    if (val < -1024 || val > 1024)
      as_bad_where(fixP->fx_file, fixP->fx_line,
		   _("Constant out of 11 bit range for BFD_RELOC_BEXKAT_10"));
    buf[0] |= ((val >> 8) & 0x07);
    buf[1] = (val & 0xff);
    break;
  case BFD_RELOC_16:
    if (target_big_endian)
      {
	buf[0] = val >> 8;
	buf[1] = val >> 0;
      }
    else
      {
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
