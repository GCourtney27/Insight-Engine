#pragma once

#include "Runtime/Math/Private/Vector3Base.h"


namespace Insight 
{

	// ------------------------
	//	Floating Point Vectors |
	// ------------------------

	/*
		A vector with 3 32-bit floating point components.
	*/
	typedef TVector3Base<float> FVector3;

	/*
		A vector with 3 double-precision floating components.
	*/
	typedef TVector3Base<double> DVector3;



	// -------------------------
	//		Integer Vectors		|
	// -------------------------

	// Unsigned Int Vectors
	// --------------------

	/*
		A vector with 3 8-bit unsigned integer components.
	*/
	typedef TVector3Base<UInt8> U8Vector3;

	/*
		A vector with 3 16-bit unsigned integer components.
	*/
	typedef TVector3Base<UInt16> U16Vector3;

	/*
		A vector with 3 32-bit unsigned integer components.
	*/
	typedef TVector3Base<UInt32> U32Vector3;

	/*
		A vector with 3 64-bit unsigned integer components.
	*/
	typedef TVector3Base<UInt64> U64Vector3;

	// Signed Int Vectors
	// ------------------

	/*
		A vector with 3 8-bit signed integer components.
	*/
	typedef TVector3Base<Int8> I8Vector3;

	/*
		A vector with 3 16-bit signed integer components.
	*/
	typedef TVector3Base<Int16> I16Vector3;

	/*
		A vector with 3 32-bit signed integer components.
	*/
	typedef TVector3Base<int> IVector3;

	/*
		A vector with 3 64-bit signed integer components.
	*/
	typedef TVector3Base<Int64> I64Vector3;


}
