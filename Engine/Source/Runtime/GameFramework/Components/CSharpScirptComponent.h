#pragma once
#include <Runtime/CoreMacros.h>

#include "ActorComponent.h"

//#include <mono/metadata/debug-helpers.h>

namespace Insight {

	class MonoScriptManager;

	namespace GameFramework {


		class INSIGHT_API CSharpScriptComponent : public ActorComponent
		{
		public:
			struct EventData
			{
				EventCallbackFn EventCallback;

			};
		public:
			CSharpScriptComponent(AActor* pOwner);
			virtual ~CSharpScriptComponent();

			virtual bool LoadFromJson(const rapidjson::Value& jsonCSScriptComponent) override;
			virtual bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer) override;

			virtual inline void SetEventCallback(const EventCallbackFn& callback) override { m_EventData.EventCallback = callback; }
			void OnEvent(Event& e);

			virtual void OnInit() override;
			virtual void OnPostInit() override;
			virtual void OnDestroy() override;
			virtual void OnUpdate(const float deltaTime);
			virtual void OnRender() override;
			virtual void OnChanged() override;
			virtual void OnImGuiRender() override;
			virtual void RenderSceneHeirarchy() override;

			virtual void BeginPlay() override;
			virtual void EditorEndPlay() override;
			virtual void Tick(const float DeltaMs) override;

			virtual void OnAttach() override;
			virtual void OnDetach() override;

			void Cleanup();
			void RegisterScript();


		private:
			void UpdateScriptFields();
			void ProcessScriptTransformChanges();
			void GetTransformFields();
		private:
			//MonoScriptManager* m_pMonoScriptManager = nullptr;
			//MonoClass* m_pClass = nullptr;
			//MonoObject* m_pObject = nullptr;
			//MonoMethod* m_pBeginPlayMethod = nullptr;
			//MonoMethod* m_pUpdateMethod = nullptr;

			std::string m_ModuleName;
			bool m_CanBeTicked = true;
			bool m_CanBeCalledOnBeginPlay = true;
			uint32_t m_ScriptWorldIndex = 0U;
			EventData m_EventData;

			// Transform Script Fields
			//MonoObject* m_TransformObject;

			//MonoClassField* m_XPositionField;
			//MonoClassField* m_YPositionField;
			//MonoClassField* m_ZPositionField;
			//MonoObject* m_PositionObj;

			//MonoClassField* m_XRotationField;
			//MonoClassField* m_YRotationField;
			//MonoClassField* m_ZRotationField;
			//MonoObject* m_RotationObj;

			//MonoClassField* m_XScaleField;
			//MonoClassField* m_YScaleField;
			//MonoClassField* m_ZScaleField;
			//MonoObject* m_ScaleObj;
		private:
			static uint32_t s_NumActiveCSScriptComponents;

		};

	} // end namespace GameFramework
} // end namespace Insight
