#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Insight/Core.h"

#include <DirectXMath.h>

namespace Insight {

	using namespace DirectX;

	static XMVECTOR WorldForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	static XMVECTOR WorldBackward = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	static XMVECTOR WorldLeft = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	static XMVECTOR WorldRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	static XMVECTOR WorldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	static XMVECTOR WorldDown = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);

	class INSIGHT_API Transform
	{
	public:
		Transform(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale) {}

		static const XMVECTOR WorldForward;
		static const XMVECTOR WorldBackward;
		static const XMVECTOR WorldLeft;
		static const XMVECTOR WorldRight;
		static const XMVECTOR WorldUp;
		static const XMVECTOR WorldDown;

	private:
		XMFLOAT3 m_Position = {};
		XMVECTOR m_PositionVector;

		XMFLOAT3 m_Rotation = {};
		XMVECTOR m_RotationVector;

		XMFLOAT3 m_Scale = {};
		XMVECTOR m_ScaleVector;

		XMVECTOR m_LocalForwardVector;
		XMVECTOR m_LocalBackwardVector;
		XMVECTOR m_LocalLeftVector;
		XMVECTOR m_LocalRightVector;
		XMVECTOR m_LocalUpVector;
		XMVECTOR m_LocalDownVector;

	};

}

#endif // !TRANSFORM_H

