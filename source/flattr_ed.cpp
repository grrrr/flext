/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flattr_ed.cpp
    \brief Attribute editor (property dialog) for PD
*/

#include "flext.h"

#if FLEXT_SYS == FLEXT_SYS_PD && !defined(FLEXT_NOATTREDIT)

/*
#ifdef PD_DEVEL_VERSION
#define FLEXT_CLONEWIDGET
#endif
*/

#ifdef _MSC_VER
#pragma warning( disable : 4091 ) 
#endif

#ifndef FLEXT_CLONEWIDGET
// This is problematic... non-public headers!
// compilation is specific for a compiled version!!
#include <m_imp.h>
#endif

#include <g_canvas.h>

#include <string.h>
#include <stdio.h>

static t_widgetbehavior widgetbehavior; 

#ifndef FLEXT_CLONEWIDGET
static void (*ori_vis)(t_gobj *c, t_glist *, int vis) = NULL;
#endif

void flext_base::SetAttrEditor(t_classid c)
{
	// widgetbehavior struct MUST be resident... (static is just ok here)

#ifndef FLEXT_CLONEWIDGET
	ori_vis = c->c_wb->w_visfn; 
	widgetbehavior.w_getrectfn =    c->c_wb->w_getrectfn; 
    widgetbehavior.w_displacefn =   c->c_wb->w_displacefn; 
    widgetbehavior.w_selectfn =     c->c_wb->w_selectfn; 
    widgetbehavior.w_activatefn =   c->c_wb->w_activatefn; 
    widgetbehavior.w_deletefn =     c->c_wb->w_deletefn; 
    widgetbehavior.w_clickfn =      c->c_wb->w_clickfn;
#else
	widgetbehavior.w_getrectfn =    text_widgetbehavior.w_getrectfn; 
    widgetbehavior.w_displacefn =   text_widgetbehavior.w_displacefn; 
    widgetbehavior.w_selectfn =     text_widgetbehavior.w_selectfn; 
    widgetbehavior.w_activatefn =   text_widgetbehavior.w_activatefn; 
    widgetbehavior.w_deletefn =     text_widgetbehavior.w_deletefn; 
    widgetbehavior.w_clickfn =      text_widgetbehavior.w_clickfn;
#endif

#if PD_MINOR_VERSION >= 37
    class_setpropertiesfn(c,cb_GfxProperties);
    class_setsavefn(c,cb_GfxSave);
#else
    widgetbehavior.w_propertiesfn = cb_GfxProperties;
    widgetbehavior.w_savefn =       cb_GfxSave;
#endif

    widgetbehavior.w_visfn =        cb_GfxVis;
    class_setwidget(c, &widgetbehavior);

	// generate the script for the property dialog

	sys_gui(
		"proc flext_apply {id alen} {\n"
			// strip "." from the TK id to make a variable name suffix
			"set vid [string trimleft $id .]\n"

			// make a list of the attribute values (including save flags)
			"set lst {}\n"
			"for {set ix 1} {$ix <= $alen} {incr ix} {\n"
				"set var_attr_name [concat [concat var_name_$ix]_$vid ]\n"
				"global $var_attr_name\n"
				"set var_attr_init [concat [concat var_init_$ix]_$vid ]\n"
				"global $var_attr_init\n"
				"set var_attr_val [concat [concat var_val_$ix]_$vid ]\n"
				"global $var_attr_val\n"
				"set var_attr_save [concat [concat var_save_$ix]_$vid ]\n"
				"global $var_attr_save\n"
				"set var_attr_type [concat [concat var_type_$ix]_$vid ]\n"
				"global $var_attr_type\n"
		
				"if { [expr $$var_attr_type] != 0 } {\n"
					// attribute is puttable

					"lappend lst [eval concat $$var_attr_name]\n" 

					// process current value
					"set len [llength [expr $$var_attr_val]]\n"
					"if { $len == 1 } {\n"
						// it's an atom
						"lappend lst [expr $$var_attr_val]\n" 
					"} else {\n"
						// it's a list
						"set lst [concat $lst {list} $len [expr $$var_attr_val]]\n" 
					"}\n"

					// process init value
					"set len [llength [expr $$var_attr_init]]\n"
					"if { $len == 1 } {\n"
						// it's an atom
						"lappend lst [expr $$var_attr_init]\n" 
					"} else {\n"
						// it's a list
						"set lst [concat $lst {list} $len [expr $$var_attr_init]]\n" 
					"}\n"

					"lappend lst [eval concat $$var_attr_save]\n" 
				"}\n"
			"}\n"

			"set cmd [concat $id attributedialog $lst \\;]\n"
			"pd $cmd\n"
		"}\n"

		"proc flext_cancel {id} {\n"
			"set cmd [concat $id cancel \\;]\n"
			"pd $cmd\n"
		"}\n"

		"proc flext_ok {id alen} {\n"
			"flext_apply $id $alen\n"
			"flext_cancel $id\n"
		"}\n"

		"proc flext_copyval {dst src} {\n"
			"global $src\n"
			"global $dst\n"
			"set $dst [expr $$src]\n"
		"}\n"

		"proc flext_textcopy {id idtxt var} {\n"
			"global $var\n"
            "set $var [eval $idtxt get 0.0 end]\n"
            "destroy $id\n"
		"}\n"

		"proc flext_textzoom {id var title attr edit} {\n"
			"global $var\n"
        	"toplevel $id.w\n"
            "wm title $id.w [concat $title \" @\" $attr]\n"
//            "wm iconname $w \"text\"\n"
//            "positionWindow $id.w\n"

            "frame $id.w.buttons\n"
            "pack $id.w.buttons -side bottom -fill x -pady 2m\n"

//            "if { $edit != 0 } {set st normal} {set st disabled}\n"
            "text $id.w.text -relief sunken -bd 2 -yscrollcommand \"$id.w.scroll set\" -setgrid 1 -width 80 -height 20\n"  //  -state $st
            "scrollbar $id.w.scroll -command \"$id.w.text yview\"\n"
            "pack $id.w.scroll -side right -fill y\n"
            "pack $id.w.text -expand yes -fill both\n"

			"if { $edit != 0 } {\n"
                "button $id.w.buttons.ok -text OK -command \"flext_textcopy $id.w $id.w.text $var\"\n"
                "pack $id.w.buttons.ok -side left -expand 1\n"
//    			"bind $id.w {<Shift-KeyPress-Return>} \"flext_textcopy $id.w $id.w.text $var\"\n"
    		"}\n"

            "button $id.w.buttons.cancel -text Cancel -command \"destroy $id.w\"\n"
            "pack $id.w.buttons.cancel -side left -expand 1\n"
			"bind $id.w {<KeyPress-Escape>} \"destroy $id.w\"\n"

            "$id.w.text insert 0.0 [expr $$var]\n"
            "$id.w.text mark set insert 0.0\n"
        "}\n"

		"proc pdtk_flext_dialog {id title attrlist} {\n"
				"set vid [string trimleft $id .]\n"
				"set alen [expr [llength $attrlist] / 6 ]\n"

				"toplevel $id\n"
				"wm title $id $title\n" 
				"wm protocol $id WM_DELETE_WINDOW [concat flext_cancel $id]\n"

				"set row 0\n"

                // set grow parameters
                "grid columnconfigure $id 0 -weight 1\n"  // label
                "grid columnconfigure $id {1 4} -weight 3\n" // value entry
                "grid columnconfigure $id {2 3} -weight 0\n"  // copy buttons
                "grid columnconfigure $id {5 6 7} -weight 0\n" // radio buttons

//                "grid rowconfigure $id {0 1 2} -weight 0\n"

				// set column labels
				"label $id.label -text {attribute} -height 2 -font {Helvetica 9 bold}\n"
				"label $id.init  -text {initial value} -height 2 -font {Helvetica 9 bold}\n"
				"label $id.copy  -text {copy} -height 2 -font {Helvetica 9 bold}\n"
				"label $id.val   -text {current value} -height 2 -font {Helvetica 9 bold}\n"
				"foreach {i txt} {0 {don't\rsave} 1 {do\rinit} 2 {always\rsave} } {\n"
					"label $id.b$i -text $txt -height 2 -font {Helvetica 9 bold}\n"
				"}\n"
//				"label $id.options -text {options} -height 2\n"

				"grid config $id.label -column 0 -row $row \n"
				"grid config $id.init  -column 1 -row $row \n"
				"grid config $id.copy  -column 2 -columnspan 2 -row $row \n"
				"grid config $id.val   -column 4 -row $row \n"
				"foreach i {0 1 2} { grid config $id.b$i -column [expr $i + 5] -row $row }\n"
//				"grid config $id.options -column 3 -row 0 \n"
				"incr row\n"

				// Separator
				"frame $id.sep -relief ridge -bd 1 -height 2\n"
				"grid config $id.sep -column 0 -columnspan 8 -row $row -pady 2 -sticky {snew}\n"
				"incr row\n"

				"set ix 1\n"
				"foreach {an av ai atp asv afl} $attrlist {\n"
                    "grid rowconfigure $id $row -weight 0\n"

                    // get attribute name
					"set var_attr_name [concat [concat var_name_$ix]_$vid ]\n"
					"global $var_attr_name\n"
					"set $var_attr_name $an\n"

					// get attribute init value (list)
					"set var_attr_init [concat [concat var_init_$ix]_$vid ]\n"
					"global $var_attr_init\n"
					"set $var_attr_init $ai\n"

					// get attribute value (list)
					"set var_attr_val [concat [concat var_val_$ix]_$vid ]\n"
					"global $var_attr_val\n"
					"set $var_attr_val $av\n"

					// get save flag
					"set var_attr_save [concat [concat var_save_$ix]_$vid ]\n"
					"global $var_attr_save\n"
					"set $var_attr_save $asv\n"

					// get type flag
					"set var_attr_type [concat [concat var_type_$ix]_$vid ]\n"
					"global $var_attr_type\n"
					"set $var_attr_type $afl\n"

					// add dialog elements to window

					// attribute label
					"label $id.label-$ix -text \"$an :\" -font {Helvetica 8 bold}\n"
					"grid config $id.label-$ix -column 0 -row $row -padx 5 -sticky {e}\n"

					"if { $afl != 0 } {\n"
						// attribute is puttable

						// entry field for initial value
						// entry field for current value

						// choose entry field type
						"switch $atp {\n"
							"0 - 1 {\n"  // int or float
								"entry $id.init-$ix -textvariable $var_attr_init\n"
								"entry $id.val-$ix -textvariable $var_attr_val\n"
							"}\n"
							"2 {\n"  // boolean
								"checkbutton $id.init-$ix -variable $var_attr_init\n"
								"checkbutton $id.val-$ix -variable $var_attr_val\n"
							"}\n"
							"3 {\n"  // symbol
								"entry $id.init-$ix -textvariable $var_attr_init\n"
								"entry $id.val-$ix -textvariable $var_attr_val\n"
							"}\n"
							"4 - 5 {\n"  // list or unknown
								"entry $id.init-$ix -textvariable $var_attr_init\n"
                                "bind $id.init-$ix {<Control-Button-1>} \" flext_textzoom $id.init-$ix $var_attr_init { $title } $an 1\"\n"
								"entry $id.val-$ix -textvariable $var_attr_val\n"
                                "bind $id.val-$ix {<Control-Button-1>} \" flext_textzoom $id.val-$ix $var_attr_val { $title } $an 1\"\n"
							"}\n"
						"}\n"

						"grid config $id.init-$ix  -column 1 -row $row -padx 5 -sticky {ew}\n"
						"grid config $id.val-$ix   -column 4 -row $row -padx 5 -sticky {ew}\n"

						"button $id.b2i-$ix -text {<-} -height 1 -command \" flext_copyval $var_attr_init $var_attr_val \"\n"
						"grid config $id.b2i-$ix  -column 2 -row $row  -sticky {ew}\n"
						"button $id.b2c-$ix -text {->} -height 1 -command \" flext_copyval $var_attr_val $var_attr_init \"\n"
						"grid config $id.b2c-$ix  -column 3 -row $row  -sticky {ew}\n"

	//					"tk_optionMenu $id.opt-$ix $var_attr_save {don't save} {initialize} {always save}\n"
	//					"grid config $id.opt-$ix -column 5 -row $ix \n"

						// radiobuttons
						"foreach {i c} {0 black 1 blue 2 red} {\n"
							"radiobutton $id.b$i-$ix -value $i -foreground $c -variable $var_attr_save \n"
							"grid config $id.b$i-$ix -column [expr $i + 5] -row $row  \n"
						"}\n"
					"} else {\n"
						// attribute is gettable only

						// entry field for current value (read-only)

						// choose display field type
						"switch $atp {\n"
							"0 - 1 {\n"  // int or float
								"entry $id.val-$ix -textvariable $var_attr_val -state disabled\n"
							"}\n"
							"2 {\n"  // boolean
								"checkbutton $id.val-$ix -variable $var_attr_val -state disabled\n"
							"}\n"
							"3 {\n"  // symbol
								"entry $id.val-$ix -textvariable $var_attr_val -state disabled\n"
							"}\n"
							"4 - 5 {\n"  // list or unknown
								"entry $id.val-$ix -textvariable $var_attr_val -state disabled\n"
                                "bind $id.val-$ix {<Control-Button-1>} \" flext_textzoom $id.val-$ix $var_attr_val { $title } $an 0\"\n"
							"}\n"
						"}\n"

//						"entry $id.val-$ix -textvariable $var_attr_val -state disabled\n"
						"grid config $id.val-$ix -column 4 -row $row -padx 5 -sticky {ew}\n"

						"label $id.readonly-$ix -text \"read-only\"\n"
						"grid config $id.readonly-$ix -column 5 -columnspan 3 -row $row -padx 5 -sticky {ew}\n"
					"}\n"

					// increase counter
					"incr ix\n"
					"incr row\n"
				"}\n"

				// Separator
				"frame $id.sep2 -relief ridge -bd 1 -height 2\n"
//                "grid rowconfigure $id $row -weight 0\n"
				"grid config $id.sep2 -column 0 -columnspan 8 -row $row -pady 5 -sticky {snew}\n"
				"incr row\n"

				// Buttons
				"frame $id.buttonframe\n"
				"pack $id.buttonframe -side bottom -fill x\n"

				"button $id.buttonframe.cancel -text {Cancel} -width 20 -command \" flext_cancel $id \"\n"
				"button $id.buttonframe.apply -text {Apply} -width 20 -command \" flext_apply $id $alen \"\n"
				"button $id.buttonframe.ok -text {OK} -width 20 -command \" flext_ok $id $alen \"\n"

				"pack $id.buttonframe.cancel -side left -expand 1\n"
				"pack $id.buttonframe.apply -side left -expand 1\n"
				"pack $id.buttonframe.ok -side left -expand 1\n"

//                "grid rowconfigure $id $row -weight 0\n"
				"grid config $id.buttonframe -column 0 -columnspan 8 -row $row -pady 5 -sticky {ew}\n"

				// Key bindings
				"bind $id {<KeyPress-Escape>} \" flext_cancel $id \"\n"
				"bind $id {<KeyPress-Return>} \" flext_ok $id $alen \"\n"
				"bind $id {<Shift-KeyPress-Return>} \" flext_apply $id $alen \"\n"
		"}\n"
	);
}

void flext_base::cb_GfxProperties(t_gobj *c, t_glist *)
{
	flext_base *th = thisObject(c);
	char buf[10000],*b = buf;

	STD::sprintf(b, "pdtk_flext_dialog %%s { "); b += strlen(b);

	t_text *x = (t_text *)c;
	FLEXT_ASSERT(x->te_binbuf);

	int argc = binbuf_getnatom(x->te_binbuf);
	t_atom *argv = binbuf_getvec(x->te_binbuf);

	PrintList(argc,argv,b,sizeof(buf)+buf-b);	b += strlen(b);

	STD::sprintf(b, " } { "); b += strlen(b);

	AtomList la;
	th->ListAttrib(la);
	int cnt = la.Count();

	for(int i = 0; i < cnt; ++i) {
		const t_symbol *sym = GetSymbol(la[i]); 

		// get attribute
		AttrItem *gattr = th->FindAttrib(sym,true);
		// get puttable attribute
		AttrItem *pattr = gattr?gattr->Counterpart():th->FindAttrib(sym,false);

		// get flags
		int sv;
		const AtomList *initdata;
		AttrDataCont::iterator it = th->attrdata->find(sym);
		if(it == th->attrdata->end())
			sv = 0,initdata = NULL;
		else {
			const AttrData &a = it->second;
			if(a.IsSaved())
				sv = 2;
			else if(a.IsInit())
				sv = 1;
			else 
				sv = 0;
			initdata = a.IsInitValue()?&a.GetInitValue():NULL;
		}

		// get attribute type
		int tp;
		bool list;
		switch((gattr?gattr:pattr)->argtp) {
			case a_int: tp = 0; list = false; break;
			case a_float: tp = 1; list = false; break;
			case a_bool: tp = 2; list = false; break;
			case a_symbol: tp = 3; list = true; break;
			case a_list: 
			case a_LIST: tp = 4; list = true; break;
			default: 
				tp = 5;	list = true; 
				FLEXT_ASSERT(false);
		}

		STD::sprintf(b,list?"%s {":"%s ",GetString(sym)); b += strlen(b);

		AtomList lv;
		if(gattr) { // gettable attribute is present
			// Retrieve attribute value
			th->GetAttrib(gattr,lv);

			PrintList(lv.Count(),lv.Atoms(),b,sizeof(buf)+buf-b); b += strlen(b);
		}
		else {
			strcpy(b,"{}"); b += strlen(b);
		}

		strcpy(b, list?"} {":" "); b += strlen(b);

		if(pattr) {
			// if there is initialization data take this, otherwise take the current data
			const AtomList &lp = initdata?*initdata:lv;

			PrintList(lp.Count(),lp.Atoms(),b,sizeof(buf)+buf-b); b += strlen(b);
		}
		else {
			strcpy(b,"{}"); b += strlen(b);
		}


		STD::sprintf(b, list?"} %i %i %i ":" %i %i %i ",tp,sv,pattr?(pattr->BothExist()?2:1):0); b += strlen(b);
	}

	strcpy(b, " }\n");

	gfxstub_new((t_pd *)th->thisHdr(), th->thisHdr(), buf);
}

//! Strip the attributes off the object command line
void flext_base::cb_GfxVis(t_gobj *c, t_glist *gl, int vis)
{
    // show object if it's not a GOP
    if(!gl->gl_isgraph || gl->gl_havewindow) {

	    t_text *x = (t_text *)c;
	    FLEXT_ASSERT(x->te_binbuf);

	    int argc = binbuf_getnatom(x->te_binbuf);
	    t_atom *argv = binbuf_getvec(x->te_binbuf);
	    int cnt = CheckAttrib(argc,argv);

	    if(cnt)	{
		    t_binbuf *nb = binbuf_new();
		    binbuf_restore(nb,cnt,argv);
		    binbuf_free(x->te_binbuf);
		    x->te_binbuf = nb;
	    }

	    t_rtext *rt = glist_findrtext(gl,x);
	    rtext_retext(rt);

        // now display the changed text with the normal drawing function
    #ifdef FLEXT_CLONEWIDGET
	    text_widgetbehavior.w_visfn(c,gl,vis);
    #else
	    ori_vis(c,gl,vis);
    #endif
   }
}

static void BinbufAdd(t_binbuf *b,const t_atom &at)
{
   	char tbuf[MAXPDSTRING];
	if(flext::IsString(at))
		binbuf_addv(b,"s",flext::GetSymbol(at));
	else if(flext::IsFloat(at))
		binbuf_addv(b,"f",flext::GetFloat(at));
	else if(flext::IsInt(at))
		binbuf_addv(b,"i",flext::GetInt(at));
	else if(at.a_type == A_DOLLAR) {
    	sprintf(tbuf, "$%d", at.a_w.w_index);
		binbuf_addv(b,"s",flext::MakeSymbol(tbuf));
	}
	else if(at.a_type == A_DOLLSYM) {
    	sprintf(tbuf, "$%s", at.a_w.w_symbol->s_name);
		binbuf_addv(b,"s",flext::MakeSymbol(tbuf));
	}
	else
		FLEXT_ASSERT(false);
}

void flext_base::cb_GfxSave(t_gobj *c, t_binbuf *b)
{
	flext_base *th = thisObject(c);
	t_text *t = (t_text *)c;
	binbuf_addv(b, "ssiis", gensym("#X"),gensym("obj"),	t->te_xpix, t->te_ypix,MakeSymbol(th->thisName()));

	int argc = binbuf_getnatom(t->te_binbuf);
	t_atom *argv = binbuf_getvec(t->te_binbuf);
	int i,cnt = CheckAttrib(argc,argv);

	// process the creation arguments
	for(i = 1; i < cnt; ++i) BinbufAdd(b,argv[i]);

	// process the attributes
	AtomList la;
	th->ListAttrib(la);
	cnt = la.Count();

	for(i = 0; i < cnt; ++i) {
		const t_symbol *sym = GetSymbol(la[i]);
		AtomList lv;
		const AtomList *lref = NULL;
		AttrDataCont::iterator it = th->attrdata->find(sym);

		if(it != th->attrdata->end()) {
			const AttrData &a = it->second;
			if(a.IsInit() && a.IsInitValue()) {
				lref = &a.GetInitValue();
/*
				// check for $-parameters
				lv = lref->Count();
				for(int j = 0; j < lref->Count(); ++j) {
					const char *s = IsSymbol((*lref)[j])?GetString((*lref)[j]):NULL;
					if(s && s[0] == '$') { // TODO: More refined checking?
						// prepend a "\"
						char tmp[256]; *tmp = '\\';
						strcpy(tmp+1,s);
						SetString(lv[j],tmp);
					}
					else
						lv[i] = (*lref)[j];
				}

				lref = &lv;
*/
			}
			else if(a.IsSaved()) {
				AttrItem *attr = th->FindAttrib(sym,true);

				// attribute must be gettable (so that the data can be retrieved) and puttable (so that the data can be inited)
				if(attr && attr->BothExist()) {
					th->GetAttrib(attr,lv); 
					lref = &lv;
				}
			}
		}

		if(lref) {
			char attrname[256]; *attrname= '@';
			// store name
			strcpy(attrname+1,GetString(sym));
			binbuf_addv(b,"s",MakeSymbol(attrname));

			// store value
			for(int j = 0; j < lref->Count(); ++j) BinbufAdd(b,(*lref)[j]);
		}
	}

	binbuf_addv(b, ";");
}

bool flext_base::cb_AttrDialog(flext_base *th,int argc,const t_atom *argv)
{
	int i = 0;
	if(IsSymbol(argv[i]) && GetSymbol(argv[i]) == sym_list) ++i;

	for(; i < argc; ) {
		FLEXT_ASSERT(IsSymbol(argv[i]));

		// get name
		const t_symbol *aname = GetSymbol(argv[i]);
		i++;

		// get current value
		int ccnt,coffs;
		if(IsSymbol(argv[i]) && GetSymbol(argv[i]) == sym_list) {
			i++;
			FLEXT_ASSERT(CanbeInt(argv[i]));
			ccnt = GetAInt(argv[i]);
			coffs = ++i;
		}
		else
			coffs = i,ccnt = 1;
		i += ccnt;

		// get init value
		int icnt,ioffs;
		if(IsSymbol(argv[i]) && GetSymbol(argv[i]) == sym_list) {
			i++;
			FLEXT_ASSERT(CanbeInt(argv[i]));
			icnt = GetAInt(argv[i]);
			ioffs = ++i;
		}
		else
			ioffs = i,icnt = 1;
		i += icnt;

		FLEXT_ASSERT(i < argc);
		int sv = GetAInt(argv[i]);
		++i;

		// find puttable attribute
		AttrItem *attr = th->FindAttrib(aname,false);
		if(attr) {
			bool ret = th->SetAttrib(attr,ccnt,argv+coffs);
			FLEXT_ASSERT(ret);

			AttrDataCont::iterator it = th->attrdata->find(aname);

			if(sv >= 1) {
				// if data not present create it
				if(it == th->attrdata->end()) {
					AttrDataPair pair; pair.first = aname;
					it = th->attrdata->insert(th->attrdata->begin(),pair);
				}

				AttrData &a = it->second;
				a.SetSave(sv == 2);
				a.SetInit(true);
				a.SetInitValue(icnt,argv+ioffs);
			}
			else {
				if(it != th->attrdata->end()) {
					AttrData &a = it->second;
					// if data is present reset flags
					a.SetSave(false);
					a.SetInit(false);

					// let init data as is
				}
			}
		}
		else {
			post("%s - Attribute %s can't be set",th->thisName(),GetString(aname));
		}
	}
	return true;
}

#endif // FLEXT_SYS_PD
