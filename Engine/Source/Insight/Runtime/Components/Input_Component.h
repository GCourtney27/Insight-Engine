// Copyright Insight Interactive. All Rights Reserved.
#pragma once

#include "Actor_Component.h"
#include "Insight/Input/Key_Codes.h"
#include "Insight/Input/Input_Dispatcher.h"

namespace Insight {

	namespace Runtime {

		class INSIGHT_API InputComponent : public ActorComponent
		{
		public:
			typedef void(*OutVoidInFloatFn_t)(float);
			typedef void(*OutVoidInVoidFn_t)(void);

			using Super = ActorComponent;

		public:
			InputComponent(AActor* pOwner);
			virtual ~InputComponent();
			
			virtual bool LoadFromJson(const rapidjson::Value& JsonComponent) override { return true; }
			virtual bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer) override{ return true; }

			virtual void SetEventCallback(const EventCallbackFn& callback) override{}
			virtual void OnEvent(Event& e) override{}

			virtual void OnImGuiRender() override{}
			virtual void RenderSceneHeirarchy() override{}

			virtual void OnInit() override;
			virtual void OnPostInit() {}
			virtual void OnDestroy() override;
			virtual void OnRender() override;

			virtual void BeginPlay() override;
			virtual void EditorEndPlay() override;
			virtual void Tick(const float DeltaMs) override;

			virtual void OnAttach() override;
			virtual void OnDetach() override;


			void BindAxis(const char* AxisName, Input::EventInputAxisFn Callback);
			void BindAction(const char* ActionName, InputEventType EventType, Input::EventInputActionFn Callback);


		private:


		};
	}

}

