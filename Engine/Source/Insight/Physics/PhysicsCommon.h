#pragma once

#include "Insight/Math/ie_Vectors.h"

namespace Insight {

	using namespace Math;

	class INSIGHT_API IPhysicsObject
	{
	public:
		enum class eColliderType
		{
			INVALID,
			SPHERE,
			BOX
		};
		using ColliderType = IPhysicsObject::eColliderType;

	public:

		eColliderType GetColliderType() { return m_ColliderType; }

	protected:
		ColliderType m_ColliderType = ColliderType::INVALID;
		bool m_IsStatic = false;
	};

	struct Contact 
	{
		ieVector3 HitPoint;
		ieVector3 HitNormal;
		float Distance;
		IPhysicsObject* ObjectOne;
		IPhysicsObject* ObjectTwo;

		Contact(ieVector3 _HitPoint, ieVector3 _HitNormal, float _Distance, IPhysicsObject* _ObjectOne, IPhysicsObject* _ObjectTwo)
			: HitPoint(_HitPoint), HitNormal(_HitNormal), Distance(_Distance), ObjectOne(_ObjectOne), ObjectTwo(_ObjectTwo) {}
	};

}
