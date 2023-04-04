# MACRO: exit
  .macro exit nr
  halt
  .endm

# MACRO: pass
# Write 'pass' to stdio and quit
  .macro pass
  ldiu %0, 'p'
  trap 0
  ldiu %0, 'a'
  trap 0
  ldiu %0, 's'
  trap 0
  ldiu %0, 's'
  trap 0
  halt
  .endm

# MACRO: fail
# Write 'fail' to stdio and quit
  .macro fail
  ldiu %0, 'f'
  trap 0
  ldiu %0, 'a'
  trap 0
  ldiu %0, 'i'
  trap 0
  ldiu %0, 'l'
  trap 0
  halt
  .endm

# MACRO: start
# All assembler tests should start with a call to "start"
  .macro start
  .text
.global _start
.start:
  .endm
