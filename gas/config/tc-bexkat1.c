/* tc-bexkat1.c -- Assembler code for the Bexkat 1.
   Written by Matt Stock (stock@csgeeks.org)
 */

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

static valueT md_chars_to_number (char * buf, int n);

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

  // %fp alias for %14
  if (s[0] == 'f' && s[1] == 'p') {
    *ptr += 3;
    return 14;
  }

  // %sp alias for %15
  if (s[0] == 's' && s[1] == 'p') {
    *ptr += 3;
    return 15;
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
  int regnum;
  expressionS arg;
  int offset;

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

  iword = (opcode->type << 28) | (opcode->opcode << 24) | opcode->size;

  p = frag_more(4);

  switch (opcode->type) {
  case BEXKAT1_INH:
    if (opcode->args == 0) {
      md_number_to_chars(p, iword, 4);
    }
    if (opcode->args == 1) {
      op_end = parse_exp_save_ilp(op_end, &arg);
      if (opcode->size) {
	md_number_to_chars(p, iword, 4);
	p = frag_more(4);
	fix_new_exp(frag_now,
		    (p - frag_now->fr_literal),
		    4,
		    &arg,
		    0,
		    BFD_RELOC_32);
      } else {
	if (arg.X_op != O_constant) {
	  as_bad(_("offset is not a constant expression"));
	  ignore_rest_of_line();
	  return;
	}
	offset = arg.X_add_number;
	if (offset < -16384 || offset > 16383) {
	  as_bad(_("offset is out of range: %d\n"), offset);
	  ignore_rest_of_line();
	  return;
	}
	
	md_number_to_chars(p, iword, 4);
	fix_new_exp(frag_now,
		    (p - frag_now->fr_literal),
		    4,
		    &arg,
		    0,
		    BFD_RELOC_BEXKAT1_15);
      }
    }
    break;
  case BEXKAT1_PUSH:
    if (opcode->args == 1 && opcode->opcode == 2) // bsr
      goto branch;
    if (opcode->args == 1) {
      if (opcode->size) {
	op_end = parse_exp_save_ilp(op_end, &arg);
	md_number_to_chars(p, iword, 4);
	p = frag_more(4);
	fix_new_exp(frag_now,
		    (p - frag_now->fr_literal),
		    4,
		    &arg,
		    0,
		    BFD_RELOC_32);
      } else {
	regnum = parse_regnum(&op_end);
	if (regnum == -1)
	  return; 
	while (ISSPACE(*op_end))
	  op_end++;
	iword |= (regnum & 0xf) << 20; // A
	md_number_to_chars(p, iword, 4);
      }
    }
    if (opcode->args == 2) {
      if (*op_end != '(')
	op_end = parse_exp_save_ilp(op_end, &arg);
      else { // Implicit 0 offset to allow for indirect
	arg.X_op = O_constant;
	arg.X_add_number = 0;
      }
      
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
      while (ISSPACE(*op_end))
	op_end++;
      iword |= (regnum & 0xf) << 20; // A
      
      if (*op_end != ')') {
	as_bad(_("missing close paren: %s"), op_end);
	ignore_rest_of_line();
	return;
      }
      op_end++;
      
      if (arg.X_op != O_constant) {
	as_bad(_("offset is not a constant expression"));
	ignore_rest_of_line();
	return;
      }
      offset = arg.X_add_number;
      if (offset < -16384 || offset > 16383) {
	as_bad(_("offset is out of range: %d\n"), offset);
	ignore_rest_of_line();
	return;
      }

      md_number_to_chars(p, iword, 4);
      fix_new_exp(frag_now,
		  (p - frag_now->fr_literal),
		  4,
		  &arg,
		  0,
		  BFD_RELOC_BEXKAT1_15);
    }
    break;
  case BEXKAT1_POP:
    if (opcode->args == 0) {
      md_number_to_chars(p, iword, 4);
    }
    if (opcode->args == 1) {
      regnum = parse_regnum(&op_end);
      if (regnum == -1)
	return; 
      while (ISSPACE(*op_end))
	op_end++;
      iword |= (regnum & 0xf) << 20; // A
      md_number_to_chars(p, iword, 4);
    }
    break;
  case BEXKAT1_MOV:
    if (opcode->opcode == 0) {
      regnum = parse_regnum(&op_end);
      if (regnum == -1)
        return; 
      while (ISSPACE(*op_end))
        op_end++;
      iword |= (regnum & 0xf) << 20; // A
      md_number_to_chars(p, iword, 4);
      break;
    }
    __attribute__((fallthrough));
  case BEXKAT1_CMP:
  case BEXKAT1_FPU:
  case BEXKAT1_FP:
  case BEXKAT1_ALU:
  case BEXKAT1_INT:
  case BEXKAT1_INTU:
    if (opcode->args > 1) {
      regnum = parse_regnum(&op_end);
      if (regnum == -1)
        return; 
      while (ISSPACE(*op_end))
        op_end++;
      iword |= (regnum & 0xf) << 20; // A

      if (*op_end != ',') {
        as_bad(_("missing comma: %s"), op_end);
        return;
      }
      op_end++;
      while (ISSPACE(*op_end))
        op_end++;
    
    }

    regnum = parse_regnum(&op_end);
    if (regnum == -1)
      return; 
    while (ISSPACE(*op_end))
      op_end++;
    iword |= (regnum & 0xf) << 16; // B

    if (opcode->args > 2) {

      if (*op_end != ',') {
	as_bad(_("missing comma: %s"), op_end);
	return;
      }
      op_end++;
      while (ISSPACE(*op_end))
	op_end++;
      
      if (opcode->opcode < 8) {
	regnum = parse_regnum(&op_end);
	if (regnum == -1)
	  return; 
	while (ISSPACE(*op_end))
	  op_end++;
	iword |= (regnum & 0xf) << 12; // C
	md_number_to_chars(p, iword, 4);
      } else {
	op_end = parse_exp_save_ilp(op_end, &arg);
	if (arg.X_op != O_constant) {
	  as_bad(_("offset is not a constant expression"));
	  ignore_rest_of_line();
	  return;
	}
	offset = arg.X_add_number;
	if (offset < -16384 || offset > 16383) {
	  as_bad(_("offset is out of range: %d\n"), offset);
	  ignore_rest_of_line();
	  return;
	}
	md_number_to_chars(p, iword, 4);
	fix_new_exp(frag_now,
		    (p - frag_now->fr_literal),
		    4,
		    &arg,
		    0,
		    BFD_RELOC_BEXKAT1_15);
      }
    } else {
      md_number_to_chars(p, iword, 4);
    }
    break;
branch:
  case BEXKAT1_BRANCH:
    md_number_to_chars(p, iword, 4);
    op_end = parse_exp_save_ilp(op_end, &arg);
    if (target_big_endian)
      fix_new_exp(frag_now,
		  (p - frag_now->fr_literal),
		  4,
		  &arg,
		  TRUE,
		  BFD_RELOC_BEXKAT1_15_PCREL);
    else
      fix_new_exp(frag_now,
		  (p - frag_now->fr_literal),
		  4,
		  &arg,
		  TRUE,
		  BFD_RELOC_BEXKAT1_15_PCREL);
    break;
  case BEXKAT1_LOAD:
  case BEXKAT1_STORE:
    // A, 
    regnum = parse_regnum(&op_end);
    if (regnum == -1)
      return; 
    while (ISSPACE(*op_end))
      op_end++;
    iword |= (regnum & 0xf) << 20; // A
    
    if (*op_end != ',') {
      as_bad(_("missing comma: %s"), op_end);
      return;
    }
    op_end++;
    while (ISSPACE(*op_end))
      op_end++;
    __attribute__((fallthrough));
  case BEXKAT1_JUMP:
    if (opcode->size) { // big address
      md_number_to_chars(p, iword, 4);
      op_end = parse_exp_save_ilp(op_end, &arg);
      p = frag_more(4);
      fix_new_exp(frag_now,
		  (p - frag_now->fr_literal),
		  4,
		  &arg,
		  0,
		  BFD_RELOC_32);
    } else { // exp(B)
      if (*op_end != '(')
	op_end = parse_exp_save_ilp(op_end, &arg);
      else { // Implicit 0 offset to allow for indirect
	arg.X_op = O_constant;
	arg.X_add_number = 0;
      }
      
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
      while (ISSPACE(*op_end))
	op_end++;
      iword |= (regnum & 0xf) << 16; // B
      
      if (*op_end != ')') {
	as_bad(_("missing close paren: %s"), op_end);
	ignore_rest_of_line();
	return;
      }
      op_end++;
      
      if (arg.X_op != O_constant) {
	as_bad(_("offset is not a constant expression"));
	ignore_rest_of_line();
	return;
      }
      offset = arg.X_add_number;
      if (offset < -16384 || offset > 16383) {
	as_bad(_("offset is out of range: %d\n"), offset);
	ignore_rest_of_line();
	return;
      }

      md_number_to_chars(p, iword, 4);
      fix_new_exp(frag_now,
		  (p - frag_now->fr_literal),
		  4,
		  &arg,
		  0,
		  BFD_RELOC_BEXKAT1_15);
    }
    break;
  case BEXKAT1_LDI:
    regnum = parse_regnum(&op_end);
    if (regnum == -1)
      return; 
    while (ISSPACE(*op_end))
      op_end++;
    iword |= (regnum & 0xf) << 20; // A
    
    if (*op_end != ',') {
      as_bad(_("missing comma: %s"), op_end);
      return;
    }
    op_end++;
    while (ISSPACE(*op_end))
      op_end++;
    op_end = parse_exp_save_ilp(op_end, &arg);
    if (opcode->size) {
      md_number_to_chars(p, iword, 4);
      p = frag_more(4);
      fix_new_exp(frag_now,
		  (p - frag_now->fr_literal),
		  4,
		  &arg,
		  0,
		  BFD_RELOC_32);
    } else {
      if (arg.X_op != O_constant) {
	as_bad(_("offset is not a constant expression"));
	ignore_rest_of_line();
	return;
      }
      offset = arg.X_add_number;
      if (offset >= 32768) {
	as_bad(_("offset is out of range: %d\n"), offset);
	ignore_rest_of_line();
	return;
      }

      md_number_to_chars(p, iword, 4);
      fix_new_exp(frag_now,
		  (p - frag_now->fr_literal),
		  4,
		  &arg,
		  0,
		  BFD_RELOC_BEXKAT1_15);
    }
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

const char *
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
    __attribute__((fallthrough));
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
md_parse_option(int c, const char *arg ATTRIBUTE_UNUSED)
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
md_apply_fix(fixS *fixP, valueT *valP, segT segment)
{
  char *buf  = fixP->fx_where + fixP->fx_frag->fr_literal;
  /* Note: use offsetT because it is signed, valueT is unsigned.  */
  offsetT val  = (offsetT) * valP;
  segT symsec
    = (fixP->fx_addsy == NULL
       ? absolute_section : S_GET_SEGMENT (fixP->fx_addsy));

  /* If the fix is relative to a symbol which is not defined, or, (if
     pcrel), not in the same segment as the fix, we cannot resolve it
     here.  */
  if (fixP->fx_addsy != NULL
      && (! S_IS_DEFINED (fixP->fx_addsy)
	  || S_IS_WEAK (fixP->fx_addsy)
	  || (fixP->fx_pcrel && symsec != segment)
	  || (! fixP->fx_pcrel
	      && symsec != absolute_section
	      && symsec != reg_section)))
    {
      fixP->fx_done = 0;
      return;
    }
  else if (fixP->fx_r_type == BFD_RELOC_VTABLE_INHERIT
	   || fixP->fx_r_type == BFD_RELOC_VTABLE_ENTRY)
    {
      /* These are never "fixed".  */
      fixP->fx_done = 0;
      return;
    }
  else
    /* We assume every other relocation is "fixed".  */
    fixP->fx_done = 1;

  switch (fixP->fx_r_type) {
  case BFD_RELOC_BEXKAT1_15:
    if (!val)
      break;
    val = md_chars_to_number(buf, fixP->fx_size)
      | ((val & 0x7fff) << 1);
    md_number_to_chars(buf, val, fixP->fx_size);
    break;
  case BFD_RELOC_BEXKAT1_15_PCREL:
    if (!val)
      break;
    if ((val % 4) != 0)
      {
	as_warn(_("alignment failure for reloc: %ld"),
		val);
	fixP->fx_done = 0;
	val = 0;
      }
    if (val < -65536 || val > 65535)
      {
	as_warn_where(fixP->fx_file, fixP->fx_line,
		      _("operand out of 15 bit range for reloc"));
	fixP->fx_done = 0;
	val = 0;
      }
    val = md_chars_to_number(buf, fixP->fx_size)
      | (((val >> 2) & 0x7fff) << 1);
    md_number_to_chars(buf, val, fixP->fx_size);
    break;
  case BFD_RELOC_32:
    md_number_to_chars(buf, val, fixP->fx_size);
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

static valueT
md_chars_to_number (char * buf, int n)
{
  valueT result = 0;
  unsigned char * where = (unsigned char *) buf;

  if (target_big_endian)
    {
      while (n--)
	{
	  result <<= 8;
	  result |= (*where++ & 255);
	}
    }
  else
    {
      while (n--)
	{
	  result <<= 8;
	  result |= (where[n] & 255);
	}
    }

  return result;
}

long
md_pcrel_from(fixS *fixP)
{
  valueT addr = fixP->fx_where + fixP->fx_frag->fr_address;

  switch (fixP->fx_r_type) {
    case BFD_RELOC_BEXKAT1_15_PCREL:
      return addr + 4;
    default:
      abort();
  }

  return addr;
}

arelent *
tc_gen_reloc(asection *section ATTRIBUTE_UNUSED, fixS *fixP)
{
  arelent *relP;
  bfd_reloc_code_real_type code = BFD_RELOC_NONE;
  symbolS *addsy = fixP->fx_addsy;
  asection *addsec = addsy == NULL ? NULL : S_GET_SEGMENT (addsy);

  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_BEXKAT1_15:
    case BFD_RELOC_32:
      code = fixP->fx_r_type;
      if (addsy == NULL || bfd_is_abs_section (addsec))
	{
	  /* Resolve this reloc now, as md_apply_fix would have done (not
	     called if -linkrelax).  There is no point in keeping a reloc
	     to an absolute symbol.  No reloc that is subject to
	     relaxation must be to an absolute symbol; difference
	     involving symbols in a specific section must be signalled as
	     an error if the relaxing cannot be expressed; having a reloc
	     to the resolved (now absolute) value does not help.  */
	  char *buf  = fixP->fx_where + fixP->fx_frag->fr_literal;
	  md_number_to_chars (buf, fixP->fx_offset, fixP->fx_size);
	  return NULL;
	}
      break;

    case BFD_RELOC_BEXKAT1_15_PCREL:
      code = fixP->fx_r_type;
      break;
    default:
      as_bad_where
	(fixP->fx_file, fixP->fx_line,
	 _("operands were not reducible at assembly-time"));
      fixP->fx_addsy = NULL;
      return NULL;
    }

  relP = (arelent *) xmalloc(sizeof(arelent));
  gas_assert(relP != 0);
  relP->sym_ptr_ptr = (asymbol **) xmalloc (sizeof (asymbol *));
  *relP->sym_ptr_ptr = addsy != NULL ? symbol_get_bfdsym (addsy) : NULL;
  relP->address = fixP->fx_frag->fr_address + fixP->fx_where;

  relP->addend = fixP->fx_offset; 

  relP->howto = bfd_reloc_type_lookup(stdoutput, code);

  if (relP->howto == NULL)
    {
      const char *name;
      
      name = S_GET_NAME (fixP->fx_addsy);
      if (name == NULL)
	name = _("<unknown>");
      as_fatal (_("cannot generate relocation type for symbol %s, code %s"),
		name, bfd_get_reloc_code_name (code));
    }

  return relP;
}
