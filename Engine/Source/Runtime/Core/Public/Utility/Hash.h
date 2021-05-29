#pragma once

#include <Runtime/Core.h>
#include <intrin.h>


// This requires SSE4.2 which is present on Intel Nehalem (Nov. 2008)
// and AMD Bulldozer (Oct. 2011) processors.  I could put a runtime
// check for this, but I'm just going to assume people playing with
// DirectX 12 on Windows 10 have fairly recent machines.
#ifdef _M_X64
#   define ENABLE_SSE_CRC32 1
#else
#   define ENABLE_SSE_CRC32 0
#endif

#if ENABLE_SSE_CRC32
#   pragma intrinsic(_mm_crc32_u32)
#   pragma intrinsic(_mm_crc32_u64)
#endif

namespace Insight
{
    template <typename T> __forceinline T AlignUpWithMask(T value, size_t mask)
    {
        return (T)(((size_t)value + mask) & ~mask);
    }

    template <typename T> __forceinline T AlignDownWithMask(T value, size_t mask)
    {
        return (T)((size_t)value & ~mask);
    }

    template <typename T> __forceinline T AlignUp(T value, size_t alignment)
    {
        return AlignUpWithMask(value, alignment - 1);
    }

    template <typename T> __forceinline T AlignDown(T value, size_t alignment)
    {
        return AlignDownWithMask(value, alignment - 1);
    }

    inline size_t HashRange(const UInt32* const Begin, const UInt32* const End, size_t Hash)
    {
#if ENABLE_SSE_CRC32
        const UInt64* Iter64 = (const UInt64*)AlignUp(Begin, 8);
        const UInt64* const End64 = (const UInt64* const)AlignDown(End, 8);

        // If not 64-bit aligned, start with a single u32
        if ((UInt32*)Iter64 > Begin)
            Hash = _mm_crc32_u32((UInt32)Hash, *Begin);

        // Iterate over consecutive u64 values
        while (Iter64 < End64)
            Hash = _mm_crc32_u64((UInt32)Hash, *Iter64++);

        // If there is a 32-bit remainder, accumulate that
        if ((UInt32*)Iter64 < End)
            Hash = _mm_crc32_u32((UInt32)Hash, *(UInt32*)Iter64);
#else
        // An inexpensive hash for CPUs lacking SSE4.2
        for (const UInt32* Iter = Begin; Iter < End; ++Iter)
            Hash = 16777619U * Hash ^ *Iter;
#endif

        return Hash;
    }

    template <typename T> inline size_t HashState(const T* StateDesc, size_t Count = 1, size_t Hash = 2166136261U)
    {
        static_assert((sizeof(T) & 3) == 0 && alignof(T) >= 4, "State object is not word-aligned");
        return HashRange((UInt32*)StateDesc, (UInt32*)(StateDesc + Count), Hash);
    }
}