#include "ie_pch.h"

#include "Scene_Nodes.h"

namespace Insight {



	SceneNodeProperties::SceneNodeProperties()
	{
		m_ActorId = INVALID_ACTOR_ID;
		m_Radius = 0;
		m_RenderPass = RenderPass_0;
		m_AlphaType = AlphaOpaque;
	}

	void SceneNodeProperties::Transform(Matrix* toWorld, Matrix* fromWorld) const
	{
		if (toWorld)
			*toWorld = m_ToWorld;

		if (fromWorld)
			*fromWorld = m_FromWorld;
	}

}