#pragma once

#include <Runtime/Core.h>

namespace Insight 
{


	template <typename ComponentType>
	struct TVector3Base
	{
		// Main Components
		//
		ComponentType X, Y, Z;

	public:
		// Constructors
		//
		constexpr TVector3Base(ComponentType& Value)
			: X(Value), Y(Value), Z(Value) { }
		constexpr TVector3Base(ComponentType&& Value)
			: X(Value), Y(Value), Z(Value) { }
		constexpr TVector3Base(ComponentType& X, ComponentType& Y, ComponentType& Z)
			: X(X), Y(Y), Z(Z) { }
		constexpr TVector3Base(ComponentType&& X, ComponentType&& Y, ComponentType&& Z)
			: X(X), Y(Y), Z(Z) { }
		constexpr TVector3Base()
		{
			memset(this, 0, sizeof(TVector3Base<ComponentType>));
		}

		// Copy Contructor
		//
		constexpr TVector3Base(const TVector3Base& Vec)
		{
			this->X = Vec.X;
			this->Y = Vec.Y;
			this->Z = Vec.Z;
		}

		// Move Constructor
		//
		constexpr TVector3Base(TVector3Base&& Vec) = default;

		// Destructor
		//
		~TVector3Base() = default;

		static inline constexpr size_t SizeInBytes()
		{
			return sizeof(TVector3Base<ComponentType>);;
		}

		FORCE_INLINE constexpr void SetX(ComponentType X) { this->X = X; }
		FORCE_INLINE constexpr void SetY(ComponentType Y) { this->Y = Y; }
		FORCE_INLINE constexpr void SetZ(ComponentType Z) { this->Z = Z; }

		FORCE_INLINE void SetXYZ(ComponentType X, ComponentType Y, ComponentType Z)
		{
			this->X = X;
			this->Y = Y;
			this->Z = Z;
		}

		FORCE_INLINE constexpr void ZeroComponents()
		{
			this->X = static_cast<ComponentType>(0);
			this->Y = static_cast<ComponentType>(0);
			this->Z = static_cast<ComponentType>(0);
		}

		FORCE_INLINE TVector3Base& operator =(const TVector3Base& A)
		{
			this->X = A.X;
			this->Y = A.Y;
			this->Z = A.Z;
			return *this;
		}

		FORCE_INLINE bool operator ==(const TVector3Base& A)
		{
			bool X = this->X == A.X;
			bool Y = this->Y == A.Y;
			bool Z = this->Z == A.Z;
			return (X && Y && Z);
		}

		FORCE_INLINE bool operator !=(const TVector3Base& A)
		{
			bool IsSame = (*this) == A;
			return (IsSame == false);
		}

		FORCE_INLINE TVector3Base operator + (const TVector3Base& rhs)
		{
			float X = this->X + rhs.X;
			float Y = this->Y + rhs.Y;
			float Z = this->Z + rhs.Z;
			return TVector3Base(X, Y, Z);
		}
		FORCE_INLINE TVector3Base& operator += (const TVector3Base& rhs)
		{
			this->X += rhs.X;
			this->Y += rhs.Y;
			this->Z += rhs.Z;
			return *this;
		}
		FORCE_INLINE TVector3Base operator +(const ComponentType& rhs)
		{
			float X = this->X + rhs;
			float Y = this->Y + rhs;
			float Z = this->Z + rhs;
			return TVector3Base(X, Y, Z);
		}
		FORCE_INLINE TVector3Base operator - (const TVector3Base& rhs)
		{
			float X = this->X - rhs.X;
			float Y = this->Y - rhs.Y;
			float Z = this->Z - rhs.Z;
			return TVector3Base(X, Y, Z);
		}
		FORCE_INLINE TVector3Base& operator -= (const TVector3Base& rhs)
		{
			this->X -= rhs.X;
			this->Y -= rhs.Y;
			this->Z -= rhs.Z;
			return *this;
		}
		FORCE_INLINE TVector3Base operator -(const ComponentType& rhs)
		{
			float X = this->X - rhs;
			float Y = this->Y - rhs;
			float Z = this->Z - rhs;
			return TVector3Base(X, Y, Z);
		}
		FORCE_INLINE TVector3Base operator * (const TVector3Base& rhs)
		{
			float X = this->X * rhs.X;
			float Y = this->Y * rhs.Y;
			float Z = this->Z * rhs.Z;
			return TVector3Base(X, Y, Z);
		}
		FORCE_INLINE TVector3Base& operator *= (const TVector3Base& rhs)
		{
			this->X *= rhs.X;
			this->Y *= rhs.Y;
			this->Z *= rhs.Z;
			return *this;
		}
		FORCE_INLINE TVector3Base operator *(const ComponentType& rhs)
		{
			float X = this->X * rhs;
			float Y = this->Y * rhs;
			float Z = this->Z * rhs;
			return TVector3Base(X, Y, Z);
		}
		FORCE_INLINE TVector3Base operator / (const TVector3Base& rhs)
		{
			float X = this->X / rhs.X;
			float Y = this->Y / rhs.Y;
			float Z = this->Z / rhs.Z;
			return TVector3Base(X, Y, Z);
		}
		FORCE_INLINE TVector3Base& operator /= (const TVector3Base& rhs)
		{
			this->X /= rhs.X;
			this->Y /= rhs.Y;
			this->Z /= rhs.Z;
			return *this;
		}
		FORCE_INLINE TVector3Base operator /(const ComponentType& rhs)
		{
			float X = this->X / rhs;
			float Y = this->Y / rhs;
			float Z = this->Z / rhs;
			return TVector3Base(X, Y, Z);
		}


		// ---------------------
		//		Operations		|
		// ---------------------

		/*
			Returns the length of this.
		*/
		FORCE_INLINE float Length() const 
		{
			const float X2 = this->X * this->X;
			const float Y2 = this->Y * this->Y;
			const float Z2 = this->Z * this->Z;
			return sqrtf(X2 + Y2 + Z2);
		}

		/*
			Returns the squared length of this.
		*/
		FORCE_INLINE float LengthSquared() const 
		{
			const TVector3Base Vec = Dot(this);
			return Vec.X + Vec.Y + Vec.Z;
		}

		/*
			Normalizes this vector.
		*/
		FORCE_INLINE void Normalize()
		{
			float LengthFactor = 1.0f / this->Length();
			this->X *= LengthFactor;
			this->Y *= LengthFactor;
			this->Z *= LengthFactor;
		}

		// -------------------------
		//		Static Functions	|
		// -------------------------

		/*
			Returns the vector perpendicular to A and B.
		*/
		static FORCE_INLINE TVector3Base Cross(const TVector3Base& A, const TVector3Base& B)
		{
			float X = A.Y * B.Z - A.Z * B.Y;
			float Y = A.Z * B.X - A.X * B.Z;
			float Z = A.X * B.Y - A.Y * B.X;
			return TVector3Base(X, Y, Z);
		}

		/*
			Returns the dot product of A and B.
		*/
		static FORCE_INLINE float Dot(const TVector3Base& A, const TVector3Base& B)
		{
			float X = A.X * B.X;
			float Y = A.Y * B.Y;
			float Z = A.Z * B.Z;
			return (X + Y + Z);
		}

		/*
			Returns the normalized vector of A.
		*/
		static FORCE_INLINE TVector3Base Normalize(const TVector3Base& A)
		{
			float LengthFactor = 1.0f / A.Length();
			return TVector3Base(LengthFactor * A.X, LengthFactor * A.Y, LengthFactor * A.Z);
		}
		
		/*
			Returns the reflected vector V and N where N is the normal.
		*/
		static FORCE_INLINE TVector3Base Reflect(const TVector3Base& V, const TVector3Base& N)
		{
			TVector3Base Normal = TVector3Base::Normalize(N);
			TVector3Base TwoAoN = TVector3Base::Dot(V, Normal) * static_cast<ComponentType>(2);
			return (TwoAoN * Normal) - V;
		}

		/* 
			World Direction
		*/
		static const TVector3Base<ComponentType> Up;
		static const TVector3Base<ComponentType> Down;
		static const TVector3Base<ComponentType> Left;
		static const TVector3Base<ComponentType> Right;
		static const TVector3Base<ComponentType> Forward;
		static const TVector3Base<ComponentType> Backward;

		/*
			Utility
		*/
		static const TVector3Base<ComponentType> Zero;
		static const TVector3Base<ComponentType> One;
	};

	/*
		World Direction
	*/
	template <typename ComponentType>
	const TVector3Base<ComponentType> TVector3Base<ComponentType>::Up( 0,  1,  0 );
	template <typename ComponentType>
	const TVector3Base<ComponentType> TVector3Base<ComponentType>::Down{ 0, -1,  0 };
	template <typename ComponentType>
	const TVector3Base<ComponentType> TVector3Base<ComponentType>::Left{ -1,  0,  0 };
	template <typename ComponentType>
	const TVector3Base<ComponentType> TVector3Base<ComponentType>::Right{ 1,  0,  0 };
	template <typename ComponentType>
	const TVector3Base<ComponentType> TVector3Base<ComponentType>::Forward{ 0,  0,  1 };
	template <typename ComponentType>
	const TVector3Base<ComponentType> TVector3Base<ComponentType>::Backward{ 0,  0, -1 };

	/*
		Utility
	*/
	template <typename ComponentType>
	const TVector3Base<ComponentType> TVector3Base<ComponentType>::Zero{ 0,  0,  0 };
	template <typename ComponentType>
	const TVector3Base<ComponentType> TVector3Base<ComponentType>::One{ 1,  1,  1 };
}

