#include "common.h"

_Context* do_syscall(_Context *c);
_Context* schedule(_Context *prev);

static _Context* do_event(_Event e, _Context* c) {
	_Context* ret = NULL;
  switch (e.event) {
		case _EVENT_YIELD: 
			Log("ID = %d: Recognize yield event!",e.event); break;
			ret = schedule(c);
			break;
		case _EVENT_SYSCALL: 
			//Log("ID = %d: Recognize system call!",e.event);
			do_syscall(c);
			break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return ret;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
