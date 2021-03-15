#pragma once

#include <Runtime/Core.h>

namespace Insight
{
	template <typename ComponentType>
	struct TVector2Base
	{
		ComponentType X, Y;


		// Constructors
		//
		constexpr TVector2Base(ComponentType Value)
			: X(Value), Y(Value), Z(Value) { }
		constexpr TVector2Base(ComponentType X, ComponentType Y)
			: X(X), Y(Y) { }
		constexpr TVector2Base()
		{
			memset(this, 0, sizeof(TVector2Base<ComponentType>));
		}

		// Copy Contructor
		//
		constexpr TVector2Base(const TVector2Base& Vec)
		{
			this->X = Vec.X;
			this->Y = Vec.Y;
		}

		// Destructor
		//
		~TVector2Base() = default;

		/*
			Returns the size of this vector in bytes.
		*/
		inline constexpr size_t SizeInBytes()
		{
			constexpr size_t Size = sizeof(this->X) + sizeof(this->Y);
			return Size;
		}

		inline void SetX(ComponentType X) { this->X = X; }
		inline void SetY(ComponentType Y) { this->Y = Y; }

		inline constexpr void ZeroComponents()
		{
			this->X = static_cast<ComponentType>(0);
			this->Y = static_cast<ComponentType>(0);
		}


		bool operator ==(const TVector2Base& A)
		{
			bool X = this->X == A.X;
			bool Y = this->Y == A.Y;
			return (X && Y);
		}

		bool operator !=(const TVector2Base& A)
		{
			bool IsSame = (*this) == A;
			return (IsSame == false);
		}

		TVector2Base operator + (const TVector2Base& rhs)
		{
			float X = this->X + rhs.X;
			float Y = this->Y + rhs.Y;
			return TVector2Base(X, Y);
		}
		TVector2Base& operator += (const TVector2Base& rhs)
		{
			this->X += rhs.X;
			this->Y += rhs.Y;
			return *this;
		}
		TVector2Base operator - (const TVector2Base& rhs)
		{
			float X = this->X - rhs.X;
			float Y = this->Y - rhs.Y;
			return TVector2Base(X, Y, Z);
		}
		TVector2Base& operator -= (const TVector2Base& rhs)
		{
			this->X -= rhs.X;
			this->Y -= rhs.Y;
			return *this;
		}
		TVector2Base operator * (const TVector2Base& rhs)
		{
			float X = this->X * rhs.X;
			float Y = this->Y * rhs.Y;
			return TVector2Base(X, Y, Z);
		}
		TVector2Base& operator *= (const TVector2Base& rhs)
		{
			this->X *= rhs.X;
			this->Y *= rhs.Y;
			return *this;
		}
		TVector2Base operator / (const TVector2Base& rhs)
		{
			float X = this->X / rhs.X;
			float Y = this->Y / rhs.Y;
			return TVector2Base(X, Y, Z);
		}
		TVector2Base& operator /= (const TVector2Base& rhs)
		{
			this->X /= rhs.X;
			this->Y /= rhs.Y;
			return *this;
		}


	};
}