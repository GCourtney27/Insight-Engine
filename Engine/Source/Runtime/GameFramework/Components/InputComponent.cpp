// Copyright Insight Interactive. All Rights Reserved.
#include "Engine_pch.h"

#include "InputComponent.h"

namespace Insight {
	
	namespace GameFramework {


		InputComponent::InputComponent(AActor* pOwner)
			: ActorComponent("Input Component", pOwner),
			m_GamepadLeftStickSensitivity(DEFAULT_THUMBSTICK_LEFT_SENSITIVITY),
			m_GamepadRightStickSensitivity(DEFAULT_THUMBSTICK_RIGHT_SENSITIVITY)
		{
		}

		InputComponent::~InputComponent()
		{
		}

		void InputComponent::OnInit()
		{
		}

		void InputComponent::OnDestroy()
		{
		}

		void InputComponent::OnRender()
		{
		}

		void InputComponent::BeginPlay()
		{
		}

		void InputComponent::EditorEndPlay()
		{
		}

		void InputComponent::Tick(const float DeltaMs)
		{
		}

		void InputComponent::OnAttach()
		{
		}

		void InputComponent::OnDetach()
		{
		}

		void InputComponent::BindAction(const char* ActionName, InputEventType EventType, Input::EventInputActionFn Callback)
		{
			Input::InputDispatcher::Get().RegisterActionCallback(ActionName, EventType, Callback);
		}

		void InputComponent::AddGamepadVibration(uint32_t PlayerIndex, GampadRumbleMotor Motor, float Amount)
		{
			Input::InputDispatcher::Get().AddGamepadVibration(PlayerIndex, Motor, Amount);
		}

		void InputComponent::BindAxis(const char* AxisName, Input::EventInputAxisFn Callback)
		{
			Input::InputDispatcher::Get().RegisterAxisCallback(AxisName, Callback);
		}

	}
}
