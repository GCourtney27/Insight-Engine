#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Insight/Core.h"
#include "Insight/Math/Vector.h"

#include <DirectXMath.h>

namespace Insight {

	using namespace DirectX;

	class INSIGHT_API Transform
	{
	public:
		Transform(Vector3 position, Vector3 rotation, Vector3 scale)
			: m_Position(position), m_Rotation(rotation), m_Scale(scale) {}

	private:
		Vector3 m_Position;
		Vector3 m_Rotation;
		Vector3 m_Scale;

		Vector3 m_LocalForwardVector;
		Vector3 m_LocalBackwardVector;
		Vector3 m_LocalLeftVector;
		Vector3 m_LocalRightVector;
		Vector3 m_LocalUpVector;
		Vector3 m_LocalDownVector;

	};

}

#endif // !TRANSFORM_H

