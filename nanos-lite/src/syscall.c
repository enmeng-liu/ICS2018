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
		case SYS_exit: _halt(a[2]);
									 Log("call sys_exit!");
									 break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return c;
}
