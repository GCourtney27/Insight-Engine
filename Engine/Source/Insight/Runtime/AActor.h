#pragma once
#include <Insight/Core.h>

#include "Insight/Math/Transform.h"

namespace Insight {

	typedef std::string ActorType;

	
	class INSIGHT_API AActor
	{
	public:
		typedef std::vector<StrongActorComponentPtr> ActorComponents;

	public:
		AActor(ActorId id);
		~AActor();

		virtual bool OnInit();
		virtual void OnPostInit();
		virtual void OnUpdate(const float& deltaMs);
		virtual void OnRender();

		virtual void BeginPlay();
		virtual void Tick();
		virtual void Destroy();

		const inline Transform& GetTransform() const { return m_Transform; }
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
		ActorComponents m_Components;
		Transform m_Transform;
		ActorId m_id;
	private:
		
	};
}

