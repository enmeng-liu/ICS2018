#include "cpu/exec.h"

/*data-mov.c*/
make_EHelper(mov);
make_EHelper(push);


/*arith.c*/
make_EHelper(sub);


/*logic.c*/
make_EHelper(xor);


/*control.c*/
make_EHelper(call);


/*special.c*/
make_EHelper(nop);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);
