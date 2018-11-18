#include "cpu/exec.h"
#include "device/port-io.h"

void difftest_skip_ref();
void difftest_skip_dut();
void raise_intr(uint8_t NO, vaddr_t ret_addr);

make_EHelper(lidt) {
  //TODO();
	//id_dest -- nanos/data[3];
	rtl_li(&t0,id_dest->addr); //load the addr of data[3] into t0
	Log("get addr of data: %d\n",id_dest->addr);
	rtl_li(&cpu.idtr.limit, vaddr_read(t0,2)); //load data[0] to idtr.limit 
	Log("load idtr.limit:%d\n",vaddr_read(t0,2));
	rtl_li(&cpu.idtr.base, vaddr_read(t0+2,4)); //load data[1-2] to idtr.base
	Log("load idtr.base: %d\n", vaddr_read(t0+2,4));

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

make_EHelper(int) {
  //TODO();
	switch(decoding.opcode){
		case 0xcc: raise_intr((uint8_t)0x3, decoding.seq_eip); break;
		case 0xcd: raise_intr(id_dest->val, decoding.seq_eip); break;
		case 0xce: raise_intr((uint8_t)0x4, decoding.seq_eip); break;
		default: raise_intr((uint8_t)id_dest->val, decoding.seq_eip);
	}

  print_asm("int %s", id_dest->str);

#if defined(DIFF_TEST) && defined(DIFF_TEST_QEMU)
  difftest_skip_dut();
#endif
}

make_EHelper(iret) {
  TODO();

  print_asm("iret");
}

make_EHelper(in) {
 // TODO();
	switch(id_dest->width){
		case 4: rtl_li(&t0, pio_read_l(id_src->val)); break; 
		case 2: rtl_li(&t0, pio_read_w(id_src->val)); break; 
		case 1: rtl_li(&t0, pio_read_b(id_src->val)); break; 
	}
	operand_write(id_dest,&t0);

  print_asm_template2(in);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

make_EHelper(out) {
  //TODO();
	switch(id_dest->width){
		case 4: pio_write_l(id_dest->val, id_src->val); break;
		case 2: pio_write_w(id_dest->val, id_src->val); break;
		case 1: pio_write_b(id_dest->val, id_src->val); break;
	}

  print_asm_template2(out);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}
