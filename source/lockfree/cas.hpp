//  $Id$
//
//  Copyright (C) 2007 Tim Blechmann & Thomas Grill
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING.  If not, write to
//  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//  Boston, MA 02111-1307, USA.

//  $Revision$
//  $LastChangedRevision$
//  $LastChangedDate$
//  $LastChangedBy$

#ifndef __LOCKFREE_CAS_H
#define __LOCKFREE_CAS_H

#include "prefix.hpp"

#ifndef _WIN32
// pthreads are not available under Windows by default and we should not need them there
#include <pthread.h>
#endif

namespace lockfree
{
    inline void memory_barrier()
    {
#if defined(__GNUC__) && ( (__GNUC__ > 4) || ((__GNUC__ >= 4) && (__GNUC_MINOR__ >= 1)) )
        __sync_synchronize();
#elif defined(_MSC_VER) && (_MSC_VER >= 1300)
        _ReadWriteBarrier();
#elif defined(__APPLE__)
        OSMemoryBarrier();
#elif defined(AO_HAVE_nop_full)
        AO_nop_full();
#else
#   error no memory barrier implemented for this platform
#endif
    }

    template <class C, class D>
    inline bool CAS(C * addr,D old,D nw)
    {
#if defined(__GNUC__) && ( (__GNUC__ > 4) || ((__GNUC__ >= 4) && (__GNUC_MINOR__ >= 1)) )
        return __sync_bool_compare_and_swap(addr, old, nw);
#elif defined(_MSC_VER)
        ASSERT((size_t(addr)&3) == 0);
        return _InterlockedCompareExchange(addr,nw,old) == old;
#elif defined(_WIN32)
        ASSERT((size_t(addr)&3) == 0);
        return InterlockedCompareExchange(addr,nw,old) == old;
#elif defined(__APPLE__)
        return OSAtomicCompareAndSwap32(old,nw,addr);
#elif defined(AO_HAVE_compare_and_swap_full)
        return AO_compare_and_swap_full(reinterpret_cast<volatile AO_t*>(addr),
            reinterpret_cast<AO_t>(old),
            reinterpret_cast<AO_t>(nw));
#else
#pragma message("blocking cas emulation")
        pthread_mutex_t guard = PTHREAD_MUTEX_INITIALIZER;
        int status = pthread_mutex_lock(&guard);
        assert(!status);

        bool ret;

        if (*addr == old)
        {
            *addr = nw;
            ret = true;
        }
        else
            ret = false;
        int status2 = pthread_mutex_unlock(&guard);
        assert(!status2);
        return ret;
#endif

    }


    template <class C, class D, class E>
    inline bool CAS2(C * addr,D old1,E old2,D new1,E new2)
    {
#if defined(__GNUC__) && ((__GNUC__ >  4) || ( (__GNUC__ >= 4) && (__GNUC_MINOR__ >= 2) ) ) && defined(__i386__)
        struct packed_c
        {
            D d;
            E e;
        };

        union cu
        {
            packed_c c;
            long long l;
        };

        cu old;
        old.c.d = old1;
        old.c.e = old2;

        cu nw;
        nw.c.d = new1;
        nw.c.e = new2;

        return __sync_bool_compare_and_swap_8(reinterpret_cast<volatile long long*>(addr),
            old.l,
            nw.l);
#elif defined(_MSC_VER)
        bool ok;
        __asm {
            mov eax,[old1]
            mov edx,[old2]
            mov ebx,[new1]
            mov ecx,[new2]
            mov edi,[addr]
            lock cmpxchg8b [edi]
            setz [ok]
        }
        return ok;
#elif defined(__GNUC__) && defined(__i386__)
        char result;
#ifndef __PIC__
        __asm__ __volatile__("lock; cmpxchg8b %0; setz %1"
                             : "=m"(*addr), "=q"(result)
                             : "m"(*addr), "a" (old1), "d" (old2),
                             "b" (new1), "c" (new2) : "memory");
#else
        __asm__ __volatile__("push %%ebx; movl %5,%%ebx; lock; cmpxchg8b %0; setz %1; pop %%ebx"
                             : "=m"(*addr), "=q"(result)
                             : "m"(*addr), "a" (old1), "d" (old2),
                             "m" (new1), "c" (new2) : "memory");
#endif
        return result != 0;
#elif defined(AO_HAVE_double_compare_and_swap_full)
        if (sizeof(D) != sizeof(AO_t) || sizeof(E) != sizeof(AO_t)) {
            assert(false);
            return false;
        }

        return AO_compare_double_and_swap_double_full(
            reinterpret_cast<volatile AO_double_t*>(addr),
            static_cast<AO_t>(old2),
            reinterpret_cast<AO_t>(old1),
            static_cast<AO_t>(new2),
            reinterpret_cast<AO_t>(new1)
        );
#elif defined(__GNUC__) && ((__GNUC__ >  4) || ( (__GNUC__ >= 4) && (__GNUC_MINOR__ >= 2) ) ) && defined(__x86_64__)
        struct packed_c
        {
            D d;
            E e;
        };

        union cu
        {
            packed_c c;
            long long l;
        };

        cu old;
        old.c.d = old1;
        old.c.e = old2;

        cu nw;
        nw.c.d = new1;
        nw.c.e = new2;

        return __sync_bool_compare_and_swap/* _16 */(reinterpret_cast<volatile long long*>(addr),
            old.l,
            nw.l);
#else
#pragma message("blocking CAS2 emulation")
        struct packed_c
        {
            D d;
            E e;
        };

        volatile packed_c * packed_addr = reinterpret_cast<volatile packed_c*>(addr);

        pthread_mutex_t guard = PTHREAD_MUTEX_INITIALIZER;
        int status = pthread_mutex_lock(&guard);
        assert(!status);

        bool ret;

        if (packed_addr->d == old1 &&
            packed_addr->e == old2)
        {
            packed_addr->d = new1;
            packed_addr->e = new2;
            ret = true;
        }
        else
            ret = false;
        int status2 = pthread_mutex_unlock(&guard);
        assert(!status2);
        return ret;
#endif
    }

} // namespace

#endif /* __LOCKFREE_CAS_H */
