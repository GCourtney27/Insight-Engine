#pragma once
#include <Insight/Core.h>

namespace Insight {


	using Super = Insight::ActorComponent;
	
	class ActorComponent
	{
	public:
		virtual ~ActorComponent(void) { m_pOwner.reset(); }

		virtual bool LoadFromJson(const rapidjson::Value& jsonComponent) = 0;

		virtual void OnInit() = 0;
		virtual void OnPostInit() {}
		virtual void OnDestroy() = 0;
		virtual void OnPreRender(const DirectX::XMMATRIX& matrix) = 0;
		virtual void OnRender() = 0;
		virtual void OnUpdate(const float& deltaTime) {}
		virtual void OnChanged() {}
		virtual void OnImGuiRender() = 0;
		virtual void RenderSceneHeirarchy() = 0;

		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;

		//void SetEventCallbackFunction()
		const bool& GetIsComponentEnabled() const { return m_Enabled; }
		void SetComponentEnabled(bool enable) { m_Enabled = enable; }

		virtual const char* GetName() const { return m_ComponentName; };
		
		void SetOwner(StrongActorPtr owner) { m_pOwner = owner; }
	protected:
		ActorComponent(const char* componentName, StrongActorPtr owner)
			: m_ComponentName(componentName), m_pOwner(owner) {}
	protected:
		StrongActorPtr m_pOwner;
		const char* m_ComponentName;
		bool m_Enabled = true;


	};
}

