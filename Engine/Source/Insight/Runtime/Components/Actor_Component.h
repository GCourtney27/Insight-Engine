#pragma once
#include <Insight/Core.h>

namespace Insight {


#define RETURN_IF_COMPONENT_DISABLED if(!m_Enabled){return;}

	class ActorComponent
	{
	public:
		virtual ~ActorComponent(void) { m_pOwner = nullptr; }

		virtual bool LoadFromJson(const rapidjson::Value& JsonComponent) = 0;
		virtual bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer) = 0;

		virtual void OnInit() = 0;
		virtual void OnPostInit() {}
		virtual void OnDestroy() = 0;
		virtual void CalculateParent(const DirectX::XMMATRIX& Matrix) = 0;
		virtual void OnRender() = 0;
		virtual void OnUpdate(const float& DeltaTime) {}
		virtual void OnChanged() {}
		virtual void OnImGuiRender() = 0;
		virtual void RenderSceneHeirarchy() = 0;

		virtual void BeginPlay() = 0;
		virtual void Tick(const float& DeltaMs) = 0;

		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;

		//void SetEventCallbackFunction()
		const bool& GetIsComponentEnabled() const { return m_Enabled; }
		void SetComponentEnabled(bool Enable) { m_Enabled = Enable; }

		virtual const char* GetName() const { return m_ComponentName; };

		void SetOwner(AActor* Owner) { m_pOwner = Owner; }
	protected:
		ActorComponent(const char* ComponentName, AActor* Owner)
			: m_ComponentName(ComponentName), m_pOwner(Owner) {}
	protected:
		AActor* m_pOwner;
		const char* m_ComponentName;
		bool m_Enabled = true;


	};
}

