#pragma once

#include <Runtime/Core.h>

#include "Runtime/Math/ie_Vectors.h"


namespace Insight {

	using namespace Math;

	namespace Physics {


		class INSIGHT_API Ray
		{
		public:
			Ray() {}
			Ray(const ieVector3& Position, const ieVector3& Direction)
				: m_Position(Position), m_Direction(Direction) {}

			ieVector3 Orgin() const { return m_Position; }
			ieVector3 Direction() const { return m_Direction; }
			ieVector3 PointAtParameter(float t) const { return m_Position + t * m_Direction; }

			ieVector3 m_Position;
			ieVector3 m_Direction;
		};
	}

}

