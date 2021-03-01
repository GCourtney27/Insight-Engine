#pragma once
#include <Runtime/CoreMacros.h>

#include "Runtime/Events/Event.h"
#include "Runtime/Math/Transform.h"
#include "Runtime/Core/Scene/SceneNode.h"
#include "Runtime/GameFramework/Components/SceneComponent.h"
#include "Runtime/Events/ApplicationEvent.h"


namespace Insight {

	namespace GameFramework {

		typedef EString ActorType;
		typedef EString ActorName;

		class INSIGHT_API AActor : public SceneNode
		{
		public:
			typedef std::vector<ActorComponent*> ActorComponents;

		public:
			AActor(ActorId Id, ActorName ActorName = TEXT("MyActor"));
			virtual ~AActor();

			virtual bool LoadFromJson(const rapidjson::Value* jsonActor) override;
			bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>* Writer) override;

			// Editor
			virtual void RenderSceneHeirarchy();
			virtual void OnImGuiRender();

			virtual bool OnInit();
			virtual bool OnPostInit();
			virtual void OnUpdate(const float DeltaMs);
			virtual void OnRender();
			virtual void Destroy();

			void OnEvent(Event& e);

			virtual void BeginPlay();
			virtual void EditorEndPlay() override;
			virtual void Tick(const float DeltaMs);
			virtual void Exit();

			ActorId GetId() { return m_Id; }
		public:
			template<typename ComponentType>
			ComponentType* CreateDefaultSubobject()
			{
				ActorComponent* Component = new ComponentType(this);
				IE_ASSERT(Component, "Trying to add null component to actor.");

				Component->OnAttach();
				Component->SetEventCallback(IE_BIND_LOCAL_EVENT_FN(AActor::OnEvent));

				m_Components.push_back(Component);
				m_NumComponents++;
				return (ComponentType*)Component;
			}
			template<typename ComponentType>
			ComponentType* GetSubobject()
			{
				ComponentType* component = nullptr;
				for (ActorComponent* _component : m_Components)
				{
					component = dynamic_cast<ComponentType*>(_component);
					if (component != nullptr) break;
				}
				return component;
			}
			void RemoveSubobject(ActorComponent* component);
			void RemoveAllSubobjects();
			std::vector<ActorComponent*> GetAllSubobjects() const { return m_Components; }
		private:
			bool OnCollision(PhysicsEvent& e);
		protected:
			ActorComponents m_Components;
			uint32_t m_NumComponents;
			ActorId m_Id;
			float m_DeltaMs;
		private:

		};

	} // end namespace GameFramework
} // end namespace Insight
