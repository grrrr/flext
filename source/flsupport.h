/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flsupport.h
	\brief flext support functions and classes   
*/

#ifndef __FLSUPPORT_H
#define __FLSUPPORT_H

#include "flstdc.h"
#include <new>

class FLEXT_SHARE FLEXT_CLASSDEF(flext_root);
typedef class FLEXT_CLASSDEF(flext_root) flext_root;

/*! \brief Flext root support class

    Moved memory functions and console output here so that all the classes
    contained in flext can use them
*/
class FLEXT_SHARE FLEXT_CLASSDEF(flext_root) {
public:
// --- console output -----------------------------------------------	

		//! post message to console
		static void post(const char *s,...);
		//! post error message to console
		static void error(const char *s,...);

// --- memory -------------------------------------------------------	

	/*!	\defgroup FLEXT_S_MEMORY Memory allocation functions
		@{ 
	*/

		/*! Overloaded new memory allocation method
			\note this uses a fast allocation method of the real-time system
			\warning Max/MSP (or MacOS) allows only 32K in overdrive mode!
		*/
		void *operator new(size_t bytes);
		//! Overloaded delete method
		void operator delete(void *blk);

		#ifndef __MRC__ // doesn't allow new[] overloading?!
		void *operator new[](size_t bytes) { return operator new(bytes); }
		void operator delete[](void *blk) { operator delete(blk); }
		#endif

		//! Get an aligned memory block
		static void *NewAligned(size_t bytes,int bitalign = 128);
		//! Free an aligned memory block
		static void FreeAligned(void *blk);
		//! Test for alignment
		static bool IsAligned(void *ptr,int bitalign = 128)	{ 
            return (reinterpret_cast<unsigned long>(ptr)&(bitalign-1)) == 0; 
        }
	//!	@}  FLEXT_S_MEMORY  	
};

#if !defined(_MSC_VER) && !defined(__BORLANDC__)
#define NEWTHROW throw(std::bad_alloc)
#define DELTHROW throw()
#else
#define NEWTHROW
#define DELTHROW
#endif

// define global new/delete operators
inline void *operator new(size_t bytes) NEWTHROW { return flext_root::operator new(bytes); }
inline void operator delete(void *blk) DELTHROW { flext_root::operator delete(blk); }
#ifndef __MRC__ // doesn't allow new[] overloading?!
inline void *operator new[](size_t bytes) NEWTHROW { return flext_root::operator new[](bytes); }
inline void operator delete[](void *blk) DELTHROW { flext_root::operator delete[](blk); }
#endif


class FLEXT_SHARE FLEXT_CLASSDEF(flext);
typedef class FLEXT_CLASSDEF(flext) flext;

class FLEXT_SHARE FLEXT_CLASSDEF(flext_base);

/*! \brief Flext support class

	A number of methods (most are static functions) are defined here for convenience.
	This class doesn't define any data members, hence it can be inherited to all
	classes (not only PD objects) to profit from the cross-platform functionality.
	Examples are the overloaded memory allocation, atom and atom list functions,
	thread functions and classes, the sample buffer class and others.

    This class can also be used for a non-object class (not representing an external object)
    and won't give any extra burden to it.
*/

class FLEXT_SHARE FLEXT_CLASSDEF(flext):
	public flext_root
{

	/*!	\defgroup FLEXT_SUPPORT Flext support class
		@{ 
	*/
public:

// --- version -----------------------------------------------	

    /*! \brief Flext version number 

        Return the version number of the flext library.
        For statically linked flext this is identical to the header definition FLEXT_VERSION,
        otherwise it reflects the version number of the shared flext library.
    */
    static int Version();    

    //! Flext version string
    static const char *VersionStr();    

// --- buffer/array stuff -----------------------------------------	

	/*!	\defgroup FLEXT_S_BUFFER Buffer handling
		@{ 
	*/

// not for Jmax at the moment
#if FLEXT_SYS != FLEXT_SYS_JMAX

	//! Class for platform independent buffer handling
    class FLEXT_SHARE buffer:
        public flext_root
	{
	public:
		/*! \brief Construct buffer.
			\param s: symbol name, can be NULL
			\param delayed = true: only sets name, needs another Set(NULL) to really initialize the buffer 
			\remark As externals can be created prior to the buffer objects they are pointing to, initialization should be done at loadbang!
		*/
		buffer(const t_symbol *s = NULL,bool delayed = false);
		
		//! Destroy buffer
		~buffer();

		/*! \brief Check if the data is valid
		*/
		bool Ok() const { return sym != NULL && data != NULL; }
		
		/*! \brief Check if buffer is valid
		*/
		bool Valid() const;
		
		/*! \brief Check and update if the buffer has been changed (e.g. resized)
		*/
		bool Update();
		
		/*! \brief Set to specified buffer.
			\param nameonly: if true sets name only, but doesn't look at buffer actually
            \return -1 on failure, 0 on success, 1 if parameters (length, data ptr, channels) have changed
		*/
		int Set(const t_symbol *s = NULL,bool nameonly = false);
		
		/*! \brief Declare buffer content as dirty.
			\param refr: if true forces immediate graphics refresh
		*/
		void Dirty(bool refr = false);

		//! Clear the dirty flag.
		void ClearDirty();

		/*! Query whether the buffer content has been changed since the last ClearDirty()
			\note With mainstream versions of PD this will always return true, since the dirtiness can't be judged
		*/
		bool IsDirty() const;

		//! Get symbol of buffer 
		t_symbol *Symbol() const { return const_cast<t_symbol *>(sym); }

		//! Get literal name of buffer 
		const char *Name() const { return sym?GetString(sym):""; }
		
		/*! \brief Get pointer to buffer, channel and frame count.
			\remark Channels are interleaved
		*/
		t_sample *Data() { return data; }

		//! Get channel count
		int Channels() const { return chns; }
		//! Get frame count
		int Frames() const { return frames; }
		//! Set frame count
		void Frames(int fr,bool keep = false,bool zero = true);
		
		//! Graphic auto refresh interval
		void SetRefrIntv(float intv);

	protected:
        //! buffer name
		const t_symbol *sym;
        //! array holding audio data
		t_sample *data;
        //! number of audio channels
		int chns;
        //! number of frames (multiplied by chns for the number of samples)
        int frames;
#if FLEXT_SYS == FLEXT_SYS_PD
        //! pointer to the PD array structure
		t_garray *arr;
        //! update interval
		float interval;
        //! flag signaling that the data has been changed
		bool isdirty;
        //! flag showing that the update clock is active
        bool ticking;
        //! update clock
		t_clock *tick;
		//! last time the dirty flag was cleared (using the clock_getlogicaltime function)
		double cleantime;

	private:
        //! update clock callback
		static void cb_tick(buffer *b);
#elif FLEXT_SYS == FLEXT_SYS_MAX
		//! last time the dirty flag was cleared (using the gettime function)
		long cleantime;
#endif
	};

#endif // jmax

//!		@} FLEXT_S_BUFFER

// --- utilities --------------------------------------------------

	/*!	\defgroup FLEXT_S_UTIL Utility functions
		@{ 
	*/

	//! Copy an atom
	static void CopyAtom(t_atom *dst,const t_atom *src) { *dst = *src; }

	//! Print an atom
	static bool PrintAtom(const t_atom &a,char *buf,int bufsz);
	//! Scan an atom
	static bool ScanAtom(t_atom &a,const char *buf);

	//! Copy a list of atoms
	static t_atom *CopyList(int argc,const t_atom *argv);
	//! Print an atom list
	static bool PrintList(int argc,const t_atom *argv,char *buf,int bufsz);

	//! Copy a memory region
	static void CopyMem(void *dst,const void *src,int bytes);
	//! Copy a sample array
	static void CopySamples(t_sample *dst,const t_sample *src,int cnt);
	//! Set a memory region
	static void ZeroMem(void *dst,int bytes);
	//! Set a sample array to a fixed value
	static void SetSamples(t_sample *dst,int cnt,t_sample s);
	//! Set a sample array to 0
	static void ZeroSamples(t_sample *dst,int cnt) { SetSamples(dst,cnt,0); }	


	//! Get a 32 bit hash value frm an atom
	static unsigned long AtomHash(const t_atom &a);
	
	/*! \brief Fold value to a number of bits
		\remark Good for hash tables
	*/
	static unsigned int FoldBits(unsigned long h,int bits);
	
	//! \brief How many bits are necessary to represent n
	static int Int2Bits(unsigned long n);

//!		@} FLEXT_S_UTIL

// --- various symbols --------------------------------------------

	/*!	\defgroup FLEXT_S_ATOM Atom/list handling
		@{ 
	*/

	//! Symbol constant for "float"
	static const t_symbol *sym_float;
	//! Symbol constant for "symbol"
	static const t_symbol *sym_symbol;
	//! Symbol constant for "bang"
	static const t_symbol *sym_bang;
	//! Symbol constant for "list"
	static const t_symbol *sym_list;
	//! Symbol constant for "anything"
	static const t_symbol *sym_anything;

	/*! \brief Symbol constant for "int"
		\note Only the Max/MSP system has this defined as an internal type
	*/
	static const t_symbol *sym_int;

	/*! Symbol constant for "pointer" 
		\note Only PD has this defined as an internal type
	*/
	static const t_symbol *sym_pointer;

#if FLEXT_SYS == FLEXT_SYS_PD
	/*! \brief Symbol constant for "signal"
		\note PD only
	*/
	static const t_symbol *sym_signal;
#endif

#if FLEXT_SYS == FLEXT_SYS_JMAX
	//! Make a symbol from a string
	static const t_symbol *MakeSymbol(const char *s) { return ::fts_new_symbol(s); }
	//! Get symbol string
	static const char *GetString(const t_symbol *s); // ** TODO **
#else
	//! Make a symbol from a string
	static const t_symbol *MakeSymbol(const char *s) { return ::gensym(const_cast<char *>(s)); }
	//! Get symbol string
	static const char *GetString(const t_symbol *s) { return s->s_name; }  
#endif
	//! Check for symbol and get string
	static const char *GetAString(const t_symbol *s,const char *def = "") { return s?GetString(s):def; }

// --- atom stuff ----------------------------------------
		
	//! Set atom from another atom
	static void SetAtom(t_atom &a,const t_atom &b) { CopyAtom(&a,&b); }
	//! Compare two atoms
	static int CmpAtom(const t_atom &a,const t_atom &b);

	// there are some more comparison functions for t_atom types outside the class

#if FLEXT_SYS == FLEXT_SYS_JMAX
	//! Set atom from another atom
	static int GetType(const t_atom &a); // ** TODO **

	//! Check whether the atom is nothing
	static bool IsNothing(const t_atom &a) { return fts_is_a(&a,fts_void_class); }
	//! Set the atom to represent nothing
	static void SetNothing(t_atom &a) { fts_set_void(&a); }

	//! Check whether the atom is a float
	static bool IsFloat(const t_atom &a) { return fts_is_a(&a,fts_float_class); }
#else
	//! Set atom from another atom
	static int GetType(const t_atom &a) { return a.a_type; }

	//! Check whether the atom is nothing
	static bool IsNothing(const t_atom &a) { return a.a_type == A_NULL; }
	//! Set the atom to represent nothing
	static void SetNothing(t_atom &a) { a.a_type = A_NULL; }

	//! Check whether the atom is a float
	static bool IsFloat(const t_atom &a) { return a.a_type == A_FLOAT; }
#endif

	//! Check whether the atom can be represented as a float
	static bool CanbeFloat(const t_atom &a) { return IsFloat(a) || IsInt(a); }

#if FLEXT_SYS == FLEXT_SYS_JMAX
	//! Access the float value (without type check)
	static float GetFloat(const t_atom &a) { return fts_get_float(&a); }
	//! Set the atom to represent a float 
	static void SetFloat(t_atom &a,float v) { fts_set_float(&a,v); }

	//! Check whether the atom is a symbol
	static bool IsSymbol(const t_atom &a) { return fts_is_a(&a,fts_symbol_class); }
#else
	//! Access the float value (without type check)
	static float GetFloat(const t_atom &a) { return a.a_w.w_float; }
	//! Set the atom to represent a float 
	static void SetFloat(t_atom &a,float v) { a.a_type = A_FLOAT; a.a_w.w_float = v; }

	//! Check whether the atom is a symbol
	static bool IsSymbol(const t_atom &a) { return a.a_type == A_SYMBOL; }
#endif

#if FLEXT_SYS == FLEXT_SYS_PD
	//! Access the symbol value (without type check)
	static t_symbol *GetSymbol(const t_atom &a) { return a.a_w.w_symbol; }
	//! Set the atom to represent a symbol
	static void SetSymbol(t_atom &a,const t_symbol *s) { a.a_type = A_SYMBOL; a.a_w.w_symbol = const_cast<t_symbol *>(s); }
#elif FLEXT_SYS == FLEXT_SYS_MAX
	//! Access the symbol value (without type check)
	static t_symbol *GetSymbol(const t_atom &a) { return a.a_w.w_sym; }
	//! Set the atom to represent a symbol
	static void SetSymbol(t_atom &a,const t_symbol *s) { a.a_type = A_SYMBOL; a.a_w.w_sym = const_cast<t_symbol *>(s); }
#elif FLEXT_SYS == FLEXT_SYS_JMAX
	//! Access the symbol value (without type check)
	static t_symbol *GetSymbol(const t_atom &a); // ** TODO **
	//! Set the atom to represent a symbol
	static void SetSymbol(t_atom &a,const t_symbol *s) { fts_set_symbol(&a,s); }
#else
#error
#endif
	//! Check for a symbol and get its value 
	static t_symbol *GetASymbol(const t_atom &a,t_symbol *def = NULL) { return IsSymbol(a)?GetSymbol(a):def; }  // NULL or empty symbol?

	//! Check whether the atom is a string
	static bool IsString(const t_atom &a) { return IsSymbol(a); }
	//! Access the string value (without type check)
	static const char *GetString(const t_atom &a) { t_symbol *s = GetSymbol(a); return s?GetString(s):NULL; }  
	//! Check for a string and get its value 
	static void GetAString(const t_atom &a,char *buf,int szbuf);
	//! Set the atom to represent a string
	static void SetString(t_atom &a,const char *c) { SetSymbol(a,MakeSymbol(c)); }

	//! Check whether the atom can be represented as an integer
	static bool CanbeInt(const t_atom &a) { return IsFloat(a) || IsInt(a); }

	//! Set the atom to represent a boolean
	static void SetBool(t_atom &a,bool v) { SetInt(a,v?1:0); }
	//! Check whether the atom can be represented as a boolean
	static bool CanbeBool(const t_atom &a) { return CanbeInt(a); }
	//! Check for an boolean and get its value 
	static bool GetABool(const t_atom &a) { return GetAInt(a) != 0; }

#if FLEXT_SYS == FLEXT_SYS_PD
	//! Check for a float and get its value 
	static float GetAFloat(const t_atom &a,float def = 0) { return IsFloat(a)?GetFloat(a):def; }

	//! Check whether the atom is an integer
	static bool IsInt(const t_atom &) { return false; }
	//! Access the integer value (without type check)
	static int GetInt(const t_atom &a) { return (int)GetFloat(a); }
	//! Check for an integer and get its value 
	static int GetAInt(const t_atom &a,int def = 0) { return (int)GetAFloat(a,(float)def); }
	//! Set the atom to represent a integer (depending on the system)
	static void SetInt(t_atom &a,int v) { a.a_type = A_FLOAT; a.a_w.w_float = (float)v; }

	//! Check whether the atom strictly is a pointer
	static bool IsPointer(const t_atom &a) { return a.a_type == A_POINTER; }
	//! Check whether the atom can be a pointer
	static bool CanbePointer(const t_atom &a) { return IsPointer(a); }
	//! Access the pointer value (without type check)
	static void *GetPointer(const t_atom &a) { return a.a_w.w_gpointer; }
	//! Check for a pointer and get its value 
	static void *GetAPointer(const t_atom &a,void *def = NULL) { return IsPointer(a)?GetPointer(a):def; }
	//! Set the atom to represent a pointer
	static void SetPointer(t_atom &a,void *p) { a.a_type = A_POINTER; a.a_w.w_gpointer = (t_gpointer *)p; }

#elif FLEXT_SYS == FLEXT_SYS_MAX
	//! Check for a float and get its value 
	static float GetAFloat(const t_atom &a,float def = 0) { return IsFloat(a)?GetFloat(a):(IsInt(a)?GetInt(a):def); }

	//! Check whether the atom is an int
	static bool IsInt(const t_atom &a) { return a.a_type == A_INT; }
	//! Access the integer value (without type check)
	static int GetInt(const t_atom &a) { return a.a_w.w_long; }
	//! Check for an integer and get its value 
	static int GetAInt(const t_atom &a,int def = 0) { return IsInt(a)?GetInt(a):(IsFloat(a)?(int)GetFloat(a):def); }
	//! Set the atom to represent an integer
	static void SetInt(t_atom &a,int v) { a.a_type = A_INT; a.a_w.w_long = v; }

	//! Check whether the atom strictly is a pointer
	static bool IsPointer(const t_atom &) { return false; }
	//! Check whether the atom can be a pointer
	static bool CanbePointer(const t_atom &a) { return IsInt(a); }
	//! Access the pointer value (without type check)
	static void *GetPointer(const t_atom &) { return NULL; }
	//! Check for a pointer and get its value 
	static void *GetAPointer(const t_atom &a,void *def = NULL) { return IsInt(a)?(void *)GetInt(a):def; }
	//! Set the atom to represent a pointer
	static void SetPointer(t_atom &a,void *p) { SetInt(a,(int)p); }
#elif FLEXT_SYS == FLEXT_SYS_JMAX
	//! Check for a float and get its value 
	static float GetAFloat(const t_atom &a,float def = 0) { return IsFloat(a)?GetFloat(a):(IsInt(a)?GetInt(a):def); }

	//! Check whether the atom is an int
	static bool IsInt(const t_atom &a) { return fts_is_a(&a,fts_int_class); }
	//! Access the integer value (without type check)
	static int GetInt(const t_atom &a) { return fts_get_int(&a); }
	//! Check for an integer and get its value 
	static int GetAInt(const t_atom &a,int def = 0) { return IsInt(a)?GetInt(a):(IsFloat(a)?(int)GetFloat(a):def); }
	//! Set the atom to represent an integer
	static void SetInt(t_atom &a,int v) { fts_set_int(&a,v); }

	//! Check whether the atom strictly is a pointer
	static bool IsPointer(const t_atom &a) { return fts_is_a(&a,fts_pointer_class); }
	//! Check whether the atom can be a pointer
	static bool CanbePointer(const t_atom &a) { return IsPointer(a); }
	//! Access the pointer value (without type check)
	static void *GetPointer(const t_atom &a) { return fts_get_pointer(&a); }
	//! Check for a pointer and get its value 
	static void *GetAPointer(const t_atom &a,void *def = NULL) { return IsPointer(a)?GetPointer(a):def; }
	//! Set the atom to represent a pointer
	static void SetPointer(t_atom &a,void *p) { fts_set_pointer(&a,p); }
#else
#error "Platform not supported"
#endif

// --- atom list stuff -------------------------------------------

	//! Class representing a list of atoms
    class FLEXT_SHARE AtomList:
        public flext_root
	{
	public:
		//! Construct list
		AtomList(int argc = 0,const t_atom *argv = NULL);
		//! Construct list
		AtomList(const AtomList &a);
		//! Destroy list
		~AtomList();

		//! Clear list
		AtomList &Clear() { return operator()(); }

		//! Set list
		AtomList &Set(int argc,const t_atom *argv,int offs = 0,bool resize = false);
		//! Get list
		int Get(t_atom *argv,int mxsz = -1) const;

		//! Set list
		AtomList &operator()(int argc = 0,const t_atom *argv = NULL) { return Set(argc,argv,0,true); }
		//! Set list by another AtomList
		AtomList &operator =(const AtomList &a) { return operator()(a.Count(),a.Atoms()); }

		//! Compare list to another AtomList ( -1..< , 0..==, 1...> )
		int Compare(const AtomList &a) const;

		bool operator <(const AtomList &a) const { return Compare(a) < 0; }
		bool operator <=(const AtomList &a) const { return Compare(a) <= 0; }
		bool operator >(const AtomList &a) const { return Compare(a) > 0; }
		bool operator >=(const AtomList &a) const { return Compare(a) >= 0; }
		bool operator ==(const AtomList &a) const { return Compare(a) == 0; }
		bool operator !=(const AtomList &a) const { return Compare(a) != 0; }

		//! Get number of atoms in the list
		int Count() const { return cnt; }
		//! Get a reference to an indexed atom
		t_atom &operator [](int ix) { return lst[ix]; }
		//! Get a reference to an indexed atom
		const t_atom &operator [](int ix) const { return lst[ix]; }

		//! Get a pointer to the list of atoms
		t_atom *Atoms() { return lst; }
		//! Get a pointer to the list of atoms
		const t_atom *Atoms() const { return lst; }

		//! Append an atom to the list
		AtomList &Append(const t_atom &a);
		//! Append an atom list to the list
		AtomList &Append(int argc,const t_atom *argv = NULL);
		//! Append an atom list to the list
		AtomList &Append(const AtomList &a) { return Append(a.Count(),a.Atoms()); }
		//! Prepend an atom to the list
		AtomList &Prepend(const t_atom &a);
		//! Prepend an atom list to the list
		AtomList &Prepend(int argc,const t_atom *argv = NULL);
		//! Prepend an atom list to the list
		AtomList &Prepend(const AtomList &a) { return Prepend(a.Count(),a.Atoms()); }

		//! Get a part of the list
		AtomList GetPart(int offs,int len) const;
		//! Set to a part of the list
		AtomList &Part(int offs,int len) { return (*this = GetPart(offs,len)); }

		//! Represent as a string
		bool Print(char *buffer,int buflen) const;

	protected:
		int cnt;
		t_atom *lst;
	};


	//! Class representing an "anything"
	class FLEXT_SHARE AtomAnything: 
		public AtomList
	{
	public:
#if FLEXT_SYS != FLEXT_SYS_JMAX
		//! Construct anything
		AtomAnything(const t_symbol *h = NULL,int argc = 0,const t_atom *argv = NULL);
#endif
		//! Construct anything
		AtomAnything(const char *h,int argc = 0,const t_atom *argv = NULL);
		//! Construct anything
		AtomAnything(const AtomAnything &a);

		//! Clear anything
		AtomAnything &Clear() { return operator()(); }

		//! Get header symbol of anything
		const t_symbol *Header() const { return hdr; }

		//! Set header symbol of anything
		void Header(const t_symbol *h) { hdr = h; }
		
		//! Set anything
		AtomAnything &operator()(const t_symbol *h = NULL,int argc = 0,const t_atom *argv = NULL)
		{ 
			hdr = h; AtomList::operator()(argc,argv);	
			return *this; 
		}

		//! Set list by another AtomAnything
		AtomAnything &operator =(const AtomAnything &a) { return operator()(a.Header(),a.Count(),a.Atoms()); }

	protected:
		const t_symbol *hdr;
	};


//!		@} FLEXT_S_ATOM


// --- messages -------------------------------------------------------	

	/*!	\defgroup FLEXT_S_MSG Flext message handling 
		@{ 
	*/

    //! Send a message to a symbol (bound to an object)
    static bool Forward(const t_symbol *sym,const t_symbol *s,int argc,const t_atom *argv);

    static bool Forward(const t_symbol *sym,AtomAnything &args) { return Forward(sym,args.Header(),args.Count(),args.Atoms()); }
    static bool Forward(const char *sym,AtomAnything &args) { return Forward(MakeSymbol(sym),args.Header(),args.Count(),args.Atoms()); }

    static bool Forward(const t_symbol *sym,int argc,const t_atom *argv) { return Forward(sym,sym_list,argc,argv); }

    static bool Forward(const t_symbol *sym,AtomList &args) { return Forward(sym,args.Count(),args.Atoms()); }
    static bool Forward(const char *sym,AtomList &args) { return Forward(MakeSymbol(sym),args.Count(),args.Atoms()); }

//!		@} FLEXT_S_MSG

    

// --- thread stuff -----------------------------------------------

#ifdef FLEXT_THREADS
	/*!	\defgroup FLEXT_S_THREAD Flext thread handling 
		@{ 
	*/

	//! thread type
#if FLEXT_THREADS == FLEXT_THR_MP
	typedef MPTaskID thrid_t;
#elif FLEXT_THREADS == FLEXT_THR_POSIX
	typedef pthread_t thrid_t;
#elif FLEXT_THREADS == FLEXT_THR_WIN32
    typedef DWORD thrid_t;
#else
#error
#endif

	/*! \brief Get current thread id
	*/
	static thrid_t GetThreadId() { 
#if FLEXT_THREADS == FLEXT_THR_POSIX
		return pthread_self(); 
#elif FLEXT_THREADS == FLEXT_THR_MP
		return MPCurrentTaskID();
#elif FLEXT_THREADS == FLEXT_THR_WIN32
		return GetCurrentThreadId();
#else
#error
#endif
	}

	/*! \brief Get system thread id
	*/
	static thrid_t GetSysThreadId();
	
	//! Check if current thread is the realtime system's thread
	static bool IsThread(thrid_t t,thrid_t ref = GetThreadId()) { 
#if FLEXT_THREADS == FLEXT_THR_POSIX
		return pthread_equal(ref,t) != 0; 
#elif FLEXT_THREADS == FLEXT_THR_WIN32
        return ref == t; 
#else
		return ref == t;
#endif
	}

	//! Check if current thread is the realtime system's thread
	static bool IsSystemThread() { return IsThread(GetSysThreadId()); }


	/*! \brief Thread parameters
		\internal
	*/
    class FLEXT_SHARE thr_params:
        public flext_root
	{
	public:
		thr_params(int n = 1);
		~thr_params();

		void set_any(const t_symbol *s,int argc,const t_atom *argv);
		void set_list(int argc,const t_atom *argv);

		FLEXT_CLASSDEF(flext_base) *cl;
		union _data {
			bool _bool;
			float _float;
			int _int;
			t_symptr _t_symptr;
			struct { AtomAnything *args; } _any;
			struct { AtomList *args; } _list;
			struct { void *data; } _ext;
		} *var;
	};

	/*! \brief This represents an entry to the list of active method threads
		\internal
	*/
    class FLEXT_SHARE thr_entry:
        public flext_root
	{
	public:
		thr_entry(void (*m)(thr_params *),thr_params *p,thrid_t id = GetThreadId());

		//! \brief Check if this class represents the current thread
		bool Is(thrid_t id = GetThreadId()) const { return IsThread(thrid,id); }

		FLEXT_CLASSDEF(flext_base) *This() const { return th; }
		thrid_t Id() const { return thrid; }

		FLEXT_CLASSDEF(flext_base) *th;
		void (*meth)(thr_params *);
		thr_params *params;
		thrid_t thrid;
		bool active,shouldexit;
#if FLEXT_THREADS == FLEXT_THR_MP
		int weight;
#endif
		thr_entry *nxt;
	};

protected:

	static thrid_t thrhelpid;
	static thrid_t thrmsgid;
	static bool StartHelper();
	static bool StopHelper();
	static void ThrHelper(void *);
    static void LaunchHelper(thr_entry *e);

	//! the system's thread id
	static thrid_t thrid;  // the system thread

public:

	/*! \brief Yield to other threads
		\remark A call to this is only needed for systems with cooperative multitasking like MacOS<=9
	*/
	static void ThrYield() { 
#if FLEXT_THREADS == FLEXT_THR_POSIX
		// for a preemptive system this should do nothing
		sched_yield(); 
#elif FLEXT_THREADS == FLEXT_THR_MP
		MPYield();
#elif FLEXT_THREADS == FLEXT_THR_WIN32
        SwitchToThread();
#else
#error
#endif
	}

	/*! \brief Query whether task is preemptive
	*/
	static bool IsThreadPreemptive(thrid_t t = GetThreadId()) {
#if FLEXT_THREADS == FLEXT_THR_POSIX || FLEXT_THREADS == FLEXT_THR_WIN32
		return true;
#elif FLEXT_THREADS == FLEXT_THR_MP
		return MPTaskIsPreemptive(t);
#else
#error
#endif
	}
	

	/*! \brief Increase/Decrease priority of a thread
	*/
	static bool RelPriority(int dp,thrid_t ref = GetSysThreadId(),thrid_t thr = GetThreadId());

	/*! \brief Get priority of a thread
	*/
	static int GetPriority(thrid_t thr = GetThreadId());

	/*! \brief Set priority of a thread
	*/
	static bool SetPriority(int p,thrid_t thr = GetThreadId());

	/*! \brief Thread mutex
		\sa pthreads documentation
	*/
    class FLEXT_SHARE ThrMutex:
        public flext_root
#if FLEXT_THREADS == FLEXT_THR_POSIX
	{
	public:
		//! Construct thread mutex
		ThrMutex() { pthread_mutex_init(&mutex,NULL); }
		//! Destroy thread mutex
		~ThrMutex() { pthread_mutex_destroy(&mutex); }

		//! Lock thread mutex
		bool Lock() { return pthread_mutex_lock(&mutex) == 0; }
		/*! Wait to lock thread mutex.
			\todo Implement!
		*/
//		bool WaitForLock(double tm) { return pthread_mutex_lock(&mutex) == 0; }
		//! Try to lock, but don't wait
		bool TryLock() { return pthread_mutex_trylock(&mutex) == 0; }
		//! Unlock thread mutex
		bool Unlock() { return pthread_mutex_unlock(&mutex) == 0; }

	protected:
		pthread_mutex_t mutex;
//		int cnt;
	};
#elif FLEXT_THREADS == FLEXT_THR_WIN32
	{
	public:
		//! Construct thread mutex
        ThrMutex() { ::InitializeCriticalSection(&mutex); }
		//! Destroy thread mutex
        ~ThrMutex() { ::DeleteCriticalSection(&mutex); }

		//! Lock thread mutex
        bool Lock() { ::EnterCriticalSection(&mutex); return true; }
		/*! Wait to lock thread mutex.
			\todo Implement!
		*/
//		bool WaitForLock(double tm) { return pthread_mutex_lock(&mutex) == 0; }
		//! Try to lock, but don't wait
        bool TryLock() { return ::TryEnterCriticalSection(&mutex) != 0; }
		//! Unlock thread mutex
        bool Unlock() { ::LeaveCriticalSection(&mutex); return true; }

	protected:
		CRITICAL_SECTION mutex;
	};
#elif FLEXT_THREADS == FLEXT_THR_MP
	{
	public:
		//! Construct thread mutex
		ThrMutex() { MPCreateCriticalRegion(&crit); }
		//! Destroy thread mutex
		~ThrMutex() { MPDeleteCriticalRegion(crit); }

		//! Lock thread mutex
		bool Lock() { return MPEnterCriticalRegion(crit,kDurationForever) == noErr; }
		//! Wait to lock thread mutex
//		bool WaitForLock(double tm) { return MPEnterCriticalRegion(crit,tm*kDurationMicrosecond*1.e6) == noErr; }
		//! Try to lock, but don't wait
		bool TryLock() { return MPEnterCriticalRegion(crit,kDurationImmediate) == noErr; }
		//! Unlock thread mutex
		bool Unlock() { return MPExitCriticalRegion(crit) == noErr; }
		
	protected:
		MPCriticalRegionID crit;
	};
#else
#error "Not implemented"
#endif

	/*! \brief Thread conditional
		\sa pthreads documentation
	*/
	class FLEXT_SHARE ThrCond
#if FLEXT_THREADS == FLEXT_THR_POSIX
		:public ThrMutex
	{
	public:
		//! Construct thread conditional
		ThrCond() { pthread_cond_init(&cond,NULL); }
		//! Destroy thread conditional
		~ThrCond() { pthread_cond_destroy(&cond); }

		//! Wait for condition 
		bool Wait();

		/*! Wait for condition (for a certain time).
			\param ftime Wait time in seconds
			\ret true = signalled, false = timed out 
			\remark If ftime = 0 this may suck away your cpu if used in a signalled loop.
			\remark The time resolution of the implementation is required to be at least ms.
		*/
		bool TimedWait(double ftime);

		//! Signal condition
        bool Signal() { return pthread_cond_signal(&cond) == 0; }

	protected:
		pthread_cond_t cond;
	};
#elif FLEXT_THREADS == FLEXT_THR_WIN32
	{
	public:
		//! Construct thread conditional
		ThrCond() { cond = CreateEvent(NULL,FALSE,FALSE,NULL); }
		//! Destroy thread conditional
		~ThrCond() { CloseHandle(cond); }

		//! Wait for condition 
		bool Wait() { return WaitForSingleObject(cond,INFINITE) == WAIT_OBJECT_0; }

		/*! Wait for condition (for a certain time).
			\param ftime Wait time in seconds
			\ret true = signalled, false = timed out 
			\remark If ftime = 0 this may suck away your cpu if used in a signalled loop.
			\remark The time resolution of the implementation is required to be at least ms.
		*/
        bool TimedWait(double ftime) { return WaitForSingleObject(cond,(LONG)(ftime*1000)) == WAIT_OBJECT_0; }

		//! Signal condition
        bool Signal() { return SetEvent(cond) != 0; }

	protected:
		HANDLE cond;
	};
#elif FLEXT_THREADS == FLEXT_THR_MP
	{
	public:
		//! Construct thread conditional
		ThrCond() { MPCreateEvent(&ev); }
		//! Destroy thread conditional
		~ThrCond() { MPDeleteEvent(ev); }

		//! Wait for condition 
		bool Wait() { return MPWaitForEvent(ev,NULL,kDurationForever) == noErr; }

		/*! \brief Wait for condition (for a certain time).
			\param time Wait time in seconds
		*/
		bool TimedWait(double tm) { return MPWaitForEvent(ev,NULL,tm*kDurationMicrosecond*1.e6) == noErr; }

		//! Signal condition
		bool Signal() { return MPSetEvent(ev,1) == noErr; } // one bit needs to be set at least

	protected:
		MPEventID ev;
	};
#else
#error "Not implemented"
#endif

	/*! \brief Add current thread to list of active threads.
		\return true on success
		\internal
	*/
	static bool PushThread();

	/*! \brief Remove current thread from list of active threads.
		\internal
	*/
	static void PopThread();

	/*! \brief Launch a thread.
		\param meth Thread function
		\param params Parameters to pass to the thread, may be NULL if not needed.
		\return Thread id on success, NULL on failure
	*/
	static bool LaunchThread(void (*meth)(thr_params *p),thr_params *params = NULL);

	/*! \brief Terminate a thread.
		\param meth Thread function
		\param params Parameters to pass to the thread, may be NULL if not needed.
		\return True if at least one matching thread has been found.
		\remark Terminates all running threads with matching meth and params.
	*/
	static bool StopThread(void (*meth)(thr_params *p),thr_params *params = NULL,bool wait = false);

//!		@} FLEXT_S_THREAD


#endif // FLEXT_THREADS


// --- timer stuff -----------------------------------------------

/*!	\defgroup FLEXT_S_TIMER Flext timer handling 
		@{ 
		
	\remark The clock of the real-time system is used for most of these functions. 
	\remark Since this clock can be synchronized to an external clock (or e.g. the audio card) 
	\remark it may differ from the clock of the operating system
*/
	/*! \brief Get time since real-time system startup.
		\note This is not the time of the operating system but of the real-time system.
		\note It depends on the time source the system is synchronized to.
	*/
	static double GetTime();
	
	/*! \brief Get time granularity of the GetTime function.
		\note This can be zero if not determined.
	*/
	static double GetTimeGrain();

	/*! \brief Get operating system time since flext startup.
	*/
	static double GetOSTime();
	
	/*! \brief Sleep for an amount of time.
		\remark The OS clock is used for that.
		\note Clearly in a real-time system this should only be used in a detached thread.
	*/
	static void Sleep(double s);

	/*! \brief Class encapsulating a timer with callback functionality.
		This class can either be used with FLEXT_ADDTIMER or used as a base class with an overloaded virtual Work function.
	*/ 
    class FLEXT_SHARE Timer:
        public flext_root
	{
	public:
		Timer(bool queued = false);
		virtual ~Timer();

		//! Set timer callback function.
		void SetCallback(void (*cb)(void *data)) { clss = NULL,cback = cb; }
		//! Set timer callback function (with class pointer).
		void SetCallback(FLEXT_CLASSDEF(flext_base) &th,bool (*cb)(FLEXT_CLASSDEF(flext_base) *th,void *data)) { clss = &th,cback = (void (*)(void *))cb; }

		//! Clear timer.
		bool Reset();
		//! Trigger a one shot at an absolute time.
		bool At(double time,void *data = NULL,bool dopast = true);
		//! Trigger a one shot interval.
		bool Delay(double time,void *data = NULL);
		//! Trigger a periodic interval.
		bool Periodic(double time,void *data = NULL);
		//! Trigger immediately.
        bool Now(void *data = NULL) { return Delay(0,data); }

		//! Worker function, called on every timer event.
		virtual void Work();
		
	protected:
		static void callback(Timer *tmr);
	
#if FLEXT_SYS == FLEXT_SYS_PD
		t_clock *clk;
#elif FLEXT_SYS == FLEXT_SYS_MAX
		static void queuefun(Timer *tmr);
		t_clock *clk;
		t_qelem *qelem;
#else
#error Not implemented
#endif

		const bool queued;
		void (*cback)(void *data);
		FLEXT_CLASSDEF(flext_base) *clss;
		void *userdata;
		double period;
	};

//!		@} FLEXT_S_TIMER

// --- SIMD functionality -----------------------------------------------

/*!	\defgroup FLEXT_S_SIMD Cross platform SIMD support for modern CPUs 
		@{ 
*/		
		enum simd_type {
			simd_none = 0,
			simd_mmx = 0x01,
			simd_3dnow = 0x02,
			simd_sse = 0x04,
			simd_sse2 = 0x08,
			simd_altivec = 0x10
		};
		
		/*! Check for SIMD capabilities of the CPU */
		static unsigned long GetSIMDCapabilities();


        static void MulSamples(t_sample *dst,const t_sample *src,t_sample mul,int cnt);
        static void MulSamples(t_sample *dst,const t_sample *src,const t_sample *mul,int cnt);
        static void AddSamples(t_sample *dst,const t_sample *src,t_sample add,int cnt);
        static void AddSamples(t_sample *dst,const t_sample *src,const t_sample *add,int cnt);
        static void ScaleSamples(t_sample *dst,const t_sample *src,t_sample mul,t_sample add,int cnt);

//!		@} FLEXT_S_SIMD

		
//!		@} FLEXT_SUPPORT

protected:
#ifdef __MRC__
	friend class flext_obj;
#endif

	static void Setup();

	static bool chktilde(const char *objname);

	static unsigned long simdcaps;
};


// gcc doesn't like these to be included into the flext class (even if static)
inline bool operator ==(const t_atom &a,const t_atom &b) { return FLEXT_CLASSDEF(flext)::CmpAtom(a,b) == 0; }
inline bool operator !=(const t_atom &a,const t_atom &b) { return FLEXT_CLASSDEF(flext)::CmpAtom(a,b) != 0; }
inline bool operator <(const t_atom &a,const t_atom &b) { return FLEXT_CLASSDEF(flext)::CmpAtom(a,b) < 0; }
inline bool operator <=(const t_atom &a,const t_atom &b) { return FLEXT_CLASSDEF(flext)::CmpAtom(a,b) <= 0; }
inline bool operator >(const t_atom &a,const t_atom &b) { return FLEXT_CLASSDEF(flext)::CmpAtom(a,b) > 0; }
inline bool operator >=(const t_atom &a,const t_atom &b) { return FLEXT_CLASSDEF(flext)::CmpAtom(a,b) >= 0; }



#endif
