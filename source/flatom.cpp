#include "atoms.h"
//#include <math.h>


AtomList::AtomList(I argc,t_atom *argv):
	cnt(argc)
{
	lst = new t_atom[cnt];

	if(argv) {
		for(I i = 0; i < argc; ++i) {
			switch(lst[i].a_type = argv[i].a_type) {
			case A_FLOAT:
				lst[i].a_w.w_float = argv[i].a_w.w_float;
				break;
#ifdef MAXMSP
			case A_LONG:
				lst[i].a_w.w_int = argv[i].a_w.w_int;
				break;
#endif
			case A_SYMBOL:
				lst[i].a_w.w_symbol = argv[i].a_w.w_symbol;
				break;
#ifdef PD
			case A_POINTER:
				lst[i].a_w.w_gpointer = argv[i].a_w.w_gpointer;
				break;
#endif
			default:
				post("AtomList - atom type (%i) not supported",lst[i].a_type);
				lst[i].a_type = A_NULL;
				break;
			}
		}
	}
}

AtomList::~AtomList()
{
	if(lst) delete[] lst;
}

