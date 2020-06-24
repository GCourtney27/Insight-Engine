#pragma once
#include <Insight/Core.h>

#include "Actor_Component.h"

#include <mono/metadata/debug-helpers.h>

namespace Insight {

	class MonoScriptManager;

	class INSIGHT_API CSharpScriptComponent : public ActorComponent
	{
	public:
		
	public:
		CSharpScriptComponent(AActor* pOwner);
		~CSharpScriptComponent();

		virtual bool LoadFromJson(const rapidjson::Value& jsonCSScriptComponent) override;

		virtual void OnInit() override;
		virtual void OnPostInit() override;
		virtual void OnDestroy() override;
		virtual void OnPreRender(const DirectX::XMMATRIX& matrix) override;
		virtual void OnUpdate(const float& deltaTime);
		virtual void OnRender() override;
		virtual void OnChanged() override;
		virtual void OnImGuiRender() override;
		virtual void RenderSceneHeirarchy() override;

		void BeginPlay();
		void Tick(const float& deltaMs);

		virtual void OnAttach() override;
		virtual void OnDetach() override;
	private:
		void UpdateScriptFields();
		void ProcessScriptTransformChanges();
		void GetTransformFields();
		void RegisterScript();
		void Cleanup();
	private:
		MonoScriptManager* m_pMonoScriptManager = nullptr;
		MonoClass* m_pClass = nullptr;
		MonoObject* m_pObject = nullptr;
		MonoMethod* m_pBeginPlayMethod = nullptr;
		MonoMethod* m_pUpdateMethod = nullptr;

		std::string m_ModuleName;
		bool m_CanBeTicked = true;
		bool m_CanBeCalledOnBeginPlay = true;

		// Transform Script Fields
		MonoObject* m_TransformObject;

		MonoClassField* m_XPositionField;
		MonoClassField* m_YPositionField;
		MonoClassField* m_ZPositionField;
		MonoObject* m_PositionObj;

		MonoClassField* m_XRotationField;
		MonoClassField* m_YRotationField;
		MonoClassField* m_ZRotationField;
		MonoObject* m_RotationObj;
		
		MonoClassField* m_XScaleField;
		MonoClassField* m_YScaleField;
		MonoClassField* m_ZScaleField;
		MonoObject* m_ScaleObj;

	};

}
