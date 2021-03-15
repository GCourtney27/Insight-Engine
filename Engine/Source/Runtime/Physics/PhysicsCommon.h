#pragma once


namespace Insight {


	class INSIGHT_API IPhysicsObject
	{
	public:
		enum class EColliderType
		{
			CT_INVALID,
			CT_SPHERE,
			CT_BOX
		};
		using ColliderType = IPhysicsObject::EColliderType;

	public:

		EColliderType GetColliderType() { return m_ColliderType; }

	protected:
		ColliderType m_ColliderType = EColliderType::CT_INVALID;
		bool m_IsStatic = false;
	};

	struct Contact 
	{
		FVector3 HitPoint;
		FVector3 HitNormal;
		float Distance;
		IPhysicsObject* ObjectOne;
		IPhysicsObject* ObjectTwo;

		Contact(FVector3 _HitPoint, FVector3 _HitNormal, float _Distance, IPhysicsObject* _ObjectOne, IPhysicsObject* _ObjectTwo)
			: HitPoint(_HitPoint), HitNormal(_HitNormal), Distance(_Distance), ObjectOne(_ObjectOne), ObjectTwo(_ObjectTwo) {}
	};

}
