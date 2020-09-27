#pragma once

namespace Retina {

	namespace Runtime {
		class AActor;
		class ActorComponent;
	}
	class Model;
	class Texture;

	typedef unsigned int ActorId;
	typedef unsigned int ComponentId;
	typedef unsigned int ModelId;

	const ActorId INVALID_ACTOR_ID = 0;
	const ComponentId INVALID_COMPONENT_ID = 0;

	typedef shared_ptr<Runtime::AActor> StrongActorPtr;
	typedef weak_ptr<Runtime::AActor> WeakActorPtr;
	typedef shared_ptr<Runtime::ActorComponent> StrongActorComponentPtr;
	typedef unique_ptr<Runtime::ActorComponent> UniqueActorComponentPtr;
	typedef weak_ptr<Runtime::ActorComponent> WeakActorComponentPtr;
	typedef shared_ptr<Model> StrongModelPtr;
	typedef weak_ptr<Model> WeakModelPtr;
	typedef shared_ptr<Texture> StrongTexturePtr;
	typedef weak_ptr<Texture> WeakTexturePtr;

	enum class eRenderPass
	{
		RenderPass_0,
		RenderPass_Scene,
		RenderPass_Shadow,
		RenderPass_Transparency,
	};

}
