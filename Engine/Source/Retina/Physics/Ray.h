#pragma once

#include <Retina/Core.h>

#include "Retina/Math/ie_Vectors.h"


namespace Retina {

	using namespace Math;

	namespace Physics {


		class RETINA_API Ray
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

