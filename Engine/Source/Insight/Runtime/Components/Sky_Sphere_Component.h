#pragma once

#include <Insight/Core.h>

#include "Actor_Component.h"


namespace Insight {

	class INSIGHT_API SkySphereComponent : public ActorComponent
	{
	public:
		SkySphereComponent(StrongActorPtr pOwner);
		virtual ~SkySphereComponent();



	private:
		Texture m_Diffuse;
		Texture m_Irradience;
		Texture m_EnvironmentMap;
		Texture m_brdfLUT;
	};

}
