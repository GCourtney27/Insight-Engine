#include <Engine_pch.h>

#include "Runtime/Core/Public/ieObject/Components/ieControllerComponent.h"

namespace Insight
{

	void ieControllerComponent::BindAction(const char* ActionName, EInputEventType EventType, Input::EventInputActionFn Callback)
	{
		Input::InputDispatcher::Get().RegisterActionCallback(ActionName, EventType, Callback);
	}

	void ieControllerComponent::AddGamepadVibration(uint32_t PlayerIndex, EGampadRumbleMotor Motor, float Amount)
	{
		Input::InputDispatcher::Get().AddGamepadVibration(PlayerIndex, Motor, Amount);
	}

	void ieControllerComponent::BindAxis(const char* AxisName, Input::EventInputAxisFn Callback)
	{
		Input::InputDispatcher::Get().RegisterAxisCallback(AxisName, Callback);
	}
}
