#pragma once

#include <Runtime/Core.h>

#include "Runtime/Core/Public/ieObject/Components/ieComponentBase.h"
#include "Runtime/Core/Public/Input/InputDispatcher.h"

namespace Insight
{
	static const float kThumbStickLeftSensitivity	= 20.0f;
	static const float kThumbStickRightSensitivity	= 20.0f;

	class INSIGHT_API ieControllerComponent : public ieComponentBase<ieControllerComponent>
	{
	public:
		ieControllerComponent()
		{
		}
		virtual ~ieControllerComponent()
		{
		}

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



	protected:
		
		float m_GamepadLeftStickSensitivity;
		float m_GamepadRightStickSensitivity;

	};
}
