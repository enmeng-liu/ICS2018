#include "cpu/exec.h"

/*data-mov.c*/
make_EHelper(mov);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(pusha);
make_EHelper(popa);
make_EHelper(leave);
make_EHelper(movsx);
make_EHelper(cltd);
make_EHelper(cwtl);


/*arith.c*/
make_EHelper(add);
make_EHelper(sub);
make_EHelper(cmp);
make_EHelper(inc);


/*logic.c*/
make_EHelper(xor);


/*control.c*/
make_EHelper(call);
make_EHelper(ret);


/*special.c*/
make_EHelper(nop);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);
