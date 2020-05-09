#pragma once

#include <Insight/Core.h>

#include "Insight/Math/Transform.h"
#include "Insight/Runtime/Components/Actor_Component.h"

namespace Insight {



	class INSIGHT_API AActor : public Object
	{
	public:
		AActor();
		~AActor();

		virtual void OnInit();
		virtual void OnUpdate();
		virtual void OnRender();

		virtual void BeginPlay();
		virtual void Tick();
		virtual void Destroy();

		const inline Transform& GetTransform() const { return m_Transform; }

	protected:
		const Vector3 WORLD_DIRECTION = WORLD_DIRECTION.Zero;
		std::vector<Component*> m_Components;
		Transform m_Transform;
	private:
		
	};
}

