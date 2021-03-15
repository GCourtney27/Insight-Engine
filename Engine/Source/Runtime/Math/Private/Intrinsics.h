#pragma once
#include <Runtime/Core.h>

#include <xmmintrin.h>
#include <assert.h>

namespace Insight
{
	namespace Intrinsics
	{
		/*
			Packed 16-byte floating-point vector capable of SIMD instructions.
		*/
		typedef __m128 XVector;

		/*
			Calling convention for all methods.
		*/
#		define XVEC_CALL_CONV __vectorcall

		/*
			The X, Y, Z, W component positions in an array.
			Used when calculating or extracting per-component values
		*/
#		define X_POS 0
#		define Y_POS 1
#		define Z_POS 2
#		define W_POS 3

		// -------------------------
		//		Construction		|
		// -------------------------

		FORCE_INLINE void XVEC_CALL_CONV PackXVecFromArray(const float* Arr, XVector& Destination)
		{
			// Load X Y Z into intermediate registers.
			__m128 X = _mm_load_ss(&Arr[X_POS]);
			__m128 Y = _mm_load_ss(&Arr[Y_POS]);
			__m128 Z = _mm_load_ss(&Arr[Z_POS]);
			// Load X and Y into their own register using the low order bits
			__m128 XYCombined = _mm_unpacklo_ps(X, Y);
			// Load Z into the high order bits of the combined X and Y register.
			Destination = _mm_movelh_ps(XYCombined, Z);
		}

		/*
			Store an XMVector as a float array.
		*/
		FORCE_INLINE void XVEC_CALL_CONV XVectorUnpackToArray(float* Destination, int DestinationSize, const XVector& Source)
		{
			IE_ASSERT(DestinationSize >= 8 * sizeof(float), "Size of destination array was too small to pack values into.");

			// Shuffle the bits in the source vector to a specific pattern for each element in the destination.
			XVector T1 = _mm_shuffle_ps(Source, Source, _MM_SHUFFLE(Y_POS, Y_POS, Y_POS, Y_POS));
			XVector T2 = _mm_shuffle_ps(Source, Source, _MM_SHUFFLE(Z_POS, Z_POS, Z_POS, Z_POS));
			XVector T3 = _mm_shuffle_ps(Source, Source, _MM_SHUFFLE(W_POS, W_POS, W_POS, W_POS));

			_mm_store_ps(&(Destination[X_POS]), Source);
			_mm_store_ps(&(Destination[Y_POS]), T1);
			_mm_store_ps(&(Destination[Z_POS]), T2);
			_mm_store_ps(&(Destination[W_POS]), T3);
		}



		// ------------------
		//		Utility		|
		// ------------------

		FORCE_INLINE float GetXComponent(const XVector& Source)
		{
			float Result[4];
			_mm_store_ps(Result, Source);
			return Result[X_POS];
		}

		FORCE_INLINE float GetYComponent(const XVector& Source)
		{
			XVector Shuffle = _mm_shuffle_ps(Source, Source, _MM_SHUFFLE(Y_POS, Y_POS, Y_POS, Y_POS));

			float Result[4];
			_mm_store_ps(Result, Shuffle);
			return Result[Y_POS];
		}

		FORCE_INLINE float GetZComponent(const XVector& Source)
		{
			XVector Shuffle = _mm_shuffle_ps(Source, Source, _MM_SHUFFLE(Z_POS, Z_POS, Z_POS, Z_POS));

			float Result[4];
			_mm_store_ps(Result, Shuffle);
			return Result[Z_POS];
		}

		FORCE_INLINE float GetWComponent(const XVector& Source)
		{
			XVector Shuffle = _mm_shuffle_ps(Source, Source, _MM_SHUFFLE(W_POS, W_POS, W_POS, W_POS));

			float Result[4];
			_mm_store_ps(Result, Shuffle);
			return Result[W_POS];
		}

		// ---------------------
		//		Arithmetic		|
		// ---------------------

		/*
			Add two packed vectors together and returns the result.
		*/
		FORCE_INLINE XVector XVEC_CALL_CONV XVectorAdd(const XVector& V1, const XVector& V2)
		{
			return _mm_add_ps(V1, V2);
		}

		/*
			Subtract two packed vectors together and returns the result.
		*/
		FORCE_INLINE XVector XVEC_CALL_CONV XVectorSubtract(const XVector& V1, const XVector& V2)
		{
			return _mm_sub_ps(V1, V2);
		}

		/*
			Multiply two packed vectors together and returns the result.
		*/
		FORCE_INLINE XVector XVEC_CALL_CONV XVectorMultiply(const XVector& V1, const XVector& V2)
		{
			return _mm_mul_ps(V1, V2);
		}

		/*
			Divide two packed vectors together and returns the result.
		*/
		FORCE_INLINE XVector XVEC_CALL_CONV XVectorDivide(const XVector& V1, const XVector& V2)
		{
			return _mm_div_ps(V1, V2);
		}

		/*
			Scales the vector by a value and returns the result.
		*/
		FORCE_INLINE XVector XVEC_CALL_CONV XVectorScale(const XVector& V, float Value)
		{
			float Values[4] = { Value , Value , Value , Value };
			XVector V2;
			PackXVecFromArray(Values, V2);

			return XVectorMultiply(V, V2);
		}
	}
}

#undef X_POS
#undef Y_POS
#undef Z_POS
#undef W_POS

// eof
