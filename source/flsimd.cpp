/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flsimd.cpp
	\brief flext SIMD support functions

    If FLEXT_USE_SIMD is defined at compilation, SIMD instructions are used wherever feasible.
    If used with MSVC++ the "Processor Pack" must be installed.

    If FLEXT_USE_IPP is defined the Intel Performance Package is used.
*/

#include "flext.h"
#include <string.h>

#if FLEXT_OS == FLEXT_OS_WIN
#include <windows.h>
#endif

#ifdef FLEXT_USE_IPP
#include <ipps.h>
#endif

#ifdef FLEXT_USE_SIMD
    #ifdef _MSC_VER
        // include MSVC SIMD header files
        #include <mmintrin.h> // MMX
        #include <xmmintrin.h> // SSE
        #include <emmintrin.h> // SSE2
        #include <mm3dnow.h> // 3DNow!
    #elif FLEXT_CPU == FLEXT_CPU_PPC && defined(__MWERKS__) && defined(__ALTIVEC__)
   		#if FLEXT_OSAPI == FLEXT_OSAPI_MAC_MACH
			 #include <sys/sysctl.h> 
    	#else
			 #include <Gestalt.h> 
   		#endif
    
		#pragma altivec_model on

    	#include <altivec.h>
//		#include <vBasicOps.h>
		#include <vectorOps.h>
    #elif FLEXT_CPU == FLEXT_CPU_PPC && defined(__GNUG__) && defined(__ALTIVEC__)
		#include <sys/sysctl.h> 
//		#include <vecLib/vBasicOps.h>
		#include <vecLib/vectorOps.h>
    #endif

#endif // FLEXT_USE_SIMD

static unsigned long setsimdcaps();

/*! \brief Holds SIMD capability flags
	\internal
*/
unsigned long flext::simdcaps = setsimdcaps();

unsigned long flext::GetSIMDCapabilities() { return simdcaps; }


#ifdef FLEXT_USE_SIMD

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
#elif FLEXT_CPU == FLEXT_CPU_PPC 

    #if FLEXT_OSAPI == FLEXT_OSAPI_MAC_MACH

	int selectors[2] = { CTL_HW, HW_VECTORUNIT }; 
	int hasVectorUnit = 0; 
	size_t length = sizeof(hasVectorUnit); 
	int error = sysctl(selectors, 2, &hasVectorUnit, &length, NULL, 0); 

	if(!error && hasVectorUnit != 0) simdflags += flext::simd_altivec; 
		
	#else

	long cpuAttributes; 
	Boolean hasAltiVec = false; 
	OSErr err = Gestalt( gestaltPowerPCProcessorFeatures, &cpuAttributes ); 

	if( noErr == err ) 
	if(( 1 << gestaltPowerPCHasVectorInstructions) & cpuAttributes) simdflags += flext::simd_altivec; 

	#endif
#endif
    return simdflags;
}


#if FLEXT_CPU == FLEXT_CPU_PPC && defined(__ALTIVEC__)

/* functions for misaligned vector data - taken from the Altivec tutorial of Ian Ollmann, Ph.D. */

//! Load a vector from an unaligned location in memory
inline vector unsigned char LoadUnaligned( vector unsigned char *v )
{
	vector unsigned char permuteVector = vec_lvsl( 0, (int*) v );
	vector unsigned char low = vec_ld( 0, v );
	vector unsigned char high = vec_ld( 16, v );
	return vec_perm( low, high, permuteVector );
}

//! Store a vector to an unaligned location in memory
inline void StoreUnaligned( vector unsigned char v, vector unsigned char *where)
{
	// Load the surrounding area
	vector unsigned char low = vec_ld( 0, where );
	vector unsigned char high = vec_ld( 16, where );
	// Prepare the constants that we need
	vector unsigned char permuteVector = vec_lvsr( 0, (int*) where );

	vector unsigned char oxFF = (vector unsigned char)vec_splat_s8( -1 );
	vector unsigned char ox00 = (vector unsigned char)vec_splat_s8( 0 );
	// Make a mask for which parts of the vectors to swap out
	vector unsigned char mask = vec_perm( ox00, oxFF, permuteVector );
	// Right rotate our input data
	v = vec_perm( v, v, permuteVector );
	// Insert our data into the low and high vectors
	low = vec_sel( v, low, mask );
	high = vec_sel( high, v, mask );
	// Store the two aligned result vectors
	vec_st( low, 0, where );
	vec_st( high, 16, where );
}

inline vector float LoadUnaligned(float *v )
{
	return (vector float)LoadUnaligned((vector unsigned char *)v);
}

inline void StoreUnaligned( vector float v,float *where)
{
	return StoreUnaligned((vector unsigned char)v,(vector unsigned char *)where);
}

inline bool IsVectorAligned(const void *where) 
{
	return reinterpret_cast<unsigned long>(where)&(sizeof(vector float)-1) == 0; 
}
#endif


#else // FLEXT_USE_SIMD
static unsigned long setsimdcaps() { return 0; }
#endif // FLEXT_USE_SIMD


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
#ifdef FLEXT_USE_SIMD
#ifdef _MSC_VER
    if(GetSIMDCapabilities()&simd_sse) {
        // single precision

		__asm {
			mov		eax,dword ptr [src]
			prefetcht0 [eax+0]
			prefetcht0 [eax+32]
		}

   	    int n = cnt>>4;
        cnt -= n<<4;

		if((reinterpret_cast<unsigned long>(src)&(__alignof(__m128)-1)) == 0) {
			if((reinterpret_cast<unsigned long>(dst)&(__alignof(__m128)-1)) == 0) {
				// aligned src, aligned dst
				__asm {
					mov		eax,dword ptr [src]
					mov		edx,dword ptr [dst]
					mov		ecx,[n]
loopaa:
					prefetcht0 [eax+64]
					prefetcht0 [eax+96]
					movaps	xmm0,xmmword ptr[eax]
					movaps	xmmword ptr[edx],xmm0
					movaps	xmm1,xmmword ptr[eax+4*4]
					movaps	xmmword ptr[edx+4*4],xmm1
					movaps	xmm2,xmmword ptr[eax+8*4]
					movaps	xmmword ptr[edx+8*4],xmm2
					movaps	xmm3,xmmword ptr[eax+12*4]
					movaps	xmmword ptr[edx+12*4],xmm3

					add		eax,16*4
					add		edx,16*4
					loop	loopaa
				}
			}
			else {
				// aligned src, unaligned dst
				__asm {
					mov		eax,dword ptr [src]
					mov		edx,dword ptr [dst]
					mov		ecx,[n]
loopau:
					prefetcht0 [eax+64]
					prefetcht0 [eax+96]
					movaps	xmm0,xmmword ptr[eax]
					movups	xmmword ptr[edx],xmm0
					movaps	xmm1,xmmword ptr[eax+4*4]
					movups	xmmword ptr[edx+4*4],xmm1
					movaps	xmm2,xmmword ptr[eax+8*4]
					movups	xmmword ptr[edx+8*4],xmm2
					movaps	xmm3,xmmword ptr[eax+12*4]
					movups	xmmword ptr[edx+12*4],xmm3

					add		eax,16*4
					add		edx,16*4
					loop	loopau
				}
			}
		}
		else {
			if((reinterpret_cast<unsigned long>(dst)&(__alignof(__m128)-1)) == 0) {
				// unaligned src, aligned dst
				__asm {
					mov		eax,dword ptr [src]
					mov		edx,dword ptr [dst]
					mov		ecx,[n]
loopua:
					prefetcht0 [eax+64]
					prefetcht0 [eax+96]
					movups	xmm0,xmmword ptr[eax]
					movaps	xmmword ptr[edx],xmm0
					movups	xmm1,xmmword ptr[eax+4*4]
					movaps	xmmword ptr[edx+4*4],xmm1
					movups	xmm2,xmmword ptr[eax+8*4]
					movaps	xmmword ptr[edx+8*4],xmm2
					movups	xmm3,xmmword ptr[eax+12*4]
					movaps	xmmword ptr[edx+12*4],xmm3

					add		eax,16*4
					add		edx,16*4
					loop	loopua
				}
			}
			else {
				// unaligned src, unaligned dst
				__asm {
					mov		eax,dword ptr [src]
					mov		edx,dword ptr [dst]
					mov		ecx,[n]
loopuu:
					prefetcht0 [eax+64]
					prefetcht0 [eax+96]
					movups	xmm0,xmmword ptr[eax]
					movups	xmmword ptr[edx],xmm0
					movups	xmm1,xmmword ptr[eax+4*4]
					movups	xmmword ptr[edx+4*4],xmm1
					movups	xmm2,xmmword ptr[eax+8*4]
					movups	xmmword ptr[edx+8*4],xmm2
					movups	xmm3,xmmword ptr[eax+12*4]
					movups	xmmword ptr[edx+12*4],xmm3

					add		eax,16*4
					add		edx,16*4
					loop	loopuu
				}
			}
		}
       	while(cnt--) *(dst++) = *(src++); 
    }
    else
#elif FLEXT_CPU == FLEXT_CPU_PPC && defined(__VEC__) && defined(__VECTOROPS__)
    {
   	    int n = cnt>>2,n4 = n<<2;
        cnt -= n4;
		vScopy(n4,(vector float *)src,(vector float *)dst);
		src += n4,dst += n4;
       	while(cnt--) *(dst++) = *(src++); 
	}
#endif // _MSC_VER
#endif // FLEXT_USE_SIMD
    {
	    int n = cnt>>3;
	    cnt -= n<<3;
	    while(n--) {
		    dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2]; dst[3] = src[3];
		    dst[4] = src[4]; dst[5] = src[5]; dst[6] = src[6]; dst[7] = src[7];
		    src += 8,dst += 8;
	    }
    	while(cnt--) *(dst++) = *(src++); 
    }
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
#ifdef FLEXT_USE_SIMD
#ifdef _MSC_VER
    if(GetSIMDCapabilities()&simd_sse) {
        // single precision

  	    int n = cnt>>4;
        cnt -= n<<4;

        __asm {
			movss	xmm0,xmmword ptr [s]
			shufps	xmm0,xmm0,0
		}

        if((reinterpret_cast<unsigned long>(dst)&(__alignof(__m128)-1)) == 0) {
            // aligned version
	        __asm {
				mov		ecx,[n]
				mov		edx,dword ptr [dst]
loopa:
				movaps	xmmword ptr[edx],xmm0
				movaps	xmmword ptr[edx+4*4],xmm0
				movaps	xmmword ptr[edx+8*4],xmm0
				movaps	xmmword ptr[edx+12*4],xmm0

				add		edx,16*4
				loop	loopa
			}
        }
        else {
            // unaligned version
			__asm {
				mov		ecx,[n]
				mov		edx,dword ptr [dst]
loopu:
				movups	xmmword ptr[edx],xmm0
				movups	xmmword ptr[edx+4*4],xmm0
				movups	xmmword ptr[edx+8*4],xmm0
				movups	xmmword ptr[edx+12*4],xmm0

				add		edx,16*4
				loop	loopu
			}
        }
      	while(cnt--) *(dst++) = s; 
    }
    else
#elif 0 //FLEXT_CPU == FLEXT_CPU_PPC && defined(__VEC__)
    if(GetSIMDCapabilities()&simd_altivec && IsVectorAligned(dst)) {
		vector float svec = IsVectorAligned(&s)?vec_splat(vec_ld(0,(vector float *)&s),0):LoadUnaligned(&s);
   	    int n = cnt>>4,n4 = n<<4;
        cnt -= n4;

		while(n--) {
			vec_st(svec,0,dst);
			vec_st(svec,16,dst);
			vec_st(svec,32,dst);
			vec_st(svec,48,dst);
			dst += 64;
		}

       	while(cnt--) *(dst++) = s; 
	}
	else
#endif
#endif // FLEXT_USE_SIMD
    {
	    int n = cnt>>3;
	    cnt -= n<<3;
	    while(n--) {
		    dst[0] = dst[1] = dst[2] = dst[3] = dst[4] = dst[5] = dst[6] = dst[7] = s;
		    dst += 8;
	    }
	    
	    while(cnt--) *(dst++) = s; 
    }
#endif
}


void flext::MulSamples(t_sample *dst,const t_sample *src,t_sample op,int cnt) 
{
#ifdef FLEXT_USE_IPP
    if(sizeof(t_sample) == 4) {
        ippsMulC_32f((const float *)src,(float)op,(float *)dst,cnt); 
    }
    else if(sizeof(t_sample) == 8) {
        ippsMulC_64f((const double *)src,(double)op,(double *)dst,cnt); 
    }
    else
        ERRINTERNAL();
#else
#ifdef FLEXT_USE_SIMD
#ifdef _MSC_VER
    if(GetSIMDCapabilities()&simd_sse) {
        // single precision
        __m128 a = _mm_load1_ps(&op);

   	    int n = cnt>>4;
        cnt -= n<<4;

        __asm {
			mov		eax,dword ptr [src]
			prefetcht0 [eax+0]
			prefetcht0 [eax+32]

			movss	xmm0,xmmword ptr [op]
			shufps	xmm0,xmm0,0
		}

        if((reinterpret_cast<unsigned long>(src)&(__alignof(__m128)-1)) == 0
            && (reinterpret_cast<unsigned long>(dst)&(__alignof(__m128)-1)) == 0
        ) {
            // aligned version
            __asm {
				mov		ecx,[n]
				mov		eax,dword ptr [src]
				mov		edx,dword ptr [dst]
loopa:
				prefetcht0 [eax+64]
				prefetcht0 [eax+96]

				movaps	xmm1,xmmword ptr[eax]
				mulps	xmm1,xmm0
				movaps	xmmword ptr[edx],xmm1

				movaps	xmm2,xmmword ptr[eax+4*4]
				mulps	xmm2,xmm0
				movaps	xmmword ptr[edx+4*4],xmm2

				movaps	xmm3,xmmword ptr[eax+8*4]
				mulps	xmm3,xmm0
				movaps	xmmword ptr[edx+8*4],xmm3

				movaps	xmm4,xmmword ptr[eax+12*4]
				mulps	xmm4,xmm0
				movaps	xmmword ptr[edx+12*4],xmm4

				add		eax,16*4
				add		edx,16*4
				loop	loopa
            }
        }
        else {
            // unaligned version
            __asm {
				mov		ecx,[n]
				mov		eax,dword ptr [src]
				mov		edx,dword ptr [dst]
loopu:
				prefetcht0 [eax+64]
				prefetcht0 [eax+96]

				movups	xmm1,xmmword ptr[eax]
				mulps	xmm1,xmm0
				movups	xmmword ptr[edx],xmm1

				movups	xmm2,xmmword ptr[eax+4*4]
				mulps	xmm2,xmm0
				movups	xmmword ptr[edx+4*4],xmm2

				movups	xmm3,xmmword ptr[eax+8*4]
				mulps	xmm3,xmm0
				movups	xmmword ptr[edx+8*4],xmm3

				movups	xmm4,xmmword ptr[eax+12*4]
				mulps	xmm4,xmm0
				movups	xmmword ptr[edx+12*4],xmm4

				add		eax,16*4
				add		edx,16*4
				loop	loopu
		    }
        }
	    while(cnt--) *(dst++) = *(src++)*op; 
    }
    else
#elif 0 //FLEXT_CPU == FLEXT_CPU_PPC && defined(__VEC__)
    if(GetSIMDCapabilities()&simd_altivec && IsVectorAligned(src) && IsVectorAligned(dst)) {
		vector float opvec = IsVectorAligned(&op)?vec_splat(vec_ld(0,(vector float *)&op),0):LoadUnaligned(&op);
		vector float addvec = (vector float)vec_splat_u32(0);
   	    int n = cnt>>4,n4 = n<<4;
        cnt -= n4;

		while(n--) {
			vec_st(vec_madd(vec_ld( 0,src),opvec,addvec), 0,dst);
			vec_st(vec_madd(vec_ld(16,src),opvec,addvec),16,dst);
			vec_st(vec_madd(vec_ld(32,src),opvec,addvec),32,dst);
			vec_st(vec_madd(vec_ld(48,src),opvec,addvec),48,dst);
			src += 64,dst += 64;
		}

       	while(cnt--) *(dst++) = *(src++)*op; 
	}
	else
#endif // _MSC_VER
#endif // FLEXT_USE_SIMD
    {
	    int n = cnt>>3;
	    cnt -= n<<3;

		if(src == dst) {
			while(n--) {
				dst[0] *= op; dst[1] *= op; dst[2] *= op; dst[3] *= op; 
				dst[4] *= op; dst[5] *= op; dst[6] *= op; dst[7] *= op; 
				dst += 8;
			}
			while(cnt--) *(dst++) *= op; 
		}
		else {
			while(n--) {
				dst[0] = src[0]*op; dst[1] = src[1]*op; 
				dst[2] = src[2]*op; dst[3] = src[3]*op; 
				dst[4] = src[4]*op; dst[5] = src[5]*op; 
				dst[6] = src[6]*op; dst[7] = src[7]*op; 
				src += 8,dst += 8;
			}
			while(cnt--) *(dst++) = *(src++)*op; 
		}
    }
#endif
}


void flext::MulSamples(t_sample *dst,const t_sample *src,const t_sample *op,int cnt) 
{
#ifdef FLEXT_USE_IPP
    if(sizeof(t_sample) == 4) {
        ippsMul_32f((const float *)src,(const float *)op,(float *)dst,cnt); 
    }
    else if(sizeof(t_sample) == 8) {
        ippsMul_32f((const double *)src,(const double *)op,(double *)dst,cnt); 
    }
    else
        ERRINTERNAL();
#else
#ifdef FLEXT_USE_SIMD
#ifdef _MSC_VER
    if(GetSIMDCapabilities()&simd_sse) {
        // single precision
   	    int n = cnt>>4;
        cnt -= n<<4;

		__asm {
			mov		eax,[src]
			mov		ebx,[op]
			prefetcht0 [eax+0]
			prefetcht0 [ebx+0]
			prefetcht0 [eax+32]
			prefetcht0 [ebx+32]
		}

        if((reinterpret_cast<unsigned long>(src)&(__alignof(__m128)-1)) == 0
            && (reinterpret_cast<unsigned long>(dst)&(__alignof(__m128)-1)) == 0
		) {
			if((reinterpret_cast<unsigned long>(op)&(__alignof(__m128)-1)) == 0) {
				__asm {
					mov		ecx,[n]
					mov		eax,dword ptr [src]
					mov		edx,dword ptr [dst]
					mov		ebx,dword ptr [op]
	loopaa:
					prefetcht0 [eax+64]
					prefetcht0 [ebx+64]
					prefetcht0 [eax+96]
					prefetcht0 [ebx+96]

					movaps	xmm0,xmmword ptr[eax]
					movaps	xmm1,xmmword ptr[ebx]
					mulps	xmm0,xmm1
					movaps	xmmword ptr[edx],xmm0

					movaps	xmm2,xmmword ptr[eax+4*4]
					movaps	xmm3,xmmword ptr[ebx+4*4]
					mulps	xmm2,xmm3
					movaps	xmmword ptr[edx+4*4],xmm2

					movaps	xmm4,xmmword ptr[eax+8*4]
					movaps	xmm5,xmmword ptr[ebx+8*4]
					mulps	xmm4,xmm5
					movaps	xmmword ptr[edx+8*4],xmm4

					movaps	xmm6,xmmword ptr[eax+12*4]
					movaps	xmm7,xmmword ptr[ebx+12*4]
					mulps	xmm6,xmm7
					movaps	xmmword ptr[edx+12*4],xmm6

					add		eax,16*4
					add		ebx,16*4
					add		edx,16*4
					loop	loopaa
				}
			}
			else {
				__asm {
					mov		ecx,[n]
					mov		eax,dword ptr [src]
					mov		edx,dword ptr [dst]
					mov		ebx,dword ptr [op]
	loopau:
					prefetcht0 [eax+64]
					prefetcht0 [ebx+64]
					prefetcht0 [eax+96]
					prefetcht0 [ebx+96]

					movaps	xmm0,xmmword ptr[eax]
					movups	xmm1,xmmword ptr[ebx]
					mulps	xmm0,xmm1
					movaps	xmmword ptr[edx],xmm0

					movaps	xmm2,xmmword ptr[eax+4*4]
					movups	xmm3,xmmword ptr[ebx+4*4]
					mulps	xmm2,xmm3
					movaps	xmmword ptr[edx+4*4],xmm2

					movaps	xmm4,xmmword ptr[eax+8*4]
					movups	xmm5,xmmword ptr[ebx+8*4]
					mulps	xmm4,xmm5
					movaps	xmmword ptr[edx+8*4],xmm4

					movaps	xmm6,xmmword ptr[eax+12*4]
					movups	xmm7,xmmword ptr[ebx+12*4]
					mulps	xmm6,xmm7
					movaps	xmmword ptr[edx+12*4],xmm6

					add		eax,16*4
					add		ebx,16*4
					add		edx,16*4
					loop	loopau
				}
			}
		}
        else {
			if((reinterpret_cast<unsigned long>(op)&(__alignof(__m128)-1)) == 0) {
				__asm {
					mov		ecx,[n]
					mov		eax,dword ptr [src]
					mov		edx,dword ptr [dst]
					mov		ebx,dword ptr [op]
	loopua:
					prefetcht0 [eax+64]
					prefetcht0 [ebx+64]
					prefetcht0 [eax+96]
					prefetcht0 [ebx+96]

					movups	xmm0,xmmword ptr[eax]
					movaps	xmm1,xmmword ptr[ebx]
					mulps	xmm0,xmm1
					movups	xmmword ptr[edx],xmm0

					movups	xmm2,xmmword ptr[eax+4*4]
					movaps	xmm3,xmmword ptr[ebx+4*4]
					mulps	xmm2,xmm3
					movups	xmmword ptr[edx+4*4],xmm2

					movups	xmm4,xmmword ptr[eax+8*4]
					movaps	xmm5,xmmword ptr[ebx+8*4]
					mulps	xmm4,xmm5
					movups	xmmword ptr[edx+8*4],xmm4

					movups	xmm6,xmmword ptr[eax+12*4]
					movaps	xmm7,xmmword ptr[ebx+12*4]
					mulps	xmm6,xmm7
					movups	xmmword ptr[edx+12*4],xmm6

					add		eax,16*4
					add		ebx,16*4
					add		edx,16*4
					loop	loopua
				}
			}
			else {
				__asm {
					mov		ecx,[n]
					mov		eax,dword ptr [src]
					mov		edx,dword ptr [dst]
					mov		ebx,dword ptr [op]
loopuu:
					prefetcht0 [eax+64]
					prefetcht0 [ebx+64]
					prefetcht0 [eax+96]
					prefetcht0 [ebx+96]

					movups	xmm0,xmmword ptr[eax]
					movups	xmm1,xmmword ptr[ebx]
					mulps	xmm0,xmm1
					movups	xmmword ptr[edx],xmm0

					movups	xmm2,xmmword ptr[eax+4*4]
					movups	xmm3,xmmword ptr[ebx+4*4]
					mulps	xmm2,xmm3
					movups	xmmword ptr[edx+4*4],xmm2

					movups	xmm4,xmmword ptr[eax+8*4]
					movups	xmm5,xmmword ptr[ebx+8*4]
					mulps	xmm4,xmm5
					movups	xmmword ptr[edx+8*4],xmm4

					movups	xmm6,xmmword ptr[eax+12*4]
					movups	xmm7,xmmword ptr[ebx+12*4]
					mulps	xmm6,xmm7
					movups	xmmword ptr[edx+12*4],xmm6

					add		eax,16*4
					add		ebx,16*4
					add		edx,16*4
					loop	loopuu
				}
			}
        }
	    while(cnt--) *(dst++) = *(src++) * *(op++); 
    }
    else
#elif 0 // FLEXT_CPU == FLEXT_CPU_PPC && defined(__VEC__)
    if(GetSIMDCapabilities()&simd_sse) {
	}
	else
#endif // _MSC_VER
#endif // FLEXT_USE_SIMD
    {
	    int n = cnt>>3;
	    cnt -= n<<3;

		if(src == dst) {
			while(n--) {
				dst[0] *= op[0]; dst[1] *= op[1]; 
				dst[2] *= op[2]; dst[3] *= op[3]; 
				dst[4] *= op[4]; dst[5] *= op[5]; 
				dst[6] *= op[6]; dst[7] *= op[7]; 
				dst += 8,op += 8;
			}
			while(cnt--) *(dst++) *= *(op++); 
		}
		else {
			while(n--) {
				dst[0] = src[0]*op[0]; dst[1] = src[1]*op[1]; 
				dst[2] = src[2]*op[2]; dst[3] = src[3]*op[3]; 
				dst[4] = src[4]*op[4]; dst[5] = src[5]*op[5]; 
				dst[6] = src[6]*op[6]; dst[7] = src[7]*op[7]; 
				src += 8,dst += 8,op += 8;
			}
			while(cnt--) *(dst++) = *(src++) * *(op++); 
		}
    }
#endif
}


void flext::AddSamples(t_sample *dst,const t_sample *src,t_sample op,int cnt) 
{
#ifdef FLEXT_USE_IPP
    if(sizeof(t_sample) == 4) {
        ippsAddC_32f((const float *)src,(float)op,(float *)dst,cnt); 
    }
    else if(sizeof(t_sample) == 8) {
        ippsAddC_64f_I((const double *)src,(double)op,(double *)dst,cnt); 
    }
    else
        ERRINTERNAL();
#else
#ifdef FLEXT_USE_SIMD
#ifdef _MSC_VER
    if(GetSIMDCapabilities()&simd_sse) {
        // single precision
   	    int n = cnt>>4;
        cnt -= n<<4;

        __asm {
			mov		eax,[src]
			prefetcht0 [eax+0]
			prefetcht0 [eax+32]

			movss	xmm0,xmmword ptr [op]
			shufps	xmm0,xmm0,0
		}

        if((reinterpret_cast<unsigned long>(src)&(__alignof(__m128)-1)) == 0
            && (reinterpret_cast<unsigned long>(dst)&(__alignof(__m128)-1)) == 0
        ) {
            // aligned version
	            __asm {
				mov		ecx,[n]
				mov		eax,dword ptr [src]
				mov		edx,dword ptr [dst]
loopa:
				prefetcht0 [eax+64]
				prefetcht0 [eax+96]

				movaps	xmm1,xmmword ptr[eax]
				addps	xmm1,xmm0
				movaps	xmmword ptr[edx],xmm1

				movaps	xmm2,xmmword ptr[eax+4*4]
				addps	xmm2,xmm0
				movaps	xmmword ptr[edx+4*4],xmm2

				movaps	xmm3,xmmword ptr[eax+8*4]
				addps	xmm3,xmm0
				movaps	xmmword ptr[edx+8*4],xmm3

				movaps	xmm4,xmmword ptr[eax+12*4]
				addps	xmm4,xmm0
				movaps	xmmword ptr[edx+12*4],xmm4
 
				add		eax,16*4
				add		edx,16*4
				loop	loopa
           }
        }
        else {
            // unaligned version
	        __asm {
				mov		ecx,[n]
				mov		eax,dword ptr [src]
				mov		edx,dword ptr [dst]
loopu:
				prefetcht0 [eax+64]
				prefetcht0 [eax+96]

				movups	xmm1,xmmword ptr[eax]
				addps	xmm1,xmm0
				movups	xmmword ptr[edx],xmm1

				movups	xmm2,xmmword ptr[eax+4*4]
				addps	xmm2,xmm0
				movups	xmmword ptr[edx+4*4],xmm2

				movups	xmm3,xmmword ptr[eax+8*4]
				addps	xmm3,xmm0
				movups	xmmword ptr[edx+8*4],xmm3

				movups	xmm4,xmmword ptr[eax+12*4]
				addps	xmm4,xmm0
				movups	xmmword ptr[edx+12*4],xmm4

				add		eax,16*4
				add		edx,16*4
				loop	loopu
			}
        }
	    while(cnt--) *(dst++) = *(src++)+op; 
    }
    else
#elif 0 //FLEXT_CPU == FLEXT_CPU_PPC && defined(__VEC__)
    if(GetSIMDCapabilities()&simd_altivec) {
	}
	else
#endif // _MSC_VER
#endif // FLEXT_USE_SIMD
    {
	    int n = cnt>>3;
	    cnt -= n<<3;

		if(src == dst) {
			while(n--) {
				dst[0] += op; dst[1] += op; dst[2] += op; dst[3] += op; 
				dst[4] += op; dst[5] += op; dst[6] += op; dst[7] += op; 
				dst += 8;
			}
			while(cnt--) *(dst++) += op; 
		}
		else {
			while(n--) {
				dst[0] = src[0]+op; dst[1] = src[1]+op; 
				dst[2] = src[2]+op; dst[3] = src[3]+op; 
				dst[4] = src[4]+op; dst[5] = src[5]+op; 
				dst[6] = src[6]+op; dst[7] = src[7]+op; 
				src += 8,dst += 8;
			}
			while(cnt--) *(dst++) = *(src++)+op; 
		}
    }
#endif
}


void flext::AddSamples(t_sample *dst,const t_sample *src,const t_sample *op,int cnt) 
{
#ifdef FLEXT_USE_IPP
    if(sizeof(t_sample) == 4) {
        ippsAdd_32f((const float *)src,(const float *)op,(float *)dst,cnt); 
    }
    else if(sizeof(t_sample) == 8) {
        ippsAdd_64f((const double *)src,(const double *)op,(double *)dst,cnt); 
    }
    else
        ERRINTERNAL();
#else
#ifdef FLEXT_USE_SIMD
#ifdef _MSC_VER
    if(GetSIMDCapabilities()&simd_sse) {
		// Prefetch cache
		__asm {
			mov		eax,dword ptr [src]
			mov		ebx,dword ptr [op]
			prefetcht0 [eax]
			prefetcht0 [ebx]
			prefetcht0 [eax+32]
			prefetcht0 [ebx+32]
		}

        // single precision
   	    int n = cnt>>4;
        cnt -= n<<4;

        if((reinterpret_cast<unsigned long>(src)&(__alignof(__m128)-1)) == 0
            && (reinterpret_cast<unsigned long>(dst)&(__alignof(__m128)-1)) == 0
		) {
			if((reinterpret_cast<unsigned long>(op)&(__alignof(__m128)-1)) == 0) {
				__asm {
					mov		ecx,dword ptr [n]
					mov		eax,dword ptr [src]
					mov		edx,dword ptr [dst]
					mov		ebx,dword ptr [op]
	loopaa:
					prefetcht0 [eax+64]
					prefetcht0 [ebx+64]
					prefetcht0 [eax+96]
					prefetcht0 [ebx+96]

					movaps	xmm0,xmmword ptr[eax]
					movaps	xmm1,xmmword ptr[ebx]
					addps	xmm0,xmm1
					movaps	xmmword ptr[edx],xmm0

					movaps	xmm2,xmmword ptr[eax+4*4]
					movaps	xmm3,xmmword ptr[ebx+4*4]
					addps	xmm2,xmm3
					movaps	xmmword ptr[edx+4*4],xmm2

					movaps	xmm4,xmmword ptr[eax+8*4]
					movaps	xmm5,xmmword ptr[ebx+8*4]
					addps	xmm4,xmm5
					movaps	xmmword ptr[edx+8*4],xmm4

					movaps	xmm6,xmmword ptr[eax+12*4]
					movaps	xmm7,xmmword ptr[ebx+12*4]
					addps	xmm6,xmm7
					movaps	xmmword ptr[edx+12*4],xmm6

					add		eax,16*4
					add		ebx,16*4
					add		edx,16*4
					loop	loopaa
				}
			}
			else {
				__asm {
					mov		ecx,dword ptr [n]
					mov		eax,dword ptr [src]
					mov		edx,dword ptr [dst]
					mov		ebx,dword ptr [op]
	loopau:
					prefetcht0 [eax+64]
					prefetcht0 [ebx+64]
					prefetcht0 [eax+96]
					prefetcht0 [ebx+96]

					movaps	xmm0,xmmword ptr[eax]
					movups	xmm1,xmmword ptr[ebx]
					addps	xmm0,xmm1
					movaps	xmmword ptr[edx],xmm0

					movaps	xmm2,xmmword ptr[eax+4*4]
					movups	xmm3,xmmword ptr[ebx+4*4]
					addps	xmm2,xmm3
					movaps	xmmword ptr[edx+4*4],xmm2

					movaps	xmm4,xmmword ptr[eax+8*4]
					movups	xmm5,xmmword ptr[ebx+8*4]
					addps	xmm4,xmm5
					movaps	xmmword ptr[edx+8*4],xmm4

					movaps	xmm6,xmmword ptr[eax+12*4]
					movups	xmm7,xmmword ptr[ebx+12*4]
					addps	xmm6,xmm7
					movaps	xmmword ptr[edx+12*4],xmm6

					add		eax,16*4
					add		ebx,16*4
					add		edx,16*4
					loop	loopau
				}
			}
        }
        else {
			if((reinterpret_cast<unsigned long>(op)&(__alignof(__m128)-1)) == 0) {
				__asm {
					mov		ecx,dword ptr [n]
					mov		eax,dword ptr [src]
					mov		edx,dword ptr [dst]
					mov		ebx,dword ptr [op]
	loopua:
					prefetcht0 [eax+64]
					prefetcht0 [ebx+64]
					prefetcht0 [eax+96]
					prefetcht0 [ebx+96]

					movups	xmm0,xmmword ptr[eax]
					movaps	xmm1,xmmword ptr[ebx]
					addps	xmm0,xmm1
					movups	xmmword ptr[edx],xmm0

					movups	xmm2,xmmword ptr[eax+4*4]
					movaps	xmm3,xmmword ptr[ebx+4*4]
					addps	xmm2,xmm3
					movups	xmmword ptr[edx+4*4],xmm2

					movups	xmm4,xmmword ptr[eax+8*4]
					movaps	xmm5,xmmword ptr[ebx+8*4]
					addps	xmm4,xmm5
					movups	xmmword ptr[edx+8*4],xmm4

					movups	xmm6,xmmword ptr[eax+12*4]
					movaps	xmm7,xmmword ptr[ebx+12*4]
					addps	xmm6,xmm7
					movups	xmmword ptr[edx+12*4],xmm6

					add		eax,16*4
					add		ebx,16*4
					add		edx,16*4
					loop	loopua
				}
			}
			else {
				__asm {
					mov		ecx,dword ptr [n]
					mov		eax,dword ptr [src]
					mov		edx,dword ptr [dst]
					mov		ebx,dword ptr [op]
	loopuu:
					prefetcht0 [eax+64]
					prefetcht0 [ebx+64]
					prefetcht0 [eax+96]
					prefetcht0 [ebx+96]

					movups	xmm0,xmmword ptr[eax]
					movups	xmm1,xmmword ptr[ebx]
					addps	xmm0,xmm1
					movups	xmmword ptr[edx],xmm0

					movups	xmm2,xmmword ptr[eax+4*4]
					movups	xmm3,xmmword ptr[ebx+4*4]
					addps	xmm2,xmm3
					movups	xmmword ptr[edx+4*4],xmm2

					movups	xmm4,xmmword ptr[eax+8*4]
					movups	xmm5,xmmword ptr[ebx+8*4]
					addps	xmm4,xmm5
					movups	xmmword ptr[edx+8*4],xmm4

					movups	xmm6,xmmword ptr[eax+12*4]
					movups	xmm7,xmmword ptr[ebx+12*4]
					addps	xmm6,xmm7
					movups	xmmword ptr[edx+12*4],xmm6

					add		eax,16*4
					add		ebx,16*4
					add		edx,16*4
					loop	loopuu
				}
			}
        }
	    while(cnt--) *(dst++) = *(src++) + *(op++); 
    }
    else
#elif 0 //FLEXT_CPU == FLEXT_CPU_PPC && defined(__VEC__)
    {
	}
#endif // _MSC_VER
#endif // FLEXT_USE_SIMD
    {
	    int n = cnt>>3;
	    cnt -= n<<3;

		if(dst == src) {
			while(n--) {
				dst[0] += op[0]; dst[1] += op[1]; 
				dst[2] += op[2]; dst[3] += op[3]; 
				dst[4] += op[4]; dst[5] += op[5]; 
				dst[6] += op[6]; dst[7] += op[7]; 
				dst += 8,op += 8;
			}
			while(cnt--) *(dst++) += *(op++); 
		}
		else {
			while(n--) {
				dst[0] = src[0]+op[0]; dst[1] = src[1]+op[1]; 
				dst[2] = src[2]+op[2]; dst[3] = src[3]+op[3]; 
				dst[4] = src[4]+op[4]; dst[5] = src[5]+op[5]; 
				dst[6] = src[6]+op[6]; dst[7] = src[7]+op[7]; 
				src += 8,dst += 8,op += 8;
			}
			while(cnt--) *(dst++) = *(src++) + *(op++); 
		}
    }
#endif
}


void flext::ScaleSamples(t_sample *dst,const t_sample *src,t_sample opmul,t_sample opadd,int cnt) 
{
#ifdef FLEXT_USE_IPP
    if(sizeof(t_sample) == 4) {
        ippsMulC_32f((const float *)src,(float)opmul,(float *)dst,cnt); 
        ippsAddC_32f_I((float)opadd,(float *)dst,cnt); 
    }
    else if(sizeof(t_sample) == 8) {
        ippsMulC_64f((const double *)src,(double)opmul,(double *)dst,cnt); 
        ippsAddC_64f_I((double)opadd,(double *)dst,cnt); 
    }
    else
        ERRINTERNAL();
#else
#ifdef FLEXT_USE_SIMD
#ifdef _MSC_VER
    if(GetSIMDCapabilities()&simd_sse) {
        // single precision
   	    int n = cnt>>4;
        cnt -= n<<4;

        __asm {
			mov		eax,dword ptr [src]
			prefetcht0 [eax+0]
			prefetcht0 [eax+32]

			movss	xmm0,xmmword ptr [opadd]
			shufps	xmm0,xmm0,0
			movss	xmm1,xmmword ptr [opmul]
			shufps	xmm1,xmm1,0
		}

        if((reinterpret_cast<unsigned long>(src)&(__alignof(__m128)-1)) == 0
            && (reinterpret_cast<unsigned long>(dst)&(__alignof(__m128)-1)) == 0
        ) {
            // aligned version
	        __asm {
				mov		ecx,dword ptr [n]
				mov		eax,dword ptr [src]
				mov		edx,dword ptr [dst]
loopa:
				prefetcht0 [eax+64]
				prefetcht0 [eax+96]

				movaps	xmm2,xmmword ptr[eax]
				mulps	xmm2,xmm1
				addps	xmm2,xmm0
				movaps	xmmword ptr[edx],xmm2

				movaps	xmm3,xmmword ptr[eax+4*4]
				mulps	xmm3,xmm1
				addps	xmm3,xmm0
				movaps	xmmword ptr[edx+4*4],xmm3

				movaps	xmm4,xmmword ptr[eax+8*4]
				mulps	xmm4,xmm1
				addps	xmm4,xmm0
				movaps	xmmword ptr[edx+8*4],xmm4

				movaps	xmm5,xmmword ptr[eax+12*4]
				mulps	xmm5,xmm1
				addps	xmm5,xmm0
				movaps	xmmword ptr[edx+12*4],xmm5

				add		eax,16*4
				add		edx,16*4
				loop	loopa
			}
        }
        else {
            // unaligned version
	        __asm {
				mov		ecx,dword ptr [n]
				mov		eax,dword ptr [src]
				mov		edx,dword ptr [dst]
loopu:
				prefetcht0 [eax+64]
				prefetcht0 [eax+96]

				movups	xmm2,xmmword ptr[eax]
				mulps	xmm2,xmm1
				addps	xmm2,xmm0
				movups	xmmword ptr[edx],xmm2

				movups	xmm3,xmmword ptr[eax+4*4]
				mulps	xmm3,xmm1
				addps	xmm3,xmm0
				movups	xmmword ptr[edx+4*4],xmm3

				movups	xmm4,xmmword ptr[eax+8*4]
				mulps	xmm4,xmm1
				addps	xmm4,xmm0
				movups	xmmword ptr[edx+8*4],xmm4

				movups	xmm5,xmmword ptr[eax+12*4]
				mulps	xmm5,xmm1
				addps	xmm5,xmm0
				movups	xmmword ptr[edx+12*4],xmm5

				add		eax,16*4
				add		edx,16*4
				loop	loopu
			}
        }
	    while(cnt--) *(dst++) = *(src++)*opmul+opadd; 
    }
    else
#elif 0 //FLEXT_CPU == FLEXT_CPU_PPC && defined(__VEC__)
    if(GetSIMDCapabilities()&simd_altivec) {
	}
	else
#endif // _MSC_VER
#endif // FLEXT_USE_SIMD
    {
	    int n = cnt>>3;
	    cnt -= n<<3;
	    while(n--) {
		    dst[0] = src[0]*opmul+opadd; dst[1] = src[1]*opmul+opadd; 
		    dst[2] = src[2]*opmul+opadd; dst[3] = src[3]*opmul+opadd; 
		    dst[4] = src[4]*opmul+opadd; dst[5] = src[5]*opmul+opadd; 
		    dst[6] = src[6]*opmul+opadd; dst[7] = src[7]*opmul+opadd; 
		    src += 8,dst += 8;
	    }
	    while(cnt--) *(dst++) = *(src++)*opmul+opadd; 
    }
#endif
}

