# MACRO: exit
  .macro exit nr
  halt
  .endm

# MACRO: pass
# Write 'pass' to stdio and quit
  .macro pass
  .endm

# MACRO: fail
# Write 'fail' to stdio and quit
  .macro fail
  .endm

# MACRO: start
# All assembler tests should start with a call to "start"
  .macro start
  .text
.global _start
.start:
  .endm
