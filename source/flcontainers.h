/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2005 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flcontainers.h
	\brief Lock-free container classes
   
	This code has been adapted from the MidiShare project (c)Grame
*/

#ifndef __FLCONTAINERS_H
#define __FLCONTAINERS_H


#include "flprefix.h"


#if 1 //def __Pentium__
#define VTYPE volatile
#else
#define VTYPE
#endif

#define __SMP__


class FLEXT_SHARE Lifo 
{
public:
    class Cell 
    {
	    friend class Lifo;
	    friend class Fifo;
    private:
	    Cell *link;
    };

    inline Lifo() { Init(); }

    inline void Init() { ic = oc = 0; top = NULL; }

    inline Cell *Avail() { return (Cell *)top; }

#if defined(_WIN32) && defined(_MSC_VER)
    #ifdef __SMP__
    #define LOCK lock
    #else
    #define LOCK
    #endif

    inline void Push(Cell *cell) 
    {
	    __asm 
	    {
		    push	eax
		    push	ebx
		    push	ecx
		    push	edx
		    push	esi
		    mov		esi, this
		    mov		eax, dword ptr [esi]
		    mov		ecx, cell
		    mov		edx, dword ptr [esi+4]
	    _loop:
		    mov		ebx, eax
		    inc		ebx
		    mov		[ecx], edx
		    LOCK cmpxchg8b qword ptr [esi]
		    jnz		_loop
		    pop		esi
		    pop		edx
		    pop		ecx
		    pop		ebx
		    pop		eax
	    }
    }

    inline Cell *Pop() 
    {
	    __asm 
	    {
		    push	ebx
		    push	ecx
		    push	edx
		    push	esi
		    mov		esi, this
		    add		esi, 4 /* point to top */
		    mov 	edx, dword ptr [esi+4]
		    mov  	eax, dword ptr [esi]	
		    test	eax, eax
		    jz		_end
	    _loop:
		    mov		ebx, dword ptr [eax]
		    mov		ecx, edx
		    inc		ecx
		    LOCK cmpxchg8b qword ptr [esi]
		    jz		_end
		    test	eax, eax
		    jnz		_loop
	    _end:
		    pop		esi
		    pop		edx
		    pop		ecx
		    pop		ebx
	    }
    }

    inline size_t Size() const { return ic-oc; }
#elif defined(__GNUC__) && (defined(_X86_) || defined(__i386__) || defined(__i586__) || defined(__i686__))
    #ifndef SMPLOCK
    # ifdef __SMP__
    #  define SMPLOCK "lock ; "
    # else
    #  define SMPLOCK ""
    # endif
    #endif

    inline void Push(Cell *cl) 
    {
	    __asm__ __volatile__ (
		    "# LFPUSH					\n\t"
		    "pushl	%%ebx				\n\t"
		    "pushl	%%ecx				\n\t"
		    "movl 0(%%esi), %%eax		\n\t"
		    "movl 4(%%esi), %%edx		\n"	
		    "1:                         \t"
		    "movl %%eax, %%ebx			\n\t"
		    "incl %%ebx					\n\t"
		    "movl %%edx, (%%ecx)		\n\t"
		    SMPLOCK "cmpxchg8b (%%esi)	\n\t"
		    "jnz	1b					\n\t"
		    "popl	%%ecx				\n\t"
		    "popl	%%ebx				\n\t"
		    :/* no output */
		    :"S" (this), "c" (cl)
		    :"memory", "eax", "edx");
    }

    inline Cell *Pop() 
    {
	    Cell *v=0;
	    __asm__ __volatile__ (
		    "# LFPOP 					\n\t"
		    "pushl	%%ebx				\n\t"
		    "pushl	%%ecx				\n\t"
		    "movl 	4(%%esi), %%edx		\n\t"
		    "movl  	(%%esi), %%eax		\n\t"	
		    "testl	%%eax, %%eax		\n\t"
		    "jz		20f					\n"
		    "10:                        \t"
		    "movl 	(%%eax), %%ebx		\n\t"
		    "movl	%%edx, %%ecx		\n\t"
		    "incl	%%ecx				\n\t"
		    SMPLOCK "cmpxchg8b (%%esi)	\n\t"
		    "jz		20f					\n\t"
		    "testl	%%eax, %%eax		\n\t"
		    "jnz	10b					\n"
		    "20:                        \t"
		    "popl	%%ecx				\n\t"
		    "popl	%%ebx				\n\t"
		    :"=a" (v)
		    :"S" (&this->top)
		    :"memory", "edx");
	    return v;
    }

    inline size_t Size() const 
    {
	    size_t n;
	    __asm__ __volatile__ (
		    "# LFSIZE					\n\t"
		    "movl 	8(%%esi), %%edx		\n\t"
		    "movl  	(%%esi), %%eax		\n\t"	
		    "subl 	%%edx, %%eax		\n\t"
		    :"=a" (n)
		    :"S" (this)
		    :"memory", "edx");
	    return n;
    }
#elif defined(__GNUC__) && defined(__POWERPC__)
    inline void Push(register Cell *cl) 
    {
        register volatile long t1;
        register long t2=0;
        asm volatile (
	        "# LFPUSH \n"
	        "0: 				      \n"
	        "   lwarx   %0, %3, %1  \n"		
	        "   stw	  %0, 0(%2)   \n"	
	        "   sync  			  \n"	
	        "   stwcx.  %2, %3, %1  \n"						   
	        "   bne-    0b	      \n"  
	        "0:				      \n"
	        "   lwarx   %0, %3, %4  \n"		
	        "   addi    %0, %0, 1	  \n"  
	        "   sync  			  \n"  
	        "   stwcx.  %0, %3, %4  \n"
	        "   bne-    0b		  \n"
	        : "=r" (t1)
	        : "r" (&this->top), "r" (cl), "r" (t2), "r" (&this->oc), "0" (t1)
	        : "r0" 		/* prevents using r0 because of the ambiguity of 'addi' coding: */
	  				        /* gcc version 2.95.3 20010315 (release - Linux-Mandrake 8.0 for PPC) */
					        /* compiles the instruction "addi 0, 0, n" as li 0, n */
        );
    }

    inline Cell *Pop() 
    {
	    register Cell *result;
	    register volatile long a, b;
	    register long c=0;
	    asm volatile (
        "# LFPOP					\n"
            "0:						\n"
		    "	lwarx	%4, %1, %2	\n"         /* creates a reservation on lf    */
		    "	cmpwi	%4, 0		\n"         /* test if the lifo is empty      */
		    "	beq-	1f		\n"
		    "	lwz		%5, 0(%4)	\n"         /* next cell in b                */
            "	sync            	\n"         /* synchronize instructions       */
		    "	stwcx.	%5, %1, %2	\n"         /* if the reservation is not altered */
                                                /* modify lifo top                */
		    "	bne-	0b  		\n"         /* otherwise: loop and try again  */
            "0:						\n"
		    "	lwarx	%5, %1, %3	\n"         /* creates a reservation on lf->count */
            "	addi	%5, %5, -1	\n"         /* dec count                      */
		    "	sync            	\n"         /* synchronize instructions       */
		    "	stwcx.	%5, %1, %3	\n"         /* conditionnal store             */
		    "	bne-	0b			\n"
            "1:						\n"
		    "	mr		%0, %4		\n"
        :"=r" (result), "=r" (c)
	    : "r" (&this->top), "r" (&this->oc), "r" (a), "r" (b), "1" (c)
	    : "r0" 		/* prevents using r0 because of the ambiguity of 'addi' coding: */
	  				    /* gcc version 2.95.3 20010315 (release - Linux-Mandrake 8.0 for PPC) */
					    /* compiles the instruction "addi 0, 0, n" as li 0, n */
	    );
	    return result;
    }

    inline size_t Size() const { return oc; }
#endif

private:
    // don't change order!
    VTYPE size_t ic;		// input (push) count
	VTYPE Cell *top;	    // top of the stack
	VTYPE size_t oc;		// output (pop) count
#ifdef __POWERPC__
	size_t unused[5];		// lifo size must be at least 32 bytes
							// to avoid livelock in multiprocessor
#endif
};

template <typename T>
class TypedLifo
    : public Lifo
{
public:
    inline T *Avail() { return static_cast<T *>(Lifo::Avail()); }
    inline void Push(T *c) { Lifo::Push(static_cast<T *>(c)); }
    inline T *Pop() { return static_cast<T *>(Lifo::Pop()); }
};


class FLEXT_SHARE Fifo 
{
public:
    typedef Lifo::Cell Cell;

    void Init() { in.Init(); out.Init(); }

    inline size_t Size() const { return in.Size()+out.Size(); }

    inline void Put(Cell *cl) { in.Push(cl); }

    Cell *Get() 
    {
        Cell *v1 = out.Pop();
        if(!v1) {
            v1 = in.Pop();
            if(v1)
                for(Cell *v2; (v2 = in.Pop()) != NULL; v1 = v2) 
                    out.Push(v1);
        }
        return v1;
    }

    Cell *Avail()
    {
        Cell *v1 = out.Avail();
        if(v1)
            return v1;
        else {
            for(Cell *v2; (v2 = in.Pop()) != NULL; ) 
                out.Push(v2);
            return out.Avail();
        }
    }

    Cell *Clear()
    {
        Cell *first = Get();
        if(!first) return NULL;

        Cell *next,*cur = first;
        while((next = Get()) != NULL) {
            cur->link = next;
            cur = next;
        }
        cur->link = NULL;

        Init();
        return first;
    }

	Lifo in,out;
};

template <typename T>
class TypedFifo
    : public Fifo
{
public:
    inline T *Avail() { return static_cast<T *>(Fifo::Avail()); }
    inline void Put(T *c) { Fifo::Put(static_cast<T *>(c)); }
    inline T *Get() { return static_cast<T *>(Fifo::Get()); }
    inline T *Clear() { return static_cast<T *>(Fifo::Clear()); }
};


#endif
