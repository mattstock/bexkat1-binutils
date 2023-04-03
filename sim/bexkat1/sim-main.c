#include "defs.h"

#include "sim-main.h"
#include "sim-signal.h"

#include "bexkat1-sim.h"
#include "opcode/bexkat1.h"

static char *regnames[] = { "%0", "%1", "%2", "%3",
  "%4", "%5", "%6", "%7",
  "%8", "%9", "%10", "%11",
  "%12", "%13", "%fp", "%sp" };

static const bexkat1_opc_info_t *
find_opcode(uint32_t w) {
  uint8_t type = (w >> 28) & 0xf;
  uint8_t opcode = (w >> 24) & 0xf;
  uint8_t s = w & 0x1;

  int i;
  for (i=0; i < bexkat1_opc_count; i++)
    if (bexkat1_opc_info[i].type == type &&
	bexkat1_opc_info[i].opcode == opcode &&
        bexkat1_opc_info[i].size == s)
      return &bexkat1_opc_info[i];
  return NULL;
}

static uint32_t get_offset(unsigned int w) {
  uint32_t offset = ((w >> 1) & 0x7fff);
  if (offset & 0x00004000)
    offset |=  0xffff8000;
  return offset;
}

void
step_once (SIM_CPU *cpu)
{
  SIM_DESC sd = CPU_STATE (cpu);
  struct bexkat1_sim_cpu *bexkat1_cpu = BEXKAT1_SIM_CPU (cpu);
  uint32_t iw, imm32;
  const bexkat1_opc_info_t *opcode;
  uint8_t rA, rB, rC;
  int offset;
  sim_cia pc = sim_pc_get (cpu);
  char c;

  iw = sim_core_read_aligned_4 (cpu, pc, exec_map, pc);
  TRACE_EXTRACT (cpu, "%04x: ins: %#x", pc, iw);
  pc += 4;
  opcode = find_opcode(iw);
  if (opcode == NULL) {
    /* not good */
  }
    
  /* for RELOC_BEXLAT1_15 opcodes */
  offset = get_offset(iw);
  /* for register-based opcodes */
  rA = (iw >> 20) & 0xf;
  rB = (iw >> 16) & 0xf;
  rC = (iw >> 12) & 0xf;

  switch (opcode->type) {
  case BEXKAT1_INH:
    switch (opcode->opcode) {
    case 0: /* nop */
      TRACE_INSN (cpu, "%s", opcode->name);
      break;
    case 1:
      if (opcode->size) { /* setint */
	imm32 = sim_core_read_aligned_4 (cpu, pc, exec_map, pc);
	pc += 4;
	TRACE_INSN (cpu, "%s %08x", opcode->name, imm32);
      } else { /* trap */
	TRACE_INSN (cpu, "%s %d", opcode->name, offset);
	switch (offset) {
	case 0: /* print character in %0 */
	  c = bexkat1_cpu->regs[0];
	  TRACE_EVENTS (cpu, "write to stdin: %#x (%c)", c, c);
	  sim_io_printf (sd, "%c", c);
	  break;
	case 1: /* read character to %0 */
	  sim_io_read_stdin (sd, &c, 1);
	  TRACE_EVENTS (cpu, "read from stdin: %#x (%c)", c, c);
	  bexkat1_cpu->regs[0] = c;
	  TRACE_REGISTER (cpu, "%s <= %08x",
			  regnames[0],
			  bexkat1_cpu->regs[0]);
	  break;
	}
      }
      break;
    case 2: /* cli */
      TRACE_INSN (cpu, "%s", opcode->name);
      bexkat1_cpu->control &= 0xe;
      TRACE_EVENTS (cpu, "disable interrupts");
      TRACE_REGISTER (cpu, "CC <= %02x",
		      bexkat1_cpu->control);
      break;
    case 3: /* sti */
      TRACE_INSN (cpu, "%s", opcode->name);
      bexkat1_cpu->control |= 0x1;
      TRACE_EVENTS (cpu, "enable interrupts");
      TRACE_REGISTER (cpu, "CC <= %02x",
		      bexkat1_cpu->control);
      break;
    case 4: /* halt */
      TRACE_INSN (cpu, "%s", opcode->name);
      sim_engine_halt (sd, cpu, NULL, pc, sim_exited, 0);
      break;
    case 5: /* reset */
      TRACE_INSN (cpu, "%s", opcode->name);
      sim_engine_halt (sd, cpu, NULL, pc, sim_exited, 0);
      break;
    }
    break;
  case BEXKAT1_PUSH:
    switch (opcode->opcode) {
    case 0: /* push */
      sim_core_write_aligned_4 (cpu, pc, write_map,
				bexkat1_cpu->regs[BEXKAT1_SP],
				bexkat1_cpu->regs[rA]);
      bexkat1_cpu->regs[BEXKAT1_SP] -= 4;
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[BEXKAT1_SP],
		      bexkat1_cpu->regs[BEXKAT1_SP]);
      TRACE_INSN (cpu, "%s %s", opcode->name, regnames[rA]);
      break;
    case 1:
      if (opcode->size) { /* jsrd */
	imm32 = sim_core_read_aligned_4 (cpu, pc, exec_map, pc);
	pc += 4;
	sim_core_write_aligned_4 (cpu, pc, write_map,
				  bexkat1_cpu->regs[BEXKAT1_SP],
				  pc);
	bexkat1_cpu->regs[BEXKAT1_SP] -= 4;
	TRACE_REGISTER (cpu, "%s <= %08x",
			regnames[BEXKAT1_SP],
			bexkat1_cpu->regs[BEXKAT1_SP]);
	TRACE_INSN (cpu, "%s %08x", opcode->name, imm32);
	pc = imm32;
      } else { /* jsr */
	sim_core_write_aligned_4 (cpu, pc, write_map,
				  bexkat1_cpu->regs[BEXKAT1_SP],
				  pc);
	bexkat1_cpu->regs[BEXKAT1_SP] -= 4;
	TRACE_REGISTER (cpu, "%s <= %08x",
			regnames[BEXKAT1_SP],
			bexkat1_cpu->regs[BEXKAT1_SP]);
	TRACE_INSN (cpu, "%s %d", opcode->name, offset);
	pc =+ offset;
      }
      break;
    case 2: /* bsr */
      break;
    case 3: /* pushcc */
      sim_core_write_aligned_4 (cpu, pc, write_map,
				bexkat1_cpu->regs[BEXKAT1_SP],
				bexkat1_cpu->control);
      bexkat1_cpu->regs[BEXKAT1_SP] -= 4;
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[BEXKAT1_SP],
		      bexkat1_cpu->regs[BEXKAT1_SP]);
      TRACE_REGISTER (cpu, "CC <= %08x",
		      bexkat1_cpu->control);
      TRACE_INSN (cpu, "%s", opcode->name);
      break;
    }
    break;
  case BEXKAT1_POP:
    switch (opcode->opcode) {
    case 0: /* pop */
      bexkat1_cpu->regs[BEXKAT1_SP] += 4;
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[BEXKAT1_SP],
		      bexkat1_cpu->regs[BEXKAT1_SP]);
      bexkat1_cpu->regs[rA] =
	sim_core_read_aligned_4 (cpu, pc, read_map,
				 bexkat1_cpu->regs[BEXKAT1_SP]);
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[rA], bexkat1_cpu->regs[rA]);
      TRACE_INSN (cpu, "%s %s", opcode->name, regnames[rA]);
      break;
    case 1: /* rts */
      bexkat1_cpu->regs[BEXKAT1_SP] += 4;
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[BEXKAT1_SP],
		      bexkat1_cpu->regs[BEXKAT1_SP]);
      pc = sim_core_read_aligned_4 (cpu, pc, read_map,
				    bexkat1_cpu->regs[BEXKAT1_SP]);
      TRACE_INSN (cpu, "%s %d", opcode->name, offset);
      break;
    case 2: /* rti */
      bexkat1_cpu->regs[BEXKAT1_SP] += 4;
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[BEXKAT1_SP],
		      bexkat1_cpu->regs[BEXKAT1_SP]);
      pc = sim_core_read_aligned_4 (cpu, pc, read_map,
				    bexkat1_cpu->regs[BEXKAT1_SP]);
      TRACE_INSN (cpu, "%s %d", opcode->name, offset);
      break;
    case 3: /* popcc */
      bexkat1_cpu->regs[BEXKAT1_SP] += 4;
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[BEXKAT1_SP],
		      bexkat1_cpu->regs[BEXKAT1_SP]);
      bexkat1_cpu->control =
	sim_core_read_aligned_4 (cpu, pc, read_map,
				 bexkat1_cpu->regs[BEXKAT1_SP]);
      TRACE_REGISTER (cpu, "CC <= %08x", bexkat1_cpu->control);
      TRACE_INSN (cpu, "%s", opcode->name);
      break;
    }
    break;
  case BEXKAT1_CMP:
    {
      int va = bexkat1_cpu->regs[rA];
      int vb = bexkat1_cpu->regs[rB];
      int cc = (va == vb ? CCR_EQ : 0);
      cc |= (va < vb ? CCR_LT : 0);
      cc |= ((unsigned int) va < (unsigned int) vb ? CCR_LTU : 0);
      bexkat1_cpu->control = cc | (bexkat1_cpu->control & 0x1);
    }
    break;
  case BEXKAT1_MOV:
    switch (opcode->opcode) {
    case 0: /* movsr */
      break;
    case 1: /* mov.b */
      bexkat1_cpu->regs[rA] = bexkat1_cpu->regs[rB] & 0xff;
      TRACE_REGISTER (cpu, "%s <= %08x (%s)",
		      regnames[rA],
		      bexkat1_cpu->regs[rA],
		      regnames[rB]);
      TRACE_INSN (cpu, "%s %s, %s", opcode->name, regnames[rA], regnames[rB]);
      break;
    case 2: /* mov */
      bexkat1_cpu->regs[rA] = bexkat1_cpu->regs[rB] & 0xffff;
      TRACE_REGISTER (cpu, "%s <= %08x (%s)",
		      regnames[rA],
		      bexkat1_cpu->regs[rA],
		      regnames[rB]);
      TRACE_INSN (cpu, "%s %s, %s", opcode->name, regnames[rA], regnames[rB]);
      break;
    case 3: /* mov.l */
      bexkat1_cpu->regs[rA] = bexkat1_cpu->regs[rB];
      TRACE_REGISTER (cpu, "%s <= %08x (%s)",
		      regnames[rA],
		      bexkat1_cpu->regs[rA],
		      regnames[rB]);
      TRACE_INSN (cpu, "%s %s, %s", opcode->name, regnames[rA], regnames[rB]);
      break;
    case 4: /* movrs */
      break;
    }
    break;
  case BEXKAT1_INTU:
    switch (opcode->opcode) {
    case 8: /* ext */
      break;
    case 9: /* ext.b */
      break;
    case 10: /* com */
      break;
    case 11: /* neg */
      break;
    }
    break;
  case BEXKAT1_FPU:
  case BEXKAT1_FP:
    /* treat all of these as NOP */
    break;
  case BEXKAT1_ALU:
    switch (opcode->opcode) {
    case 0: /* and */
      bexkat1_cpu->regs[rA] =
	bexkat1_cpu->regs[rB] &
	bexkat1_cpu->regs[rC];
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[rA],
		      bexkat1_cpu->regs[rA]);
      TRACE_INSN (cpu, "%s %s, %s, %s", opcode->name, regnames[rA],
		  regnames[rB], regnames[rC]);
      break;
    case 1: /* or */
      bexkat1_cpu->regs[rA] =
	bexkat1_cpu->regs[rB] |
	bexkat1_cpu->regs[rC];
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[rA],
		      bexkat1_cpu->regs[rA]);
      TRACE_INSN (cpu, "%s %s, %s, %s", opcode->name, regnames[rA],
		  regnames[rB], regnames[rC]);
      break;
    case 2: /* add */
      bexkat1_cpu->regs[rA] =
	bexkat1_cpu->regs[rB] +
	bexkat1_cpu->regs[rC];
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[rA],
		      bexkat1_cpu->regs[rA]);
      TRACE_INSN (cpu, "%s %s, %s, %s", opcode->name, regnames[rA],
		  regnames[rB], regnames[rC]);
      break;
    case 3: /* sub */
      bexkat1_cpu->regs[rA] =
	bexkat1_cpu->regs[rB] -
	bexkat1_cpu->regs[rC];
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[rA],
		      bexkat1_cpu->regs[rA]);
      TRACE_INSN (cpu, "%s %s, %s, %s", opcode->name, regnames[rA],
		  regnames[rB], regnames[rC]);
      break;
    case 4: /* lsl */
      bexkat1_cpu->regs[rA] =
	bexkat1_cpu->regs[rB] <<
	bexkat1_cpu->regs[rC];
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[rA],
		      bexkat1_cpu->regs[rA]);
      TRACE_INSN (cpu, "%s %s, %s, %s", opcode->name, regnames[rA],
		  regnames[rB], regnames[rC]);
      break;
    case 5: /* asr */
      bexkat1_cpu->regs[rA] =
	(int)bexkat1_cpu->regs[rB] >>
	(int)bexkat1_cpu->regs[rC];
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[rA],
		      bexkat1_cpu->regs[rA]);
      TRACE_INSN (cpu, "%s %s, %s, %s", opcode->name, regnames[rA],
		  regnames[rB], regnames[rC]);
      break;
    case 6: /* lsr */
      bexkat1_cpu->regs[rA] =
	bexkat1_cpu->regs[rB] >>
	bexkat1_cpu->regs[rC];
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[rA],
		      bexkat1_cpu->regs[rA]);
      TRACE_INSN (cpu, "%s %s, %s, %s", opcode->name, regnames[rA],
		  regnames[rB], regnames[rC]);
      break;
    case 7: /* xor */
      bexkat1_cpu->regs[rA] =
	bexkat1_cpu->regs[rB] ^
	bexkat1_cpu->regs[rC];
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[rA],
		      bexkat1_cpu->regs[rA]);
      TRACE_INSN (cpu, "%s %s, %s, %s", opcode->name, regnames[rA],
		  regnames[rB], regnames[rC]);
      break;
    case 8: /* andi */
      imm32 = sim_core_read_aligned_4 (cpu, pc, exec_map, pc);
      pc += 4;
      bexkat1_cpu->regs[rA] =
	bexkat1_cpu->regs[rB] &
	offset;
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[rA],
		      bexkat1_cpu->regs[rA]);
      TRACE_INSN (cpu, "%s %s, %s, %d", opcode->name, regnames[rA],
		  regnames[rB], offset);
      break;
    case 9: /* ori */
      imm32 = sim_core_read_aligned_4 (cpu, pc, exec_map, pc);
      pc += 4;
      bexkat1_cpu->regs[rA] =
	bexkat1_cpu->regs[rB] |
	offset;
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[rA],
		      bexkat1_cpu->regs[rA]);
      TRACE_INSN (cpu, "%s %s, %s, %d", opcode->name, regnames[rA],
		  regnames[rB], offset);
      break;
    case 10: /* addi */
      imm32 = sim_core_read_aligned_4 (cpu, pc, exec_map, pc);
      pc += 4;
      bexkat1_cpu->regs[rA] =
	bexkat1_cpu->regs[rB] +
	offset;
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[rA],
		      bexkat1_cpu->regs[rA]);
      TRACE_INSN (cpu, "%s %s, %s, %d", opcode->name, regnames[rA],
		  regnames[rB], offset);
      break;
    case 11: /* subi */
      imm32 = sim_core_read_aligned_4 (cpu, pc, exec_map, pc);
      pc += 4;
      bexkat1_cpu->regs[rA] =
	bexkat1_cpu->regs[rB] -
	offset;
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[rA],
		      bexkat1_cpu->regs[rA]);
      TRACE_INSN (cpu, "%s %s, %s, %d", opcode->name, regnames[rA],
		  regnames[rB], offset);
      break;
    case 12: /* lsli */
      imm32 = sim_core_read_aligned_4 (cpu, pc, exec_map, pc);
      pc += 4;
      bexkat1_cpu->regs[rA] =
	bexkat1_cpu->regs[rB] <<
	offset;
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[rA],
		      bexkat1_cpu->regs[rA]);
      TRACE_INSN (cpu, "%s %s, %s, %d", opcode->name, regnames[rA],
		  regnames[rB], offset);
      break;
    case 13: /* asri */
      imm32 = sim_core_read_aligned_4 (cpu, pc, exec_map, pc);
      pc += 4;
      bexkat1_cpu->regs[rA] =
	(int) bexkat1_cpu->regs[rB] >>
	(int) offset;
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[rA],
		      bexkat1_cpu->regs[rA]);
      TRACE_INSN (cpu, "%s %s, %s, %d", opcode->name, regnames[rA],
		  regnames[rB], offset);
      break;
    case 14: /* lsri */
      imm32 = sim_core_read_aligned_4 (cpu, pc, exec_map, pc);
      pc += 4;
      bexkat1_cpu->regs[rA] =
	bexkat1_cpu->regs[rB] >>
	offset;
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[rA],
		      bexkat1_cpu->regs[rA]);
      TRACE_INSN (cpu, "%s %s, %s, %d", opcode->name, regnames[rA],
		  regnames[rB], offset);
      break;
    case 15: /* xori */
      imm32 = sim_core_read_aligned_4 (cpu, pc, exec_map, pc);
      pc += 4;
      bexkat1_cpu->regs[rA] =
	bexkat1_cpu->regs[rB] ^
	offset;
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[rA],
		      bexkat1_cpu->regs[rA]);
      TRACE_INSN (cpu, "%s %s, %s, %d", opcode->name, regnames[rA],
		  regnames[rB], offset);
      break;
    }
    break;
  case BEXKAT1_INT:
  case BEXKAT1_STORE:
  case BEXKAT1_LOAD:
    break;
  case BEXKAT1_BRANCH:
    switch (opcode->opcode) {
    case 0: /* bra */
      pc =+ offset;
      break;
    case 1: /* beq */
      if ((bexkat1_cpu->control & CCR_EQ) == CCR_EQ) {
	pc =+ offset;
      }
      break;
    case 2: /* bne */
      if ((bexkat1_cpu->control & CCR_EQ) != CCR_EQ) {
	pc =+ offset;
      }
      break;
    case 3: /* bgtu */
      if ((bexkat1_cpu->control & (CCR_LTU | CCR_EQ)) != (CCR_LTU|CCR_EQ)) {
	pc =+ offset;
      }
      break;
    case 4: /* bgt */
      if ((bexkat1_cpu->control & (CCR_LTU | CCR_EQ)) == (CCR_LTU|CCR_EQ)) {
	pc =+ offset;
      }
      break;
    case 5: /* bge */
      if ((bexkat1_cpu->control & (CCR_LTU | CCR_EQ)) == (CCR_LTU|CCR_EQ)) {
	pc =+ offset;
      }
      break;
    case 6: /* ble */
      if ((bexkat1_cpu->control & (CCR_LTU | CCR_EQ)) == (CCR_LTU|CCR_EQ)) {
	pc =+ offset;
      }
      break;
    case 7: /* blt */
      if ((bexkat1_cpu->control & (CCR_LTU | CCR_EQ)) == (CCR_LTU|CCR_EQ)) {
	pc =+ offset;
      }
      break;
    case 8: /* bgeu */
      if ((bexkat1_cpu->control & (CCR_LTU | CCR_EQ)) == (CCR_LTU|CCR_EQ)) {
	pc =+ offset;
      }
      break;
    case 9: /* bltu */
      if ((bexkat1_cpu->control & (CCR_LTU | CCR_EQ)) == (CCR_LTU|CCR_EQ)) {
	pc =+ offset;
      }
      break;
    case 10: /* bleu */
      if ((bexkat1_cpu->control & (CCR_LTU | CCR_EQ)) == (CCR_LTU|CCR_EQ)) {
	pc =+ offset;
      }
      break;
    default: /* brn */
      break;
    }
    TRACE_INSN (cpu, "%s %d", opcode->name, offset);
    break;
  case BEXKAT1_JUMP:
    if (opcode->size) { /* jmp */
      imm32 = sim_core_read_aligned_4 (cpu, pc, exec_map, pc);
      TRACE_INSN (cpu, "%s %08x", opcode->name, imm32);
      pc = imm32;
    } else { /* jmpd */
      TRACE_INSN (cpu, "%s %d", opcode->name, offset);
      pc =+ offset;
    }
    break;
  case BEXKAT1_LDI:
    if (opcode->size) { /* ldi */
      imm32 = sim_core_read_aligned_4 (cpu, pc, exec_map, pc);
      pc += 4;
      bexkat1_cpu->regs[rA] = imm32;
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[rA],
		      bexkat1_cpu->regs[rA]);
      TRACE_INSN (cpu, "%s %s, %d", opcode->name, regnames[rA], imm32);
    } else { /* ldiu */
      bexkat1_cpu->regs[rA] = offset;
      TRACE_REGISTER (cpu, "%s <= %08x",
		      regnames[rA],
		      bexkat1_cpu->regs[rA]);
      TRACE_INSN (cpu, "%s %s, %d", opcode->name, regnames[rA], offset);
    }
    break;
  }
  
  TRACE_REGISTER (cpu, "PC <= %#x", pc);
  sim_pc_set (cpu, pc);
}

/* Return the program counter for this cpu. */
static sim_cia
pc_get (sim_cpu *cpu)
{
  struct bexkat1_sim_cpu *bexkat1_cpu = BEXKAT1_SIM_CPU (cpu);

  return bexkat1_cpu->pc;
}

/* Set the program counter for this cpu to the new pc value. */
static void
pc_set (sim_cpu *cpu, sim_cia pc)
{
  struct bexkat1_sim_cpu *bexkat1_cpu = BEXKAT1_SIM_CPU (cpu);

  bexkat1_cpu->pc = pc;
}

static int
reg_fetch (sim_cpu *cpu, int rn, void *buf, int len)
{
  struct bexkat1_sim_cpu *bexkat1_cpu = BEXKAT1_SIM_CPU (cpu);

  if (len <= 0 || len > sizeof (unsigned_word))
    return -1;

  memcpy (buf, &bexkat1_cpu->regs[rn], len);
  return len;
}

static int
reg_store (sim_cpu *cpu, int rn, const void *buf, int len)
{
  struct bexkat1_sim_cpu *bexkat1_cpu = BEXKAT1_SIM_CPU (cpu);

  if (len <= 0 || len > sizeof (unsigned_word))
    return -1;

  memcpy (&bexkat1_cpu->regs[rn], buf, len);
  return len;
}

/* Initialize the state for a single cpu.  Usually this involves clearing all
   registers back to their reset state.  Should also hook up the fetch/store
   helper functions too.  */
void initialize_cpu (SIM_DESC sd, SIM_CPU *cpu)
{
  struct bexkat1_sim_cpu *bexkat1_cpu = BEXKAT1_SIM_CPU (cpu);

  memset (bexkat1_cpu->regs, 0, sizeof (bexkat1_cpu->regs));
  bexkat1_cpu->pc = 0;

  CPU_PC_FETCH (cpu) = pc_get;
  CPU_PC_STORE (cpu) = pc_set;
  CPU_REG_FETCH (cpu) = reg_fetch;
  CPU_REG_STORE (cpu) = reg_store;
  
}
