#pragma once
#include <Insight/Core.h>

#include "Insight/Events/Event.h"
#include "Insight/Math/Transform.h"
#include "Insight/Core/Scene/Scene_Node.h"
#include "Insight/Runtime/Components/Scene_Component.h"
#include "Insight/Events/Application_Event.h"


namespace Insight {

	namespace Runtime {

		typedef std::string ActorType;
		typedef std::string ActorName;

		class INSIGHT_API AActor : public SceneNode
		{
		public:
			typedef std::vector<ActorComponent*> ActorComponents;

		public:
			AActor(ActorId Id, ActorName ActorName = "MyActor");
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
				Component->SetEventCallback(IE_BIND_EVENT_FN(AActor::OnEvent));

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

	} // end namespace Runtime
} // end namespace Insight
