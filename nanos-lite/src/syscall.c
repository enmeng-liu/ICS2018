#include "common.h"
#include "syscall.h"

extern char _end;

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
		case SYS_exit:  Log("call sys_exit!");
										_halt(a[2]);
									  break;
		case SYS_write: //Log("call sys_write!");
										//int fd = (int)a[1];
										//char* buf = (char*)a[2];
										//size_t count = (size_t)a[3];
										if(a[1] == 1 || a[1] == 2){
											char* buf = (char*)a[2];
											for(int i = 0; i < a[3]; ++i) _putc(buf[i]);
										}
										c->GPRx = (size_t)a[3];
										break;
		case SYS_brk:		Log("call sys_brk!");
										_heap.start = &_end;
										_heap.end = (void*)a[1];
										c->GPRx = 0;
										break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return c;
}
