#pragma once
#include <Insight/Core.h>

#include "Insight/Math/Transform.h"
#include "Insight/Core/Scene/Scene_Node.h"
#include "Insight/Runtime/Components/Scene_Component.h"

namespace Insight {

	typedef std::string ActorType;
	typedef std::string ActorName;

	using Super = Insight::SceneNode;

	class INSIGHT_API AActor : public SceneNode
	{
	public:
		typedef std::vector<StrongActorComponentPtr> ActorComponents;

	public:
		AActor(ActorId id, ActorName actorName = "Default Actor");
		~AActor();

		void RenderSceneHeirarchy();

		virtual bool OnInit();
		virtual bool OnPostInit();
		virtual void OnUpdate(const float& deltaMs);
		virtual void OnRender();
		virtual void Destroy();

		virtual void BeginPlay();
		virtual void Tick(const float& deltaMs);
		virtual void Exit();

		const inline Transform& GetTransform() const { return m_SceneComponent.GetTransform(); }
		inline Transform& GetTransformRef() { return m_SceneComponent.GetTransformRef(); }
	public:
		template<typename T>
		T* AddComponent()
		{
			T* component = new T(this);
			assert(dynamic_cast<StrongActorComponentPtr>(component));

			m_Components.push_back(component);
			return component;
		}
		template<typename T>
		T* GetComponent()
		{
			T* component = nullptr;
			for (StrongActorComponentPtr _component : m_components)
			{
				component = dynamic_cast<T*>(_component);
				if (component != nullptr) break;
			}
			return component;
		}
		void AddComponent(StrongActorComponentPtr component);
		void RemoveComponent(StrongActorComponentPtr component);
		std::vector<StrongActorComponentPtr> GetAllComponents() const { return m_Components; }

		
	protected:
		const Vector3 WORLD_DIRECTION = WORLD_DIRECTION.Zero;
		SceneComponent m_SceneComponent;
		ActorComponents m_Components;
		ActorId m_id;
	private:
		
	};
}

