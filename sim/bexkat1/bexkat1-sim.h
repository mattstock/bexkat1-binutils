#ifndef BEXKAT1_SIM_H
#define BEXKAT1_SIM_H

#include "opcode/bexkat1.h"

#define BEXKAT1_SP 15

#define CCR_LTU 0x8
#define CCR_LT 0x4
#define CCR_EQ 0x2
#define CCR_INTR 0x1
struct bexkat1_sim_cpu {
  uint32_t regs[17];
  sim_cia pc;
  /* 0, ccr_ltu, crr_lt, crr_eq, interrupts */
  uint32_t control;
};

#define BEXKAT1_SIM_CPU(cpu) ((struct bexkat1_sim_cpu *) CPU_ARCH_DATA (cpu))

extern void step_once (SIM_CPU *);
extern void initialize_cpu (SIM_DESC, SIM_CPU *);

#define DEFAULT_MEM_SIZE  (1024 * 1024)

#endif
