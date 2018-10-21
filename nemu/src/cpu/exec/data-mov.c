#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
	rtl_push(&id_dest->val);
	Log("Successful push!");
  //TODO();

  print_asm_template1(push);
}

make_EHelper(pop) {
	rtl_pop(&id_src->val);
	operand_write(id_dest,&id_src->val);
	Log("Successful pop!");
  //TODO();

  print_asm_template1(pop);
}

make_EHelper(pusha) {
	rtlreg_t temp = 0;
	rtl_lm(&temp,&cpu.esp,4);	
	rtl_push(&cpu.eax);
	rtl_push(&cpu.ecx);
	rtl_push(&cpu.edx);
	rtl_push(&cpu.ebx);
	rtl_push(&temp);
	rtl_push(&cpu.ebp);
	rtl_push(&cpu.esi);
	rtl_push(&cpu.edi);
  //TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
	rtlreg_t throwaway = 0;
	rtl_pop(&cpu.edi);
	rtl_pop(&cpu.esi);
	rtl_pop(&cpu.ebp);
	rtl_pop(&throwaway);
	rtl_pop(&cpu.ebx);
	rtl_pop(&cpu.edx);
	rtl_pop(&cpu.ecx);
	rtl_pop(&cpu.eax);
  //TODO();

  print_asm("popa");
}

make_EHelper(leave) {
	rtl_pop(&cpu.ebp);	
  //TODO();
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
		rtl_sext(&t0,&reg_l(R_EAX),2);
		rtl_mv(&reg_l(R_EDX), (&t0)+2);
		//DX:AX <- sign-ext of AX
  }
  else {
		//rtl_andi(&t0, &reg_l(R_EAX), 0x80000000);
		rtl_sari(&reg_l(R_EDX), &reg_l(R_EAX), 31);
		//EDX:EAX <- sign-ext of EAX
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    //TODO();
		rtl_sext(&t0, &reg_l(R_EAX), 1);
		rtl_mv(&reg_l(R_EAX), &t0);
		//AX <- sign-ext of AL
  }
  else {
    //TODO();
		rtl_sext(&t0, &reg_l(R_EAX), 2);
		rtl_sm(&reg_l(R_EAX), &t0, 4);
		//EAX <- sign-ext of AX
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t0, &id_src->val, id_src->width);
  operand_write(id_dest, &t0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
