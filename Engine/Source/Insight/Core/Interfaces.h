#pragma once

#include <SimpleMath.h>
using namespace DirectX::SimpleMath;

namespace Insight {


	class AActor;
	class ActorComponent;
	class Model;

	typedef unsigned int ActorId;
	typedef unsigned int ComponentId;
	typedef unsigned int ModelId;

	const ActorId INVALID_ACTOR_ID = 0;
	const ComponentId INVALID_COMPONENT_ID = 0;

	typedef shared_ptr<AActor> StrongActorPtr;
	typedef weak_ptr<AActor> WeakActorPtr;
	typedef shared_ptr<ActorComponent> StrongActorComponentPtr;
	typedef unique_ptr<ActorComponent> UniqueActorComponentPtr;
	typedef weak_ptr<ActorComponent> WeakActorComponentPtr;
	typedef shared_ptr<Model> StrongModelPtr;
	typedef weak_ptr<Model> WeakModelPtr;

	enum RenderPass
	{
		RenderPass_0,
		RenderPass_Static = RenderPass_0,
		RenderPass_Actor,
		RenderPass_Sky,
		RenderPass_NotRendered,
		RenderPass_Last,
		RenderPass_UI
	};

}
