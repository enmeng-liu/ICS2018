#include "common.h"
#include "syscall.h"

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
		case SYS_write: Log("call sys_write!");
										int fd = (int)a[1];
										char* buf = (char*)a[2];
										size_t count = (size_t)a[3];
										if(fd == 1 || fd == 2){
											for(int i = 0; i < count; ++i) _putc(buf[i]);
										}
										c->GPRx = count;
										break;

    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return c;
}
