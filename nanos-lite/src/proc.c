#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used));
static PCB pcb_boot;
PCB *current;

void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void *entry);
void context_uload(PCB *pcb, const char *filename);

typedef size_t ssize_t;
int fs_open(const char *pathname, int flags, int mode);
size_t fs_filesz(int fd);
ssize_t fs_read(int fd ,void *buf, size_t len);

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
	context_uload(&pcb[1], "/bin/pal");
	context_uload(&pcb[2], "/bin/pal");
	context_uload(&pcb[3], "/bin/pal");
	Log("after uload");
	switch_boot_pcb();
}

PCB* get_fg_pcb(){
	Log("call get_fg_pcb");
	assert(0);
	int fd = fs_open("/dev/events", 0, 0);
  char buf[128];
	fs_read(fd, buf, fs_filesz(fd));
	Log("Receive events %s!", buf);
	assert(0);
	if(strlen(buf) == 0) return &pcb[1];
}


int palcnt = 0;
_Context* schedule(_Context *prev) {
	Log("call schedule!");
	current->cp = prev; //save the context pointer
	//Log("prev context saved!");
	//current = &pcb[3];  //always seletc dummy as the new process
	//Log("current->as.ptr=%p",current->as.ptr);
	//current = (current == &pcb[0] ? &pcb[3] : &pcb[0]);
	//Log("current context changed!");
	PCB* fg_pcb = get_fg_pcb();
	if(current == &pcb[0]) {
		current = fg_pcb;
		palcnt = 0;
	}
	else {
		palcnt ++;
		if(palcnt == 30) current = &pcb[0];
		else current = fg_pcb;
	}
	return current->cp;	//then return the new context
}
