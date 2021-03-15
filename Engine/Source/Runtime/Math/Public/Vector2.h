#pragma once

#include "Runtime/Math/Private/Vector2Base.h"

namespace Insight
{

	// ------------------------
	//	Floating Point Vectors |
	// ------------------------

	/*
		A vector with 3 32-bit floating point components.
	*/
	typedef TVector2Base<float> FVector2;

	/*
		A vector with 3 double-precision floating components.
	*/
	typedef TVector2Base<double> DVector2;



	// -------------------------
	//		Integer Vectors		|
	// -------------------------

	// Unsigned Int Vectors
	// --------------------

	/*
		A vector with 3 8-bit unsigned integer components.
	*/
	typedef TVector2Base<UInt8> U8Vector2;

	/*
		A vector with 3 16-bit unsigned integer components.
	*/
	typedef TVector2Base<UInt16> U16Vector2;

	/*
		A vector with 3 32-bit unsigned integer components.
	*/
	typedef TVector2Base<UInt32> U32Vector2;

	/*
		A vector with 3 64-bit unsigned integer components.
	*/
	typedef TVector2Base<UInt64> U64Vector2;

	// Signed Int Vectors
	// ------------------

	/*
		A vector with 3 8-bit signed integer components.
	*/
	typedef TVector2Base<Int8> I8Vector2;

	/*
		A vector with 3 16-bit signed integer components.
	*/
	typedef TVector2Base<Int16> I16Vector2;

	/*
		A vector with 3 32-bit signed integer components.
	*/
	typedef TVector2Base<int> IVector2;

	/*
		A vector with 3 64-bit signed integer components.
	*/
	typedef TVector2Base<Int64> I64Vector2;


}
