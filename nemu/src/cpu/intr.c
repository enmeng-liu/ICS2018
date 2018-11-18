#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  //TODO();
	rtl_push(&cpu.eflag);
	rtl_push(&cpu.cs);
	rtl_push(&ret_addr);
	//push eflags,cs,next eip
	assert(NO <= cpu.idtr.limit);
	//GateDesc = (GateDescriptor) vaddr_read(ret_addr + NO * 8 , 8);
	//memcpy(&GateDesc, cpu.idtr.base + NO * 8, 8);
	GateDesc gate_desc;
	gate_desc.val1 = vaddr_read(cpu.idtr.base + NO * 8, 4);
	gate_desc.val2 = vaddr_read(cpu.idtr.base + NO * 8 + 4, 4);
	if(gate_desc.present == 0){
		panic("Invalid gate descriptor!");
		assert(0);
	}
	vaddr_t target = (gate_desc.offset_31_16 << 16) + (gate_desc.offset_15_0);
	rtl_j(target);
}

void dev_raise_intr() {
}
