/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flsimd.cpp
	\brief flext SIMD support functions    
*/

#include "flext.h"
#include <string.h>

#if FLEXT_OS == FLEXT_OS_WIN
#include <windows.h>
#endif

#if FLEXT_CPU == FLEXT_CPU_PPC && defined(__MWERKS__)
#include "Altivec.h"
#endif

#ifdef FLEXT_USE_IPP
#include <ipps.h>
#endif

static unsigned long setsimdcaps();

/*! \brief Holds SIMD capability flags
	\internal
*/
unsigned long flext::simdcaps = setsimdcaps();


#if FLEXT_CPU == FLEXT_CPU_INTEL

#define _CPU_FEATURE_MMX    0x0001
#define _CPU_FEATURE_SSE    0x0002
#define _CPU_FEATURE_SSE2   0x0004
#define _CPU_FEATURE_3DNOW  0x0008

typedef struct _processor_info {
    int family;                         // family of the processor
                                        // e.g. 6 = Pentium-Pro architecture
    int model;                          // model of processor
                                        // e.g. 1 = Pentium-Pro for family = 6
    int stepping;                       // processor revision number
    int feature;                        // processor feature
                                        // (same as return value from _cpuid)
    int os_support;                     // does OS Support the feature?
    int checks;                         // mask of checked bits in feature
                                        // and os_support fields
} _p_info;

// These are the bit flags that get set on calling cpuid
// with register eax set to 1
#define _MMX_FEATURE_BIT        0x00800000
#define _SSE_FEATURE_BIT        0x02000000
#define _SSE2_FEATURE_BIT       0x04000000

// This bit is set when cpuid is called with
// register set to 80000001h (only applicable to AMD)
#define _3DNOW_FEATURE_BIT      0x80000000

#ifdef _MSC_VER
static int IsCPUID()
{
    __try {
        _asm {
            xor eax, eax
            cpuid
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return 0;
    }
    return 1;
}

static int _os_support(int feature)
{
    __try {
        switch (feature) {
        case _CPU_FEATURE_SSE:
            __asm {
                xorps xmm0, xmm0        // executing SSE instruction
            }
            break;
        case _CPU_FEATURE_SSE2:
            __asm {
                xorpd xmm0, xmm0        // executing SSE2 instruction
            }
            break;
        case _CPU_FEATURE_3DNOW:
            __asm {
                pfrcp mm0, mm0          // executing 3DNow! instruction
                emms
            }
            break;
        case _CPU_FEATURE_MMX:
            __asm {
                pxor mm0, mm0           // executing MMX instruction
                emms
            }
            break;
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        if (_exception_code() == STATUS_ILLEGAL_INSTRUCTION) {
            return 0;
        }
        return 0;
    }
    return 1;
}

static int _cpuid (_p_info *pinfo)
{
    DWORD dwStandard = 0;
    DWORD dwFeature = 0;
    DWORD dwMax = 0;
    DWORD dwExt = 0;
    int feature = 0;
    int os_support = 0;
    union {
        struct {
            DWORD dw0;
            DWORD dw1;
            DWORD dw2;
        } s;
    } Ident;

    if (!IsCPUID()) {
        return 0;
    }

    _asm {
        push ebx
        push ecx
        push edx

        // get the vendor string
        xor eax, eax
        cpuid
        mov dwMax, eax
        mov Ident.s.dw0, ebx
        mov Ident.s.dw1, edx
        mov Ident.s.dw2, ecx

        // get the Standard bits
        mov eax, 1
        cpuid
        mov dwStandard, eax
        mov dwFeature, edx

        // get AMD-specials
        mov eax, 80000000h
        cpuid
        cmp eax, 80000000h
        jc notamd
        mov eax, 80000001h
        cpuid
        mov dwExt, edx

notamd:
        pop ecx
        pop ebx
        pop edx
    }

    if (dwFeature & _MMX_FEATURE_BIT) {
        feature |= _CPU_FEATURE_MMX;
        if (_os_support(_CPU_FEATURE_MMX))
            os_support |= _CPU_FEATURE_MMX;
    }
    if (dwExt & _3DNOW_FEATURE_BIT) {
        feature |= _CPU_FEATURE_3DNOW;
        if (_os_support(_CPU_FEATURE_3DNOW))
            os_support |= _CPU_FEATURE_3DNOW;
    }
    if (dwFeature & _SSE_FEATURE_BIT) {
        feature |= _CPU_FEATURE_SSE;
        if (_os_support(_CPU_FEATURE_SSE))
            os_support |= _CPU_FEATURE_SSE;
    }
    if (dwFeature & _SSE2_FEATURE_BIT) {
        feature |= _CPU_FEATURE_SSE2;
        if (_os_support(_CPU_FEATURE_SSE2))
            os_support |= _CPU_FEATURE_SSE2;
    }

    if (pinfo) {
        memset(pinfo, 0, sizeof(_p_info));

        pinfo->os_support = os_support;
        pinfo->feature = feature;
        pinfo->family = (dwStandard >> 8) & 0xF; // retrieve family
        if (pinfo->family == 15) {               // retrieve extended family
            pinfo->family |= (dwStandard >> 16) & 0xFF0;
        }
        pinfo->model = (dwStandard >> 4) & 0xF;  // retrieve model
        if (pinfo->model == 15) {                // retrieve extended model
            pinfo->model |= (dwStandard >> 12) & 0xF;
        }
        pinfo->stepping = (dwStandard) & 0xF;    // retrieve stepping

        pinfo->checks = _CPU_FEATURE_MMX |
                        _CPU_FEATURE_SSE |
                        _CPU_FEATURE_SSE2 |
                        _CPU_FEATURE_3DNOW;
    }

    return feature;
}
#else
// not MSVC
static int _cpuid (_p_info *pinfo)
{
    if(pinfo) memset(pinfo,0,sizeof *pinfo);
    return 0;
}
#endif

#endif


/*! \brief Determine SIMD capabilities
	\internal
*/
static unsigned long setsimdcaps()
{
    unsigned long simdflags = flext::simd_none;
#if FLEXT_CPU == FLEXT_CPU_INTEL 
    _p_info cpuinfo;
    int feature = _cpuid(&cpuinfo);
    if(cpuinfo.os_support&_CPU_FEATURE_MMX) simdflags += flext::simd_mmx;
    if(cpuinfo.os_support&_CPU_FEATURE_3DNOW) simdflags += flext::simd_3dnow;
    if(cpuinfo.os_support&_CPU_FEATURE_SSE) simdflags += flext::simd_sse;
    if(cpuinfo.os_support&_CPU_FEATURE_SSE2) simdflags += flext::simd_sse2;
#endif
    return simdflags;
}


void flext::CopySamples(t_sample *dst,const t_sample *src,int cnt) 
{
#ifdef FLEXT_USE_IPP
    if(sizeof(t_sample) == 4)
        ippsCopy_32f((const float *)src,(float *)dst,cnt); 
    else if(sizeof(t_sample) == 8)
        ippsCopy_64f((const double *)src,(double *)dst,cnt); 
    else
        ERRINTERNAL();
#else
	int n = cnt>>3;
	cnt -= n<<3;
	while(n--) {
		dst[0] = src[0]; dst[1] = src[1];
		dst[2] = src[2]; dst[3] = src[3];
		dst[4] = src[4]; dst[5] = src[5];
		dst[6] = src[6]; dst[7] = src[7];
		src += 8,dst += 8;
	}
	
	while(cnt--) *(dst++) = *(src++); 
#endif
}

void flext::SetSamples(t_sample *dst,int cnt,t_sample s) 
{
#ifdef FLEXT_USE_IPP
    if(sizeof(t_sample) == 4)
        ippsSet_32f((float)s,(float *)dst,cnt); 
    else if(sizeof(t_sample) == 8)
        ippsSet_64f((double)s,(double *)dst,cnt); 
    else
        ERRINTERNAL();
#else
	int n = cnt>>3;
	cnt -= n<<3;
	while(n--) {
		dst[0] = dst[1] = dst[2] = dst[3] = dst[4] = dst[5] = dst[6] = dst[7] = s;
		dst += 8;
	}
	
	while(cnt--) *(dst++) = s; 
#endif
}
