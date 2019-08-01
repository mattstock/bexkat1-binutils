/* bexkat1-opc.c -- Bexkat 1 opcode list
   Written by Matt Stock (stock@bexkat.com)
 */

#include <stdio.h>
#include "ansidecl.h"
#include "opcode/bexkat1.h"

const bexkat1_opc_info_t bexkat1_opc_info[] =
  {
    /* type, opcode, size, args, name */
    { BEXKAT1_INH, 0, 0, 0, "nop" },
    { BEXKAT1_INH, 1, 0, 1, "trap" },
    { BEXKAT1_INH, 1, 1, 1, "setint" },
    { BEXKAT1_INH, 2, 0, 0, "cli" },
    { BEXKAT1_INH, 3, 0, 0, "sti" },
    { BEXKAT1_INH, 4, 0, 0, "halt" },
    { BEXKAT1_INH, 5, 0, 0, "reset" },
    { BEXKAT1_PUSH, 0, 0, 1, "push" },
    { BEXKAT1_PUSH, 1, 1, 1, "jsrd" },
    { BEXKAT1_PUSH, 1, 0, 2, "jsr" },
    { BEXKAT1_PUSH, 2, 0, 1, "bsr" },
    { BEXKAT1_POP, 0, 0, 1, "pop" },
    { BEXKAT1_POP, 1, 0, 0, "rts" },
    { BEXKAT1_POP, 2, 0, 0, "rti" },
    { BEXKAT1_CMP, 0, 0, 2, "cmp" },
    { BEXKAT1_CMP, 1, 0, 2, "cmp.s" },
    { BEXKAT1_MOV, 0, 0, 1, "movsr" },
    { BEXKAT1_MOV, 1, 0, 2, "mov.b" },
    { BEXKAT1_MOV, 2, 0, 2, "mov" },
    { BEXKAT1_MOV, 3, 0, 2, "mov.l" },
    { BEXKAT1_MOV, 4, 0, 1, "movrs" },
    { BEXKAT1_INTU, 8, 0, 2, "ext" },
    { BEXKAT1_INTU, 9, 0, 2, "ext.b" },
    { BEXKAT1_INTU, 10, 0, 2, "com" },
    { BEXKAT1_INTU, 11, 0, 2, "neg" },
    { BEXKAT1_FPU, 0, 0, 2, "cvtis" },
    { BEXKAT1_FPU, 1, 0, 2, "cvtsi" },
    { BEXKAT1_FPU, 2, 0, 2, "sqrt.s" },
    { BEXKAT1_FPU, 3, 0, 2, "neg.s" },
    { BEXKAT1_FP, 4, 0, 3, "add.s" },
    { BEXKAT1_FP, 5, 0, 3, "sub.s" },
    { BEXKAT1_FP, 6, 0, 3, "mul.s" },
    { BEXKAT1_FP, 7, 0, 3, "div.s" },
    { BEXKAT1_ALU, 0, 0, 3, "and" },
    { BEXKAT1_ALU, 1, 0, 3, "or" },
    { BEXKAT1_ALU, 2, 0, 3, "add" },
    { BEXKAT1_ALU, 3, 0, 3, "sub" },
    { BEXKAT1_ALU, 4, 0, 3, "lsl" },
    { BEXKAT1_ALU, 5, 0, 3, "asr" },
    { BEXKAT1_ALU, 6, 0, 3, "lsr" },
    { BEXKAT1_ALU, 7, 0, 3, "xor" },
    { BEXKAT1_ALU, 8, 0, 3, "andi" },
    { BEXKAT1_ALU, 9, 0, 3, "ori" },
    { BEXKAT1_ALU, 10, 0, 3, "addi" },
    { BEXKAT1_ALU, 11, 0, 3, "subi" },
    { BEXKAT1_ALU, 12, 0, 3, "lsli" },
    { BEXKAT1_ALU, 13, 0, 3, "asri" },
    { BEXKAT1_ALU, 14, 0, 3, "lsri" },
    { BEXKAT1_ALU, 15, 0, 3, "xori" },
    { BEXKAT1_INT, 0, 0, 3, "mul" },
    { BEXKAT1_INT, 1, 0, 3, "div" },
    { BEXKAT1_INT, 2, 0, 3, "mod" },
    { BEXKAT1_INT, 3, 0, 3, "mulu" },
    { BEXKAT1_INT, 4, 0, 3, "divu" },
    { BEXKAT1_INT, 5, 0, 3, "modu" },
    { BEXKAT1_INT, 6, 0, 3, "mul.x" },
    { BEXKAT1_INT, 7, 0, 3, "mulu.x" },
    { BEXKAT1_INT, 8, 0, 3, "muli" },
    { BEXKAT1_INT, 9, 0, 3, "divi" },
    { BEXKAT1_INT, 10, 0, 3, "modi" },
    { BEXKAT1_INT, 11, 0, 3, "mului" },
    { BEXKAT1_INT, 12, 0, 3, "divui" },
    { BEXKAT1_INT, 13, 0, 3, "modui" },
    { BEXKAT1_INT, 14, 0, 3, "muli.x" },
    { BEXKAT1_INT, 15, 0, 3, "mului.x" },
    { BEXKAT1_STORE, 0, 1, 2, "std.l" },
    { BEXKAT1_STORE, 1, 1, 2, "std" },
    { BEXKAT1_STORE, 2, 1, 2, "std.b" },
    { BEXKAT1_STORE, 0, 0, 3, "st.l" },
    { BEXKAT1_STORE, 1, 0, 3, "st" },
    { BEXKAT1_STORE, 2, 0, 3, "st.b" },
    { BEXKAT1_LOAD, 0, 1, 2, "ldd.l" },
    { BEXKAT1_LOAD, 1, 1, 2, "ldd" },
    { BEXKAT1_LOAD, 2, 1, 2, "ldd.b" },
    { BEXKAT1_LOAD, 0, 0, 3, "ld.l" },
    { BEXKAT1_LOAD, 1, 0, 3, "ld" },
    { BEXKAT1_LOAD, 2, 0, 3, "ld.b" },
    { BEXKAT1_BRANCH, 0, 0, 1, "bra" },
    { BEXKAT1_BRANCH, 1, 0, 1, "beq" },
    { BEXKAT1_BRANCH, 2, 0, 1, "bne" },
    { BEXKAT1_BRANCH, 3, 0, 1, "bgtu" },
    { BEXKAT1_BRANCH, 4, 0, 1, "bgt" },
    { BEXKAT1_BRANCH, 5, 0, 1, "bge" },
    { BEXKAT1_BRANCH, 6, 0, 1, "ble" },
    { BEXKAT1_BRANCH, 7, 0, 1, "blt" },
    { BEXKAT1_BRANCH, 8, 0, 1, "bgeu" },
    { BEXKAT1_BRANCH, 9, 0, 1, "bltu" },
    { BEXKAT1_BRANCH, 10, 0, 1, "bleu" },
    { BEXKAT1_BRANCH, 11, 0, 1, "brn" },
    { BEXKAT1_BRANCH, 12, 0, 1, "bord" },
    { BEXKAT1_BRANCH, 13, 0, 1, "bunord" },
    { BEXKAT1_BRANCH, 14, 0, 1, "bunle" },
    { BEXKAT1_BRANCH, 15, 0, 1, "bungt" },    
    { BEXKAT1_JUMP, 0, 1, 1, "jmpd" },
    { BEXKAT1_JUMP, 0, 0, 2, "jmp" },
    { BEXKAT1_LDI, 0, 0, 2, "ldiu" },
    { BEXKAT1_LDI, 0, 1, 2, "ldi" }
  };

const int bexkat1_opc_count = (sizeof(bexkat1_opc_info)/sizeof(bexkat1_opc_info[0]));
