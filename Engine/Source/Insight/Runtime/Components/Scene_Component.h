#pragma once

#include <Insight/Core.h>

#include "Insight/Math/Transform.h"


namespace Insight {

	class INSIGHT_API SceneComponent
	{
	public:
		SceneComponent();
		~SceneComponent();

		inline const Transform& GetTransform() const { return m_Transfrom; }
		inline Transform& GetTransformRef() { return m_Transfrom; }

	private:
		Transform m_Transfrom;
	};

}
