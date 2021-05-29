#include <Engine_pch.h>

#include "Runtime/Core/Public/ieObject/Components/ieControllerComponent.h"

namespace Insight
{
	void ieControllerComponent::AddGamepadVibration(uint32_t PlayerIndex, EGampadRumbleMotor Motor, float Amount)
	{
		Input::InputDispatcher::Get().AddGamepadVibration(PlayerIndex, Motor, Amount);
	}
}
