#include "cpu/exec.h"

/*data-mov.c*/
make_EHelper(mov);
make_EHelper(push);


/*logic.c*/
make_EHelper(xor);


/*control.c*/
make_EHelper(call);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);
