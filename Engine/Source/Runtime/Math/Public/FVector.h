#pragma once

#include <Runtime/Core.h>

#include "Runtime/Math/Private/Intrinsics.h"

namespace Insight {

	using Intrinsics::XVector;

	/*
		SIMD enabled vector with X, Y, and Z components.
	*/
	class FVector
	{
	protected:
		// 128 bit wide register holding the values of the vector
		XVector m_Data;

	public:

		// Constructors
		//
		FORCE_INLINE FVector()
		{
			float InitVal[3] = { 0.0f, 0.0f, 0.0f };
			Intrinsics::PackXVecFromArray(InitVal, m_Data);
		}
		FORCE_INLINE FVector(float&& Value)
		{
			float InitVal[3] = { Value, Value , Value };
			Intrinsics::PackXVecFromArray(InitVal, m_Data);
		}
		FORCE_INLINE FVector(float& X, float& Y, float& Z)
		{
			float InitVal[3] = { X, Y, Z };
			Intrinsics::PackXVecFromArray(InitVal, m_Data);
		}

		FORCE_INLINE FVector(float&& X, float&& Y, float&& Z)
		{
			float InitVal[3] = { X, Y, Z };
			Intrinsics::PackXVecFromArray(InitVal, m_Data);
		}
		FORCE_INLINE FVector(const XVector& Data)
		{
			m_Data = Data;
		}

		// Destructor
		//
		FORCE_INLINE ~FVector() = default;

		// Accessors
		//
		FORCE_INLINE XVector Data()
		{
			return m_Data;
		}

		FORCE_INLINE float X() const 
		{
			return Intrinsics::GetXComponent(m_Data);
		}

		FORCE_INLINE float Y() const
		{
			return Intrinsics::GetYComponent(m_Data);
		}

		FORCE_INLINE float Z() const
		{
			return Intrinsics::GetZComponent(m_Data);
		}

		// Utility
		//
		FVector3 ToFVector3()
		{
			return FVector3(this->X(), this->Y(), this->Z());
		}

		// Arithmetic
		//
		FORCE_INLINE const FVector operator +(const FVector& rhs) const
		{
			return FVector(Intrinsics::XVectorAdd(this->m_Data, rhs.m_Data));
		}
		FORCE_INLINE FVector& operator +=(const FVector& rhs)
		{
			this->m_Data = Intrinsics::XVectorAdd(this->m_Data, rhs.m_Data);
			return *this;
		}

		FORCE_INLINE const FVector operator -(const FVector& rhs) const
		{
			return FVector(Intrinsics::XVectorSubtract(this->m_Data, rhs.m_Data));
		}
		FORCE_INLINE FVector& operator -=(const FVector& rhs)
		{
			this->m_Data = Intrinsics::XVectorSubtract(this->m_Data, rhs.m_Data);
			return *this;
		}

		FORCE_INLINE const FVector operator *(const float& rhs) const
		{
			return FVector(Intrinsics::XVectorScale(this->m_Data, rhs));
		}
		FORCE_INLINE const FVector operator *(const FVector& rhs) const 
		{
			return FVector(Intrinsics::XVectorMultiply(this->m_Data, rhs.m_Data));
		}
		FORCE_INLINE FVector& operator *=(const FVector& rhs)
		{
			this->m_Data = Intrinsics::XVectorMultiply(this->m_Data, rhs.m_Data);
			return *this;
		}

		FORCE_INLINE const FVector operator /(const FVector& rhs) const 
		{
			return FVector(Intrinsics::XVectorDivide(this->m_Data, rhs.m_Data));
		}
		FORCE_INLINE FVector& operator /=(const FVector& rhs)
		{
			this->m_Data = Intrinsics::XVectorDivide(this->m_Data, rhs.m_Data);
			return *this;
		}

		FORCE_INLINE bool operator ==(const FVector& rhs) const
		{
			struct Buffer
			{
				int vals[4];
			};
			const Buffer* rc_lhs = reinterpret_cast<const Buffer*>(this);
			const Buffer* rc_rhs = reinterpret_cast<const Buffer*>(&rhs);

			bool equal = rc_lhs->vals == rc_rhs->vals;
			return equal;

			//bool c = reinterpret_cast<const Buffer*>(&this->m_Data) == reinterpret_cast<const Buffer*>(&rhs.m_Data);
			//return (memcmp(&this->m_Data, &rhs.m_Data, sizeof(XVector)) == 0);
		}

		FORCE_INLINE bool operator !=(const FVector& rhs) const
		{
			return !( (*this) == rhs );
		}

		/*
			Returns the length of this.
		*/
		float Length() const
		{
			return 0.f;
			//return sqrtf( Dot(*this, *this) );
		}

		/*
			Returns the squared length of this.
		*/
		float LengthSquared() const
		{
			return 	Dot(*this, *this);
		}

		/*
			Returns a copy of the normalized vector of this.
		*/
		FORCE_INLINE FVector Normalized() const
		{
			float LengthFactor = 1.0f / this->Length();
			return Intrinsics::XVectorScale(m_Data, LengthFactor);
		}

		// -----------------
		// static Funcitons	|
		// -----------------

		/*
			Returns the vector perpendicular to A and B.
		*/
		static FORCE_INLINE FVector Cross(const FVector& A, const FVector& B)
		{
			// Unpack to arrays
			float AVec[8];
			Intrinsics::XVectorUnpackToArray(AVec, sizeof(AVec), A.m_Data);
			float BVec[8];
			Intrinsics::XVectorUnpackToArray(BVec, sizeof(BVec), B.m_Data);
			// Cross product logic
			float X = AVec[1] * BVec[2] - AVec[2] * BVec[1];
			float Y = AVec[2] * BVec[0] - AVec[0] * BVec[2];
			float Z = AVec[0] * BVec[1] - AVec[1] * BVec[0];
			// Construct a new vector
			return FVector(X, Y, Z);
		}

		/*
			Returns the dot product of A and B.
		*/
		static FORCE_INLINE float Dot(const FVector& A, const FVector& B)
		{
			XVector Multiply = Intrinsics::XVectorMultiply(A.m_Data, B.m_Data);
			return	Intrinsics::GetXComponent(Multiply) + 
					Intrinsics::GetYComponent(Multiply) + 
					Intrinsics::GetZComponent(Multiply);
		}
		
		static const FVector One;
		static const FVector Zero;
		static const FVector Up;
		static const FVector Down;
		static const FVector Left;
		static const FVector Right;
		static const FVector Forward;
		static const FVector Backward;
	};

}
