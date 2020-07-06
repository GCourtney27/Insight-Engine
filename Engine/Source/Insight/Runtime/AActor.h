#pragma once
#include <Insight/Core.h>

#include "Insight/Events/Event.h"
#include "Insight/Math/Transform.h"
#include "Insight/Core/Scene/Scene_Node.h"
#include "Insight/Runtime/Components/Scene_Component.h"


namespace Insight {

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
		virtual void OnUpdate(const float& deltaMs);
		virtual void CalculateParent(XMMATRIX parentMat);
		virtual void OnRender();
		virtual void Destroy();

		void OnEvent(Event& e);

		virtual void BeginPlay();
		virtual void Tick(const float& deltaMs);
		virtual void Exit();

		ActorId GetId() { return m_Id; }
	public:
		template<typename T>
		StrongActorComponentPtr CreateDefaultSubobject()
		{
			StrongActorComponentPtr component = std::make_shared<T>(this);
			IE_CORE_ASSERT(component, "Trying to add null component to actor");

			component->OnAttach();
			m_Components.push_back(component);
			m_NumComponents++;
			return component;
		}
		template<typename T>
		WeakActorComponentPtr GetSubobject()
		{
			WeakActorComponentPtr component = nullptr;
			for (UniqueActorComponentPtr _component : m_components)
			{
				component = dynamic_cast<T>(_component);
				if (component != nullptr) break;
			}
			return component;
		}
		void RemoveSubobject(StrongActorComponentPtr component);
		std::vector<StrongActorComponentPtr> GetAllSubobjects() const { return m_Components; }
		
	protected:
		const Vector3 WORLD_DIRECTION = WORLD_DIRECTION.Zero;
		ActorComponents m_Components;
		UINT m_NumComponents = 0;
		ActorId m_Id;
	private:

	};
}

