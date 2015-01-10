#include "config.h"
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include "sysdep.h"
#include <sys/times.h>
#include <sys/param.h>
#include <netinet/in.h>	/* for byte ordering macros */
#include "bfd.h"
#include "gdb/callback.h"
#include "libiberty.h"
#include "gdb/remote-sim.h"

/* Run or resume simulated program */
void sim_resume(SIM_DESC sd, int step, int signal) {
}

/* Store bytes into the simulated program's memory */
int sim_write(SIM_DESC sd, SIM_ADDR addr, const unsigned char *buffer, int size) {
  return 1;
}

/* Fetch bytes of the simulated program's memory */
int sim_read (SIM_DESC sd, SIM_ADDR addr, unsigned char *buffer, int size) {
  return 1;
}

/* Store target-endian value in register */
int sim_store_register (SIM_DESC sd, int rn, unsigned char *memory, int length) {
}

/* Fetch register, storing target-endian value */
int sim_fetch_register (SIM_DESC sd, int rn, unsigned char *memory, int length) {
}

/* Return the reason why a program stopped */
void sim_stop_reason (SIM_DESC sd, enum sim_stop * reason,  int *sigrc) {
}

/* Async request to stop the simulation */
int sim_stop (SIM_DESC sd) {
}

/* Create an initialized simulator instance */
SIM_DESC sim_open (SIM_OPEN_KIND kind, host_callback *cb, struct bfd *abfd, char **argv) {
}

/* Destroy a simulator instance */
void sim_close (SIM_DESC sd, int quitting) {
}

/* Load a program into simulator memory */
SIM_RC sim_load (SIM_DESC sd, const char *prog, bfd *abfd, int from_tty) {
}

/* Prepare to run the simulated program */
SIM_RC sim_create_inferior (SIM_DESC sd, struct bfd *prog_bfd, char **argv, char **env) {
}

/* Print statistics from simulator */
void sim_info (SIM_DESC sd, int verbose) {
}

int sim_trace(SIM_DESC sd) {
  return 1;
}

/* Legacy stuff */
void sim_set_callbacks (host_callback *ptr) {
}

void sim_size(int s) {
}


