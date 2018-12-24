#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used));
static PCB pcb_boot;
PCB *current;

void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void *entry);
void context_uload(PCB *pcb, const char *filename);

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
	//Log("before load dummy");
	//naive_uload(NULL, "/bin/init");
	context_uload(&pcb[0], "/bin/hello");
	context_uload(&pcb[1], "/bin/init");
	context_uload(&pcb[2], "/bin/dummy");
	context_uload(&pcb[3], "/bin/pal");
	//Log("after load dummy");
	switch_boot_pcb();
}

_Context* schedule(_Context *prev) {
	//Log("call schedule!");
	current->cp = prev; //save the context pointer
	//Log("prev context saved!");
	//current = &pcb[3];  //always seletc dummy as the new process
	//Log("current->as.ptr=%p",current->as.ptr);
	//current = (current == &pcb[0] ? &pcb[3] : &pcb[0]);
	//Log("current context changed!");
	int palcnt = 0;
	if(current == &pcb[0]) {
		current = &pcb[3];
		palcnt = 0;
	}
	else {
		palcnt ++;
		if(palcnt == 10) current = &pcb[0];
	}
	return current->cp;	//then return the new context
}
