// Copyright Insight Interactive. All rights reserved.
#pragma once

#include "Core/Public/DataTypes.h"
#include "Core/Public/EnumHelper.h"

// Safely checks a pointer and deletes it if it is non-null.
#define SAFE_DELETE_PTR(Ptr)		if( (Ptr) != NULL ) { delete (Ptr); } else { IE_ASSERT(false); IE_LOG(Error, TEXT("Trying to delete null pointer!")); }
#define SAFE_DELETE_PTR_ARRAY(Ptr)	if( (Ptr) != NULL ) { delete[] (Ptr); } else { IE_ASSERT(false); IE_LOG(Error, TEXT("Trying to delete null array pointer!")); }
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
// Returns the required memory size in bytes for a given megabyte value.
#define IE_MEGABYTES(Value)			( Value * 1024 )
// Returns the required memory size in bytes for a given gigabyte value.
#define IE_GIGABYTES(Value)			( Value * IE_MEGABYTES(1024) )
#define IE_PI						3.14159265359
#define IE_2PI						(2.0 * IE_PI)
#define IE_PRAGMA_DISABLE(X, ...)				\
IE_PRAGMA (warning (push))						\
IE_PRAGMA (warning (disable : LITERAL(X)))		\
__VA_ARGS__										\
IE_PRAGMA (warning (pop))						
#define StackAlloc(Size)			alloca(Size)

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
		Copies memory from a source to destination for a given number of bytes.
		@param Source - The source of the copy.
		@param Destination - The destination to store the copied data.
		@param SizeInBytes - The size of the copy to be performed.
	*/
	FORCEINLINE void CopyMemRanged(const void* Source, void* Destination, UInt64 SizeInBytes)
	{
		::memcpy(Destination, Source, SizeInBytes);
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
