#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used));
static PCB pcb_boot;
PCB *current;

void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void *entry);
void context_uload(PCB *pcb, const char *filename);

//typedef size_t ssize_t;
//int fs_open(const char *pathname, int flags, int mode);
//size_t fs_filesz(int fd);
//ssize_t fs_read(int fd ,void *buf, size_t len);
size_t events_read(void* buf, size_t offset, size_t len);

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
	Log("before uload");
	//naive_uload(NULL, "/bin/init");
	context_uload(&pcb[0], "/bin/hello");
	context_uload(&pcb[1], "/bin/bmptest");
	context_uload(&pcb[2], "/bin/pal");
	context_uload(&pcb[3], "/bin/pal");
	Log("after uload");
	switch_boot_pcb();
}

/*PCB* get_fg_pcb(){
  char buf[128];
	events_read(buf, 0, 128);
	if(strcmp(buf, "kd F1\n") == 0) return &pcb[1];
	if(strcmp(buf, "kd F2\n") == 0) return &pcb[2];
	if(strcmp(buf, "kd F3\n") == 0) return &pcb[3];
	return &pcb[1];
}*/


int palcnt = 0;
int fg_pcb;
int first_flag = 1;
_Context* schedule(_Context *prev) {
	//Log("call schedule!");
	current->cp = prev; //save the context pointer
	//Log("prev context saved!");
	//current = &pcb[3];  //always seletc dummy as the new process
	//Log("current->as.ptr=%p",current->as.ptr);
	char buf[128];
	events_read(buf, 0, 128);
	if(first_flag == 1) {
		current = &pcb[0];
		first_flag = 0;
		fg_pcb = 1;
	}
	else 
		current = (current == &pcb[0] ? &pcb[fg_pcb] : &pcb[0]);
	//Log("current context changed!");
	//PCB* pre_cur = &pcb[1];
	
	/*if(current == &pcb[0]) {
		palcnt = 0;
		current = &pcb[fg_pcb];
	}
	else {
		palcnt ++;
		if(palcnt == 100) current = &pcb[0];
		else  current = &pcb[fg_pcb];
	}*/
	return current->cp;	//then return the new context
}
