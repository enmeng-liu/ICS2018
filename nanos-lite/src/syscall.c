#include "common.h"
#include "syscall.h"
#include "proc.h"

extern char _end;

//typedef size_t ssize_t;
//typedef size_t off_t;
int fs_open(const char* pathname, int flags, int mode);
ssize_t fs_read(int fd, void* buf, size_t len);
ssize_t fs_close(int fd);
ssize_t fs_lseek(int fd, off_t offset, int whence);
ssize_t fs_write(int fd, const void* buf, size_t len);

void naive_uload(PCB *pcb, const char *filename);

extern _Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;	//type 
	a[1] = c->GPR2;	//parameter1:status
	a[2] = c->GPR3;
	a[3] = c->GPR4;

  switch (a[0]) {
		case SYS_yield: _yield();
									 	c->GPRx = 0; 
										//Log("call sys_yield!");
										break;
		case SYS_exit:  //Log("call sys_exit!");
										//_halt(a[2]);
										naive_uload(NULL, "/bin/init");
									  break;
		case SYS_write: //Log("call sys_write!");
										c->GPRx = (ssize_t)fs_write((int)a[1], (const void*)a[2], (size_t)a[3]);
										break;
		case SYS_brk:		//Log("call sys_brk! _end = %d", &_end);
										//_heap.start = &_end;
										_heap.end = (void*)a[1];
										c->GPRx = 0;
										break;
		case SYS_read: 	//Log("call sys_read! len = %d", a[3]);
									 	c->GPRx = (ssize_t)fs_read((int)a[1], (void*)a[2], (size_t)a[3]);
										break;
		case SYS_open: 	//Log("call sys_open!");
									 	c->GPRx = (ssize_t)fs_open((const char*)a[1], (int)a[2], (int)a[3]);
									 	break;
		case SYS_close: //Log("call sys_close!");
										c->GPRx = (ssize_t)fs_close((int)a[1]);
									  break;
		case SYS_lseek: //Log("call sys_lseek!");
										c->GPRx = (off_t)fs_lseek((int)a[1], (off_t)a[2], (int)a[3]);
										break;	
		case SYS_execve: Log("call SYS_execve, name = %s", (char*)a[1]);
										 naive_uload(NULL, (char*)a[1]);
										 c->GPRx = 0;
										 break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return c;
}
