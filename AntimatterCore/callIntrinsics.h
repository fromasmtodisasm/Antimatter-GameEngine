#pragma once
#include "SystemHeader.h"
#include<intrin.h>

//http://blog.softwareverify.com/how-to-do-cpuid-and-rdtsc-on-32-bit-and-64-bit-windows/

#ifdef _WIN64
#include 
#else	// _WIN64
// x86 architecture

// __debugbreak()

#if     _MSC_VER >= 1300
// Win32, __debugbreak defined for VC2005 onwards
#else	//_MSC_VER >= 1300
// define for before VC 2005

#define __debugbreak()				__asm { int 3 }
#endif	//_MSC_VER >= 1300

// __cpuid(registers, type)
//		registers is int[4], 
//		type = 0

// DO NOT add ";" after each instruction - it screws up the code generation

#define rdtsc	__asm __emit 0fh __asm __emit 031h
#define cpuid	__asm __emit 0fh __asm __emit 0a2h

inline void _cpuid(int	cpuInfo[4],
	int	cpuType)
{
	__asm pushad;
	__asm mov	eax, cpuType;

	cpuid;

	if (cpuInfo != NULL)
	{
		__asm mov	cpuInfo[0], eax;
		__asm mov	cpuInfo[1], ebx;
		__asm mov	cpuInfo[2], ecx;
		__asm mov	cpuInfo[3], edx;
	}

	__asm popad;
}

// __rdtsc()

inline unsigned __int64 _rdtsc()
{
	LARGE_INTEGER	li;

	rdtsc;

	__asm	mov	li.LowPart, eax;
	__asm	mov	li.HighPart, edx;
	return li.QuadPart;
}

#endif	// _WIN64