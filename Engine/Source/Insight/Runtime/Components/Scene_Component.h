#pragma once

#include <Insight/Core.h>

#include "Insight/Math/Transform.h"


namespace Insight {

	class INSIGHT_API SceneComponent
	{
	public:
		SceneComponent();
		~SceneComponent();

		inline const ieTransform& GetTransform() const { return m_Transfrom; }
		inline ieTransform& GetTransformRef() { return m_Transfrom; }

	private:
		ieTransform m_Transfrom;
	};

}
