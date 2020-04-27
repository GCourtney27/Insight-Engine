#pragma once

#include "Insight/Core.h"

#include "Insight/Math/Transform.h"


namespace Insight {



	class INSIGHT_API Actor
	{
	public:
		Actor();
		~Actor();

		virtual void OnInit();
		virtual void OnUpdate();
		virtual void OnRender();

		const inline Transform& GetTransform() const { return m_Transform; }

	protected:
		const Vector3 WORLD_DIRECTION = WORLD_DIRECTION.Zero;
		Transform m_Transform;
		// TODO: component list
	private:
		
	};

}
