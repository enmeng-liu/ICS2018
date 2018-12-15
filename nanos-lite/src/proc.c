#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used));
static PCB pcb_boot;
PCB *current;

void naive_uload(PCB *pcb, const char *filename);

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

void init_proc() {
	naive_uload(NULL, "/bin/init");
	switch_boot_pcb();
}

_Context* schedule(_Context *prev) {
	current->cp = prev; //save the context pointer
	current = &pcb[0];  //always seletc pcb[0] as the new process
	return current->cp;	//then return the new context
}
