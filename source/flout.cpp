/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flout.cpp
    \brief Implementation of the flext outlet functionality.
*/

#include "flext.h"
#include "flinternal.h"
#include <string.h>
 
#ifndef FLEXT_THREADS
void flext_base::ToOutBang(outlet *o) const { CRITON(); outlet_bang((t_outlet *)o); CRITOFF(); }
void flext_base::ToOutFloat(outlet *o,float f) const { CRITON(); outlet_float((t_outlet *)o,f); CRITOFF(); }
void flext_base::ToOutInt(outlet *o,int f) const { CRITON(); outlet_flint((t_outlet *)o,f); CRITOFF(); }
void flext_base::ToOutSymbol(outlet *o,const t_symbol *s) const { CRITON(); outlet_symbol((t_outlet *)o,const_cast<t_symbol *>(s)); CRITOFF(); }
void flext_base::ToOutList(outlet *o,int argc,const t_atom *argv) const { CRITON(); outlet_list((t_outlet *)o,gensym("list"),argc,(t_atom *)argv); CRITOFF(); }
void flext_base::ToOutAnything(outlet *o,const t_symbol *s,int argc,const t_atom *argv) const { CRITON(); outlet_anything((t_outlet *)o,const_cast<t_symbol *>(s),argc,(t_atom *)argv); CRITOFF(); }
#else
void flext_base::ToOutBang(outlet *o) const { if(IsSystemThread()) { CRITON(); outlet_bang((t_outlet *)o); CRITOFF(); } else ToQueueBang(o); }
void flext_base::ToOutFloat(outlet *o,float f) const { if(IsSystemThread()) { CRITON(); outlet_float((t_outlet *)o,f); CRITOFF(); } else ToQueueFloat(o,f); }
void flext_base::ToOutInt(outlet *o,int f) const { if(IsSystemThread()) { CRITON(); outlet_flint((t_outlet *)o,f); CRITOFF(); } else ToQueueInt(o,f); }
void flext_base::ToOutSymbol(outlet *o,const t_symbol *s) const { if(IsSystemThread()) { CRITON(); outlet_symbol((t_outlet *)o,const_cast<t_symbol *>(s)); CRITOFF(); } else ToQueueSymbol(o,s); }
void flext_base::ToOutList(outlet *o,int argc,const t_atom *argv) const { if(IsSystemThread()) { CRITON(); outlet_list((t_outlet *)o,gensym("list"),argc,(t_atom *)argv); CRITOFF(); } else ToQueueList(o,argc,(t_atom *)argv); }
void flext_base::ToOutAnything(outlet *o,const t_symbol *s,int argc,const t_atom *argv) const { if(IsSystemThread()) { CRITON(); outlet_anything((t_outlet *)o,const_cast<t_symbol *>(s),argc,(t_atom *)argv); CRITOFF(); } else ToQueueAnything(o,s,argc,(t_atom *)argv); }
#endif


bool flext_base::InitInlets()
{
	bool ok = true;

	// ----------------------------------
	// create inlets
	// ----------------------------------

	incnt = insigs = 0; 

	if(inlist) {
		xlet *xi;
		incnt = 0;
		for(xi = inlist; xi; xi = xi->nxt) ++incnt;
		xlet::type *list = new xlet::type[incnt];
		int i;
		for(xi = inlist,i = 0; xi; xi = xi->nxt,++i) list[i] = xi->tp;
		
#if FLEXT_SYS == FLEXT_SYS_MAX		
		// copy inlet descriptions
		indesc = new char *[incnt];
		for(xi = inlist,i = 0; xi; xi = xi->nxt,++i) {
			int l = xi->desc?strlen(xi->desc):0;
			if(l) {
				indesc[i] = new char[l+1];
				memcpy(indesc[i],xi->desc,l);
				indesc[i][l] = 0;
			}
			else
				indesc[i] = NULL;
		}
#endif

		delete inlist; inlist = NULL;
		
		inlets = new px_object *[incnt];
		for(i = 0; i < incnt; ++i) inlets[i] = NULL;
		
		// type info is now in list array
#if FLEXT_SYS == FLEXT_SYS_PD
		{
			int cnt = 0;

			if(incnt >= 1) {
				switch(list[0]) {
					case xlet::tp_sig:
						++insigs;
						break;
					default:
						// leftmost inlet is already there...
						break;
				} 
				++cnt;
			}		

			for(int ix = 1; ix < incnt; ++ix,++cnt) {
				switch(list[ix]) {
					case xlet::tp_float:
					case xlet::tp_int: {
						char sym[] = "ft??";
						if(ix >= 10) { 
							if(compatibility) {
								// Max allows max. 9 inlets
								post("%s: Only 9 float/int inlets allowed in compatibility mode",thisName());
								ok = false;
							}
							else {
								if(ix > 99)
									post("%s: Inlet index > 99 not allowed for float/int inlets",thisName());
								sym[2] = '0'+ix/10,sym[3] = '0'+ix%10;
							}
						}
						else 
							sym[2] = '0'+ix,sym[3] = 0;  
					    if(ok) inlet_new(&x_obj->obj, &x_obj->obj.ob_pd, &s_float, gensym(sym)); 
						break;
					}
					case xlet::tp_sym: 
					    (inlets[ix] = (px_object *)pd_new(px_class))->init(this,ix);  // proxy for 2nd inlet messages 
						inlet_new(&x_obj->obj,&inlets[ix]->obj.ob_pd, &s_symbol, &s_symbol);  
						break;
					case xlet::tp_list:
					    (inlets[ix] = (px_object *)pd_new(px_class))->init(this,ix);  // proxy for 2nd inlet messages 
						inlet_new(&x_obj->obj,&inlets[ix]->obj.ob_pd, &s_list, &s_list);  
						break;
					case xlet::tp_any:
					    (inlets[ix] = (px_object *)pd_new(px_class))->init(this,ix);  // proxy for 2nd inlet messages 
						inlet_new(&x_obj->obj,&inlets[ix]->obj.ob_pd, 0, 0);  
						break;
					case xlet::tp_sig:
						if(compatibility && list[ix-1] != xlet::tp_sig) {
							post("%s: All signal inlets must be left-aligned in compatibility mode",thisName());
							ok = false;
						}
						else {
							// pd doesn't seem to be able to handle signals and messages into the same inlet...
							
							inlet_new(&x_obj->obj, &x_obj->obj.ob_pd, &s_signal, &s_signal);  
							++insigs;
						}
						break;
					default:
						error("%s: Wrong type for inlet #%i: %i",thisName(),ix,(int)list[ix]);
						ok = false;
				} 
			}

			incnt = cnt;
		}
#elif FLEXT_SYS == FLEXT_SYS_MAX
		{
			int ix,cnt;
			// count leftmost signal inlets
			while(insigs < incnt && list[insigs] == xlet::tp_sig) ++insigs;
			
			for(cnt = 0,ix = incnt-1; ix >= insigs; --ix,++cnt) {
				if(ix == 0) {
					if(list[ix] != xlet::tp_any) {
						error("%s: Leftmost inlet must be of type signal or default",thisName());
						ok = false;
					}
				}
				else {
					switch(list[ix]) {
						case xlet::tp_sig:
							error("%s: All signal inlets must be at the left side",thisName());
							ok = false;
							break;
						case xlet::tp_float:
							if(ix >= 10) { 
								post("%s: Only 9 float inlets possible",thisName());
								ok = false;
							}
							else
								floatin(x_obj,ix);  
							break;
						case xlet::tp_int:
							if(ix >= 10) { 
								post("%s: Only 9 int inlets possible",thisName());
								ok = false;
							}
							else
								intin(x_obj,ix);  
							break;
						case xlet::tp_any: // non-leftmost
						case xlet::tp_sym:
						case xlet::tp_list:
							inlets[ix] = (px_object *)proxy_new(x_obj,ix,&((flext_hdr *)x_obj)->curinlet);  
							break;
						default:
							error("%s: Wrong type for inlet #%i: %i",thisName(),ix,(int)list[ix]);
							ok = false;
					} 
				}
			}

//			incnt = cnt;
	
			if(insigs) 
//				dsp_setup(thisHdr(),insigs); // signal inlets	
				dsp_setupbox(thisHdr(),insigs); // signal inlets	
		}
#else
#error
#endif

		delete[] list;
	}

	return ok;	
}

bool flext_base::InitOutlets()
{
	bool ok = true;

	// ----------------------------------
	// create outlets
	// ----------------------------------

	outcnt = outsigs = 0; 
	
#if FLEXT_SYS == FLEXT_SYS_MAX
	// for Max/MSP the rightmost outlet has to be created first
	if(procattr) 
		outattr = (outlet *)newout_anything(&x_obj->obj);
#endif

	if(outlist) {
		xlet *xi;

		// count outlets
		outcnt = 0;
		for(xi = outlist; xi; xi = xi->nxt) ++outcnt;

		xlet::type *list = new xlet::type[outcnt];
		int i;
		for(xi = outlist,i = 0; xi; xi = xi->nxt,++i) list[i] = xi->tp;

#if FLEXT_SYS == FLEXT_SYS_MAX		
		// copy outlet descriptions
		outdesc = new char *[outcnt];
		for(xi = outlist,i = 0; xi; xi = xi->nxt,++i) {
			int l = xi->desc?strlen(xi->desc):0;
			if(l) {
				outdesc[i] = new char[l+1];
				memcpy(outdesc[i],xi->desc,l);
				outdesc[i][l] = 0;
			}
			else
				outdesc[i] = NULL;
		}
#endif

		delete outlist; outlist = NULL;
		
		outlets = new outlet *[outcnt];

		// type info is now in list array
#if FLEXT_SYS == FLEXT_SYS_PD
		for(int ix = 0; ix < outcnt; ++ix) 
#elif FLEXT_SYS == FLEXT_SYS_MAX
		for(int ix = outcnt-1; ix >= 0; --ix) 
#else
#error
#endif
		{
			switch(list[ix]) {
				case xlet::tp_float:
					outlets[ix] = (outlet *)newout_float(&x_obj->obj);
					break;
				case xlet::tp_int: 
					outlets[ix] = (outlet *)newout_flint(&x_obj->obj);
					break;
				case xlet::tp_sig:
					outlets[ix] = (outlet *)newout_signal(&x_obj->obj);
					++outsigs;
					break;
				case xlet::tp_sym:
					outlets[ix] = (outlet *)newout_symbol(&x_obj->obj);
					break;
				case xlet::tp_list:
					outlets[ix] = (outlet *)newout_list(&x_obj->obj);
					break;
				case xlet::tp_any:
					outlets[ix] = (outlet *)newout_anything(&x_obj->obj);
					break;
#ifdef FLEXT_DEBUG
				default:
					ERRINTERNAL();
					ok = false;
#endif
			} 
		}

		delete[] list;
	}

	if(procattr) {
#if FLEXT_SYS == FLEXT_SYS_PD
		// attribute dump outlet is the last one
		outattr = (outlet *)newout_anything(&x_obj->obj);
#endif

	}
	
	return ok;
}


