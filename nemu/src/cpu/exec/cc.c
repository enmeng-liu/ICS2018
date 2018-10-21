#include "cpu/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O:
			rtl_get_OF(&t0);
			*dest = t0 ? 1 : 0;
			break;
			//set byte if overflow (OF = 1)
    case CC_B:
			rtl_get_CF(&t0);
			*dest = t0 ? 1 : 0;
			break;		
			//set byte if below (CF = 1)
    case CC_E:
			rtl_get_ZF(&t0);
			*dest = t0 ? 1 : 0;
			break;
			//set byte if equal (ZF = 1)
    case CC_BE:
			rtl_get_ZF(&t0);
			rtl_get_CF(&t1);
			*dest = (t0 || t1) ? 1 : 0;
			break;
			//set byte if below or equal (CF = 1 or ZF = 1)
    case CC_S:
			rtl_get_SF(&t0);
			*dest = t0 ? 1 : 0;
		  break;
			//set byte if sign (SF = 1) 	
    case CC_L:
			rtl_get_SF(&t0);
			rtl_get_OF(&t1);
			*dest = (t0 != t1) ? 1 : 0;
			break;
			//set byte if less (SF != OF)
    case CC_LE:
			rtl_get_SF(&t0);
			rtl_get_OF(&t1);
			rtl_get_ZF(&t2);
			*dest = ((t0 != t1) || t2) ? 1 : 0;
			break;
			//set byte if less or equal (ZF = 1 or SF != OF)
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}
