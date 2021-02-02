#pragma once

namespace Insight {

	namespace GameFramework {
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

	typedef shared_ptr<GameFramework::AActor> StrongActorPtr;
	typedef weak_ptr<GameFramework::AActor> WeakActorPtr;
	typedef shared_ptr<GameFramework::ActorComponent> StrongActorComponentPtr;
	typedef unique_ptr<GameFramework::ActorComponent> UniqueActorComponentPtr;
	typedef weak_ptr<GameFramework::ActorComponent> WeakActorComponentPtr;
	typedef shared_ptr<Model> StrongModelPtr;
	typedef weak_ptr<Model> WeakModelPtr;
	typedef shared_ptr<Texture> StrongTexturePtr;
	typedef weak_ptr<Texture> WeakTexturePtr;

	typedef enum class RenderPassType
	{
		RenderPassType_Invalid,
		RenderPassType_Scene,
		RenderPassType_Shadow,
		RenderPassType_Transparency,
		RenderPassType_UI,
	} RenderPassType;

}
