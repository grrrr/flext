/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flsupport.h
	\brief flext support functions and classes   
*/

#ifndef __FLSUPPORT_H
#define __FLSUPPORT_H

#include "flstdc.h"

class FLEXT_SHARE flext_base;

/*! \brief Flext support class
	A number of methods (most are static functions) are defined here for convenience.
	This class doesn't define any data members, hence it can be inherited to all
	classes (not only PD objects) to profit from the cross-platform functionality.
	Examples are the overloaded memory allocation, atom and atom list functions,
	thread functions and classes, the sample buffer class and others.
*/
class FLEXT_SHARE flext {

	/*!	\defgroup FLEXT_SUPPORT Flext support class
		@{ 
	*/
public:

// --- memory -------------------------------------------------------	

	/*!	\defgroup FLEXT_S_MEMORY Memory allocation functions
		@{ 
	*/

		/*! Overloaded new memory allocation method
			\warning Max/MSP (or MacOS) allows only 16K in overdrive mode!
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
		
	//!	@}  FLEXT_S_MEMORY  	

// --- buffer/array stuff -----------------------------------------	

	/*!	\defgroup FLEXT_S_BUFFER Buffer handling
		@{ 
	*/

	//! Class for platform independent buffer handling
	class FLEXT_SHARE buffer
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
		
		/*! \brief Check and update if the buffer has been changed (e.g. resized)
		*/
		bool Update();
		
		/*! \brief Set to specified buffer.
			\param nameonly: if true sets name only, but doesn't look at buffer actually
		*/
		int Set(const t_symbol *s = NULL,bool nameonly = false);
		
		/*! \brief Declare buffer content as dirty.
			\param refr: if true forces immediate graphics refresh
		*/
		void Dirty(bool refr = false);
		
		//! Get symbol of buffer 
		t_symbol *Symbol() const { return const_cast<t_symbol *>(sym); }

		//! Get literal name of buffer 
		const char *Name() const { return sym?sym->s_name:""; }
		
		/*! \brief Get pointer to buffer, channel and frame count.
			\remark Channels are interleaved
		*/
		t_sample *Data() { return data; }

		//! Get channel count
		int Channels() const { return chns; }
		//! Get frame count
		int Frames() const { return frames; }
		//! Set frame count
		void Frames(int fr,bool keep = false);
		
		//! Graphic auto refresh interval
		void SetRefrIntv(float intv);

	protected:
		const t_symbol *sym;
		t_sample *data;
		int chns,frames;
#if FLEXT_SYS == FLEXT_SYS_PD
		t_garray *arr;
		float interval;
		bool isdirty,ticking;
		t_clock *tick;

	private:
		static void cb_tick(buffer *b);
#endif
	};

//!		@} FLEXT_S_BUFFER

// --- utilities --------------------------------------------------

	/*!	\defgroup FLEXT_S_UTIL Utility functions
		@{ 
	*/

	//! Copy an atom
	static void CopyAtom(t_atom *dst,const t_atom *src) { *dst = *src; }

	//! Print an atom
	static void PrintAtom(const t_atom &a,char *buf);
	//! Scan an atom
	static bool ScanAtom(t_atom &a,const char *buf);

	//! Copy a list of atoms
	static t_atom *CopyList(int argc,const t_atom *argv);
	//! Copy a memory region
	static void CopyMem(void *dst,const void *src,int bytes);
	//! Zero a memory region
	static void ZeroMem(void *dst,int bytes);
	//! Sleep for an amount of time
	static void Sleep(double s);

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

	//! Make a symbol from a string
	static const t_symbol *MakeSymbol(const char *s) { return gensym(const_cast<char *>(s)); }

	//! Get symbol string
	static const char *GetString(const t_symbol *s) { return s->s_name; }  
	//! Check for symbol and get string
	static const char *GetAString(const t_symbol *s) { return s?s->s_name:""; }  

// --- atom stuff ----------------------------------------
		
	//! Set atom from another atom
	static int GetType(const t_atom &a) { return a.a_type; }

	//! Set atom from another atom
	static void SetAtom(t_atom &a,const t_atom &b) { CopyAtom(&a,&b); }

	//! Check whether the atom is nothing
	static bool IsNothing(const t_atom &a) { return a.a_type == A_NULL; }
	//! Set the atom to represent nothing
	static void SetNothing(t_atom &a) { a.a_type = A_NULL; }

	//! Check whether the atom is a float
	static bool IsFloat(const t_atom &a) { return a.a_type == A_FLOAT; }
	//! Check whether the atom can be represented as a float
	static bool CanbeFloat(const t_atom &a) { return IsFloat(a) || IsInt(a); }
	//! Access the float value (without type check)
	static float GetFloat(const t_atom &a) { return a.a_w.w_float; }
	//! Set the atom to represent a float 
	static void SetFloat(t_atom &a,float v) { a.a_type = A_FLOAT; a.a_w.w_float = v; }

	//! Check whether the atom is a symbol
	static bool IsSymbol(const t_atom &a) { return a.a_type == A_SYMBOL; }
#if FLEXT_SYS == FLEXT_SYS_PD
	//! Access the symbol value (without type check)
	static t_symbol *GetSymbol(const t_atom &a) { return a.a_w.w_symbol; }
	//! Set the atom to represent a symbol
	static void SetSymbol(t_atom &a,const t_symbol *s) { a.a_type = A_SYMBOL; a.a_w.w_symbol = const_cast<t_symbol *>(s); }
#else
	//! Access the symbol value (without type check)
	static t_symbol *GetSymbol(const t_atom &a) { return a.a_w.w_sym; }
	//! Set the atom to represent a symbol
	static void SetSymbol(t_atom &a,const t_symbol *s) { a.a_type = A_SYMBOL; a.a_w.w_sym = const_cast<t_symbol *>(s); }
#endif
	//! Check for a symbol and get its value 
	static t_symbol *GetASymbol(const t_atom &a) { return IsSymbol(a)?GetSymbol(a):NULL; }  // NULL or empty symbol?

	//! Check whether the atom is a string
	static bool IsString(const t_atom &a) { return IsSymbol(a); }
	//! Access the string value (without type check)
	static const char *GetString(const t_atom &a) { t_symbol *s = GetSymbol(a); return s?GetString(s):NULL; }  
	//! Check for a string and get its value 
	static void GetAString(const t_atom &a,char *buf,int szbuf);
	//! Set the atom to represent a string
	static void SetString(t_atom &a,const char *c) { SetSymbol(a,gensym(const_cast<char *>(c))); }

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
	static float GetAFloat(const t_atom &a) { return IsFloat(a)?GetFloat(a):0; }

	//! Check whether the atom is an integer
	static bool IsInt(const t_atom &) { return false; }
	//! Access the integer value (without type check)
	static int GetInt(const t_atom &a) { return (int)GetFloat(a); }
	//! Check for an integer and get its value 
	static int GetAInt(const t_atom &a) { return (int)GetAFloat(a); }
	//! Set the atom to represent a integer (depending on the system)
	static void SetInt(t_atom &a,int v) { a.a_type = A_FLOAT; a.a_w.w_float = (float)v; }

	//! Check whether the atom strictly is a pointer
	static bool IsPointer(const t_atom &a) { return a.a_type == A_POINTER; }
	//! Check whether the atom can be a pointer
	static bool CanbePointer(const t_atom &a) { return IsPointer(a); }
	//! Access the pointer value (without type check)
	static t_gpointer *GetPointer(const t_atom &a) { return a.a_w.w_gpointer; }
	//! Check for a pointer and get its value 
	static void *GetAPointer(const t_atom &a) { return IsPointer(a)?GetPointer(a):NULL; }
	//! Set the atom to represent a pointer
	static void SetPointer(t_atom &a,void *p) { a.a_type = A_POINTER; a.a_w.w_gpointer = (t_gpointer *)p; }

#elif FLEXT_SYS == FLEXT_SYS_MAX
	//! Check for a float and get its value 
	static float GetAFloat(const t_atom &a) { return IsFloat(a)?GetFloat(a):(IsInt(a)?GetInt(a):0); }

	//! Check whether the atom is an int
	static bool IsInt(const t_atom &a) { return a.a_type == A_INT; }
	//! Access the integer value (without type check)
	static int GetInt(const t_atom &a) { return a.a_w.w_long; }
	//! Check for an integer and get its value 
	static int GetAInt(const t_atom &a) { return IsInt(a)?GetInt(a):(IsFloat(a)?(int)GetFloat(a):0); }
	//! Set the atom to represent an integer
	static void SetInt(t_atom &a,int v) { a.a_type = A_INT; a.a_w.w_long = v; }

	//! Check whether the atom strictly is a pointer
	static bool IsPointer(const t_atom &) { return false; }
	//! Check whether the atom can be a pointer
	static bool CanbePointer(const t_atom &a) { return IsInt(a); }
	//! Check for a pointer and get its value 
	static void *GetAPointer(const t_atom &a) { return IsInt(a)?(void *)GetInt(a):NULL; }
	//! Set the atom to represent a pointer
	static void SetPointer(t_atom &a,void *p) { SetInt(a,(int)p); }
#else
#error
#endif

// --- atom list stuff -------------------------------------------

	//! Class representing a list of atoms
	class FLEXT_SHARE AtomList
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

	protected:
		int cnt;
		t_atom *lst;
	};


	//! Class representing an "anything"
	class FLEXT_SHARE AtomAnything: 
		public AtomList
	{
	public:
		//! Construct anything
		AtomAnything(const t_symbol *h = NULL,int argc = 0,const t_atom *argv = NULL);
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


// --- utilities ------------------------------------------------

	/*!	\defgroup FLEXT_S_UTIL Flext utility functions
		@{ 
	*/

	/*! \brief Fold value to a number of bits
		\remark Good for hash generation
	*/
	static int FoldBits(unsigned long h,int bits);
	
	//! \brief How many bits are necessary to represent n
	static int Int2Bits(unsigned long n);

//!		@} FLEXT_S_UTIL


// --- clock stuff ------------------------------------------------


	/*!	\defgroup FLEXT_S_CLOCK Flext clock functions

		At the moment there are none

		@{ 
	*/

//!		@} 


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
#else
#error
#endif
	}

	/*! \brief Get system thread id
	*/
	static thrid_t GetSysThreadId() { return thrid; }
	
	//! Check if current thread is the realtime system's thread
	static bool IsThread(thrid_t t,thrid_t ref = GetThreadId()) { 
#if FLEXT_THREADS == FLEXT_THR_POSIX
		return pthread_equal(ref,t) != 0; 
#else
		return ref == t;
#endif
	}

	//! Check if current thread is the realtime system's thread
	static bool IsSystemThread() { return IsThread(GetSysThreadId()); }


	/*! \brief Thread parameters
		\internal
	*/
	class thr_params
	{
	public:
		thr_params(int n = 1);
		~thr_params();

		void set_any(const t_symbol *s,int argc,const t_atom *argv);
		void set_list(int argc,const t_atom *argv);

		flext_base *cl;
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
	class thr_entry 
	{
	public:
		thr_entry(void (*m)(thr_params *),thr_params *p,thrid_t id = GetThreadId());

		//! \brief Check if this class represents the current thread
		bool Is(thrid_t id = GetThreadId()) const { return IsThread(thrid,id); }

		flext_base *This() const { return th; }
		thrid_t Id() const { return thrid; }

		flext_base *th;
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
	static bool StartHelper();
	static bool StopHelper();
	static void ThrHelper(void *);

	//! system's thread id
	static thrid_t thrid;  // the system thread

public:

	/*! \brief Yield to other threads
		\remark A call to this is only needed for systems with cooperative multitasking like MacOS<=9
	*/
	static void ThrYield() { 
#if FLEXT_THREADS == FLEXT_THR_POSIX
		sched_yield(); 
#elif FLEXT_THREADS == FLEXT_THR_MP
		MPYield();
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
	class FLEXT_SHARE ThrMutex 
#if FLEXT_THREADS == FLEXT_THR_POSIX
	{
	public:
		//! Construct thread mutex
		ThrMutex() /*: cnt(0)*/ { pthread_mutex_init(&mutex,NULL); }
		//! Destroy thread mutex
		~ThrMutex() { pthread_mutex_destroy(&mutex); }

		//! Lock thread mutex
		bool Lock() { /*cnt = 1;*/ return pthread_mutex_lock(&mutex) == 0; }
		//! Lock thread mutex
		bool WaitForLock(float tm) { /*cnt = 1;*/ return pthread_mutex_lock(&mutex) == 0; }
		//! Try to lock, but don't wait
		bool TryLock() { return pthread_mutex_trylock(&mutex) == 0; }
		//! Unlock thread mutex
		bool Unlock() { /*cnt = 0;*/ return pthread_mutex_unlock(&mutex) == 0; }
/*
		//! Lock thread mutex (increase lock count by one)
		void Push() { if(!cnt++) Lock(); }
		//! Unlock thread mutex if lock count reaches zero
		void Pop() { if(!--cnt) Unlock(); }
*/		
	protected:
		pthread_mutex_t mutex;
//		int cnt;
	};
#elif FLEXT_THREADS == FLEXT_THR_MP
	{
	public:
		//! Construct thread mutex
		ThrMutex() /*: cnt(0)*/ { MPCreateCriticalRegion(&crit); }
		//! Destroy thread mutex
		~ThrMutex() { MPDeleteCriticalRegion(crit); }

		//! Lock thread mutex
		bool Lock() { /*cnt = 1;*/ return MPEnterCriticalRegion(crit,kDurationForever) == noErr; }
		//! Wait to lock thread mutex
		bool WaitForLock(float tm) { /*cnt = 1;*/ return MPEnterCriticalRegion(crit,tm*kDurationMicrosecond*1.e6) == noErr; }
		//! Try to lock, but don't wait
		bool TryLock() { return MPEnterCriticalRegion(crit,kDurationImmediate) == noErr; }
		//! Unlock thread mutex
		bool Unlock() { /*cnt = 0;*/ return MPExitCriticalRegion(crit) == noErr; }
		
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
		bool Wait() { 
			Lock();
			bool ret = pthread_cond_wait(&cond,&mutex) == 0; 
			Unlock();
			return ret;
		}

		/*! \brief Wait for condition (for a certain time)
			\param time Wait time in seconds
		*/
		bool TimedWait(float time) 
		{ 
			timespec tm; tm.tv_sec = (long)time; tm.tv_nsec = (long)((time-(long)time)*1.e9);
			Lock();
			bool ret = pthread_cond_timedwait(&cond,&mutex,&tm) == 0; 
			Unlock();
			return ret;
		}

		//! Signal condition
		bool Signal() 
		{ 
			Lock();
			bool ret = pthread_cond_signal(&cond) == 0; 
			Unlock();
			return ret;
		}
		//! Broadcast condition
//		int Broadcast() { return pthread_cond_broadcast(&cond); }
	protected:
		pthread_cond_t cond;
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

		/*! \brief Wait for condition (for a certain time)
			\param time Wait time in seconds
		*/
		bool TimedWait(float tm) { return MPWaitForEvent(ev,NULL,tm*kDurationMicrosecond*1.e6) == noErr; }

		//! Signal condition
		bool Signal() { return MPSetEvent(ev,1) == noErr; } // one bit needs to be set at least
		//! Broadcast condition
//		int Broadcast() { return pthread_cond_broadcast(&cond); }
	protected:
		MPEventID ev;
	};
#else
#error "Not implemented"
#endif

	/*! \brief Add current thread to list of active threads
		\return true on success
		\internal
	*/
	static bool PushThread();

	/*! \brief Remove current thread from list of active threads
		\internal
	*/
	static void PopThread();

	/*! \brief Launch a thread
		\remark thr_params *p may be NULL if not needed
	*/
	static bool LaunchThread(void (*meth)(thr_params *p),thr_params *p);

//!		@} FLEXT_S_THREAD

#endif // FLEXT_THREADS

//!		@} 

protected:
#ifdef __MRC__
	friend class flext_obj;
#endif
	static void Setup();
};

#endif
