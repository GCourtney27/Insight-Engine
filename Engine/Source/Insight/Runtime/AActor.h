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
			typedef std::vector<StrongActorComponentPtr> ActorComponents;

		public:
			AActor(ActorId Id, ActorName ActorName = "MyActor");
			virtual ~AActor();

			virtual bool LoadFromJson(const rapidjson::Value& jsonActor) override;
			bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer) override;

			// Editor
			virtual void RenderSceneHeirarchy();
			virtual void OnImGuiRender();

			virtual bool OnInit();
			virtual bool OnPostInit();
			virtual void OnUpdate(const float DeltaMs);
			virtual void CalculateParent(XMMATRIX parentMat);
			virtual void OnRender();
			virtual void Destroy();

			void OnEvent(Event& e);

			virtual void BeginPlay();
			virtual void Tick(const float DeltaMs);
			virtual void Exit();

			ActorId GetId() { return m_Id; }
		public:
			template<typename Component>
			StrongActorComponentPtr CreateDefaultSubobject()
			{
				StrongActorComponentPtr component = std::make_shared<Component>(this);
				IE_CORE_ASSERT(component, "Trying to add null component to actor");

				component->OnAttach();
				component->SetEventCallback(IE_BIND_EVENT_FN(AActor::OnEvent));

				m_Components.push_back(component);
				m_NumComponents++;
				return component;
			}
			template<typename Event>
			WeakActorComponentPtr GetSubobject()
			{
				WeakActorComponentPtr component = nullptr;
				for (UniqueActorComponentPtr _component : m_components)
				{
					component = dynamic_cast<Event>(_component);
					if (component != nullptr) break;
				}
				return component;
			}
			void RemoveSubobject(StrongActorComponentPtr component);
			void RemoveAllSubobjects();
			std::vector<StrongActorComponentPtr> GetAllSubobjects() const { return m_Components; }
		private:
			bool OnCollision(PhysicsEvent& e);
		protected:
			ActorComponents m_Components;
			uint32_t m_NumComponents = 0;
			ActorId m_Id;
		private:

		};

	} // end namespace Runtime
} // end namespace Insight
