// Copyright Insight Interactive. All Rights Reserved.
#pragma once

#include "ActorComponent.h"
#include "Runtime/Input/KeyCodes.h"
#include "Runtime/Input/InputDispatcher.h"

namespace Insight {

	namespace GameFramework {

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

			/*
				Bind a function to an input axis event defined in the user settings.
				@param AxisName: The name of the axis to bind to.
				@param Callback: A pointer to the funciton to call once the Axis has been invoked.
			*/
			void BindAxis(const char* AxisName, Input::EventInputAxisFn Callback);
			/*
				Bind a function to an input action event defined in the user settings.
				@param ActionName: The name of the action to bind to.
				@param EventType: An event type that must be satisfied before the Callback can be invoked.
				@param Callback: A pointer to the funciton to call once the action has been invoked.
			*/
			void BindAction(const char* ActionName, EInputEventType EventType, Input::EventInputActionFn Callback);
			/*
				Add vibration to a players gamepad.
				@param PlayerIndex: The index of the player to add vibration to.
				@param Motor: The motor to add vibration to. This can be either the left or right gamepad motor.
				@param Amount: A normalized value (0 - 1) that specifies the amount of vibration to add to the controller. 0 being no vibration and 1 being full vibration.
			*/
			void AddGamepadVibration(uint32_t PlayerIndex, EGampadRumbleMotor Motor, float Amount);

		private:
			const float DEFAULT_THUMBSTICK_LEFT_SENSITIVITY = 20.0f;
			const float DEFAULT_THUMBSTICK_RIGHT_SENSITIVITY = 20.0f;
			float m_GamepadLeftStickSensitivity;
			float m_GamepadRightStickSensitivity;

		};
	}

}

