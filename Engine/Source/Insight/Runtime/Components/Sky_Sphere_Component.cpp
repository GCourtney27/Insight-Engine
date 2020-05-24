#include <ie_pch.h>

#include "Insight/Runtime/AActor.h"
#include "Insight/Rendering/Texture.h"
#include "Sky_Sphere_Component.h"

namespace Insight {



	SkySphereComponent::SkySphereComponent(StrongActorPtr pOwner)
		: ActorComponent("Static Mesh Component", pOwner)
	{
		pOwner->SetRenderPass(RenderPass::RenderPass_Sky);
	}

	SkySphereComponent::~SkySphereComponent()
	{
	}

}