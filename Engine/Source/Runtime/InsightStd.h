// Copyright Insight Interactive. All rights reserved.
#pragma once

#include "Core/Public/DataTypes.h"
#include "Core/Public/EnumHelper.h"
#include "Platform/Public/PlatformCommon.h"

// Safely checks a pointer and deletes it if it is non-null.
#define SAFE_DELETE_PTR(Ptr)		if( (Ptr) ) { delete (Ptr); } else { IE_ASSERT(false); IE_LOG(Error, TEXT("Trying to delete null pointer!")); }
// Safely checks a COM pointer and deletes it if it is non-null.
#define COM_SAFE_RELEASE(ComObject) if( (ComObject) ) { (ComObject)->Release(); (ComObject) = nullptr; }
// The literal value of a piece of text.
#define LITERAL(Value)				#Value
#define FORCEINLINE					__forceinline
#define INLINE						inline 
#define CEXPR						constexpr
#define NULL						0
#define IE_PRAGMA(X)				__pragma(X)
#define IE_NO_DISCARD				[[nodiscard]]
// Aligns a structure to a specified number of bytes.
#define IE_ALIGN(InBytes)			__declspec( align(InBytes) )
// Returns the size of a array.
#define IE_ARRAYSIZE(Arr)			( sizeof(Arr) / sizeof(Arr[0]) )
// The max path for a string of characters (analogous to microsoft's MAX_PATH).
#define IE_MAX_PATH					260
#define IE_PI						3.14159265359
#define IE_2PI						(2.0 * IE_PI)
#define IE_PRAGMA_DISABLE(X, ...)				\
IE_PRAGMA (warning (push))						\
IE_PRAGMA (warning (disable : LITERAL(X)))		\
__VA_ARGS__										\
IE_PRAGMA (warning (pop))						\


namespace Insight
{
	/*
		Zeros the memory on a pointer for a specified number of bytes.
		@param Mem - A pointer to the data to 0.
		@param Size - The length in bytes to 0 out.
	*/
	template <typename T>
	FORCEINLINE CEXPR void ZeroMemRanged(T* Mem, size_t Size = sizeof(T))
	{
		::memset(RCast<void*>(Mem), 0, Size);
	}

	/*
		Performs a const_cast on specified value.
	*/
	template <typename As, typename Value>
	FORCEINLINE CEXPR As CCast(Value* Val)
	{
		return const_cast<As>(Val);
	}

	/*
		Performs a reinterpret_cast on specified value.
	*/
	template <typename As, typename Value>
	FORCEINLINE CEXPR As RCast(Value* Val)
	{
		return reinterpret_cast<As>(Val);
	}

	/*
		Performs a dynamic_cast on specified value.
	*/
	template <typename As, typename Value>
	FORCEINLINE CEXPR As DCast(Value* Val)
	{
		return dynamic_cast<As>(Val);
	}

	/*
		Performs a static_cast on specified value.
	*/
	template <typename As, typename Value>
	FORCEINLINE CEXPR As SCast(Value& Val)
	{
		return static_cast<As>(Val);
	}
}
