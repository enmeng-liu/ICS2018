#include <am.h>
#include <x86.h>
#include "klib.h"

#define YIELD 0x81
#define SYSCALL 0x80

static _Context* (*user_handler)(_Event, _Context*) = NULL;

void vectrap();
void vecnull();
void vecsys();

_Context* irq_handle(_Context *tf) {
  _Context *next = tf;
	/*printf("$eax = %d\n", tf->eax);
	printf("$ecx = %d\n", tf->ecx);
	printf("$edx = %d\n", tf->edx);
	printf("$ebx = %d\n", tf->ebx);
	printf("$esp = %d\n", tf->esp);
	printf("$ebp = %d\n", tf->ebp);
	printf("$esi = %d\n", tf->esi);
	printf("$edi = %d\n", tf->edi);
	*/
	//printf("#irq = %d\n", tf-> irq);
  if (user_handler) {
    _Event ev;
    switch (tf->irq) {
			case YIELD: ev.event = _EVENT_YIELD; break; 
			case SYSCALL: ev.event = _EVENT_SYSCALL; break;
      default: ev.event = _EVENT_ERROR; break;
    }

    next = user_handler(ev, tf);
    if (next == NULL) {
      next = tf;
    }
  }

  return next;
}

static GateDesc idt[NR_IRQ];	//NR_IRQ:IDT size



int _cte_init(_Context*(*handler)(_Event, _Context*)) {
  // initialize IDT
  for (unsigned int i = 0; i < NR_IRQ; i ++) {
    idt[i] = GATE(STS_TG32, KSEL(SEG_KCODE), vecnull, DPL_KERN);
  }

  // -------------------- system call --------------------------
  idt[0x81] = GATE(STS_TG32, KSEL(SEG_KCODE), vectrap, DPL_KERN);
	idt[0x80] = GATE(STS_TG32, KSEL(SEG_KCODE), vecsys, DPL_KERN);

  set_idt(idt, sizeof(idt));

  // register event handler
  user_handler = handler;

  return 0;
}

_Context *_kcontext(_Area stack, void (*entry)(void *), void *arg) {
	printf("Call kcontext!\n");
	_Context *cont = (_Context*)(stack.end - sizeof(_Context));
	cont->cs = 8;
	cont->eip = (uintptr_t)(entry);
	cont->ebp = (int)(stack.start + sizeof(_Context*));
	cont->edi = cont->esi = cont->esp = cont->ebx = cont->ecx = cont->eax = 0;
	cont->irq = cont->err = cont->eflags = 0;
	//how to initialize cont?
	printf("kcontext return!\n");
  return cont;
}

void _yield() {
	//printf("call _yield!\n");
  asm volatile("int $0x81");
}

int _intr_read() {
  return 0;
}

void _intr_write(int enable) {
}
