// Copyright Insight Interactive. All Rights Reserved.
#pragma once

#include "Runtime/Input/PlatformKeyCodes.h"

namespace Insight {


	typedef enum _InputEventType
	{
		InputEventType_Pressed	= 0,
		InputEventType_Released	= 1,
		InputEventType_Held		= 2,
		InputEventType_Typed	= 3,
		InputEventType_Moved	= 4,
	} InputEventType;

	
	typedef int KeyMapCode;

	// Mouse
	// -----
	// Buttons
	static KeyMapCode KeyMapCode_Mouse_Button_Left = ConstPlatformInputCodes::PlatformMouseCode_Button_Left;
	static KeyMapCode KeyMapCode_Mouse_Button_Right = ConstPlatformInputCodes::PlatformMouseCode_Button_Right;
	static KeyMapCode KeyMapCode_Mouse_Button_Middle = ConstPlatformInputCodes::PlatformMouseCode_Button_Middle;
	static KeyMapCode KeyMapCode_Mouse_Wheel_Up = 3;
	static KeyMapCode KeyMapCode_Mouse_Wheel_Down = 4;
	static KeyMapCode KeyMapCode_Mouse_Wheel_Left = 5;
	static KeyMapCode KeyMapCode_Mouse_Wheel_Right = 6;
	static KeyMapCode KeyMapCode_Mouse_MoveX = 7;
	static KeyMapCode KeyMapCode_Mouse_MoveY = 8;


	// Keyboard
	// --------
	// Functions
	static KeyMapCode KeyMapCode_Keyboard_Shift = ConstPlatformInputCodes::PlatformKeyboardCode_Shift;
	static KeyMapCode KeyMapCode_Keyboard_Shift_Left = ConstPlatformInputCodes::PlatformKeyboardCode_Shift_Left;
	static KeyMapCode KeyMapCode_Keyboard_Shift_Right = ConstPlatformInputCodes::PlatformKeyboardCode_Shift_Right;
	static KeyMapCode KeyMapCode_Keyboard_Alt = ConstPlatformInputCodes::PlatformKeyboardCode_Alt;
	static KeyMapCode KeyMapCode_Keyboard_Control = ConstPlatformInputCodes::PlatformKeyboardCode_Control;
	static KeyMapCode KeyMapCode_Keyboard_Arrow_Left = ConstPlatformInputCodes::PlatformKeyboardCode_Arrow_Left;
	static KeyMapCode KeyMapCode_Keyboard_Arrow_Right = ConstPlatformInputCodes::PlatformKeyboardCode_Arrow_Right;
	static KeyMapCode KeyMapCode_Keyboard_Arrow_Up = ConstPlatformInputCodes::PlatformKeyboardCode_Arrow_Up;
	static KeyMapCode KeyMapCode_Keyboard_Arrow_Down = ConstPlatformInputCodes::PlatformKeyboardCode_Arrow_Down;
	// Numbers
	static KeyMapCode KeyMapCode_Keyboard_0 = ConstPlatformInputCodes::PlatformKeyboardCode_0;
	static KeyMapCode KeyMapCode_Keyboard_1 = ConstPlatformInputCodes::PlatformKeyboardCode_1;
	static KeyMapCode KeyMapCode_Keyboard_2 = ConstPlatformInputCodes::PlatformKeyboardCode_2;
	static KeyMapCode KeyMapCode_Keyboard_3 = ConstPlatformInputCodes::PlatformKeyboardCode_3;
	static KeyMapCode KeyMapCode_Keyboard_4 = ConstPlatformInputCodes::PlatformKeyboardCode_4;
	static KeyMapCode KeyMapCode_Keyboard_5 = ConstPlatformInputCodes::PlatformKeyboardCode_5;
	static KeyMapCode KeyMapCode_Keyboard_6 = ConstPlatformInputCodes::PlatformKeyboardCode_6;
	static KeyMapCode KeyMapCode_Keyboard_7 = ConstPlatformInputCodes::PlatformKeyboardCode_7;
	static KeyMapCode KeyMapCode_Keyboard_8 = ConstPlatformInputCodes::PlatformKeyboardCode_8;
	static KeyMapCode KeyMapCode_Keyboard_9 = ConstPlatformInputCodes::PlatformKeyboardCode_9;
	// Letters
	static KeyMapCode KeyMapCode_Keyboard_A = ConstPlatformInputCodes::PlatformKeyboardCode_A;
	static KeyMapCode KeyMapCode_Keyboard_B = ConstPlatformInputCodes::PlatformKeyboardCode_B;
	static KeyMapCode KeyMapCode_Keyboard_C = ConstPlatformInputCodes::PlatformKeyboardCode_C;
	static KeyMapCode KeyMapCode_Keyboard_D = ConstPlatformInputCodes::PlatformKeyboardCode_D;
	static KeyMapCode KeyMapCode_Keyboard_E = ConstPlatformInputCodes::PlatformKeyboardCode_E;
	static KeyMapCode KeyMapCode_Keyboard_F = ConstPlatformInputCodes::PlatformKeyboardCode_F;
	static KeyMapCode KeyMapCode_Keyboard_G = ConstPlatformInputCodes::PlatformKeyboardCode_G;
	static KeyMapCode KeyMapCode_Keyboard_H = ConstPlatformInputCodes::PlatformKeyboardCode_H;
	static KeyMapCode KeyMapCode_Keyboard_I = ConstPlatformInputCodes::PlatformKeyboardCode_I;
	static KeyMapCode KeyMapCode_Keyboard_J = ConstPlatformInputCodes::PlatformKeyboardCode_J;
	static KeyMapCode KeyMapCode_Keyboard_K = ConstPlatformInputCodes::PlatformKeyboardCode_K;
	static KeyMapCode KeyMapCode_Keyboard_L = ConstPlatformInputCodes::PlatformKeyboardCode_L;
	static KeyMapCode KeyMapCode_Keyboard_M = ConstPlatformInputCodes::PlatformKeyboardCode_M;
	static KeyMapCode KeyMapCode_Keyboard_N = ConstPlatformInputCodes::PlatformKeyboardCode_N;
	static KeyMapCode KeyMapCode_Keyboard_O = ConstPlatformInputCodes::PlatformKeyboardCode_O;
	static KeyMapCode KeyMapCode_Keyboard_P = ConstPlatformInputCodes::PlatformKeyboardCode_P;
	static KeyMapCode KeyMapCode_Keyboard_Q = ConstPlatformInputCodes::PlatformKeyboardCode_Q;
	static KeyMapCode KeyMapCode_Keyboard_R = ConstPlatformInputCodes::PlatformKeyboardCode_R;
	static KeyMapCode KeyMapCode_Keyboard_S = ConstPlatformInputCodes::PlatformKeyboardCode_S;
	static KeyMapCode KeyMapCode_Keyboard_T = ConstPlatformInputCodes::PlatformKeyboardCode_T;
	static KeyMapCode KeyMapCode_Keyboard_U = ConstPlatformInputCodes::PlatformKeyboardCode_U;
	static KeyMapCode KeyMapCode_Keyboard_V = ConstPlatformInputCodes::PlatformKeyboardCode_V;
	static KeyMapCode KeyMapCode_Keyboard_W = ConstPlatformInputCodes::PlatformKeyboardCode_W;
	static KeyMapCode KeyMapCode_Keyboard_X = ConstPlatformInputCodes::PlatformKeyboardCode_X;
	static KeyMapCode KeyMapCode_Keyboard_Y = ConstPlatformInputCodes::PlatformKeyboardCode_Y;
	static KeyMapCode KeyMapCode_Keyboard_Z = ConstPlatformInputCodes::PlatformKeyboardCode_Z;


	// Gamepad
	// -------
	// Buttons
	static KeyMapCode GamepadCode_Button_A = ConstPlatformInputCodes::PlatformGamepadButtonCode_A;
	static KeyMapCode GamepadCode_Button_B				= ConstPlatformInputCodes::PlatformGamepadButtonCode_B;
	static KeyMapCode GamepadCode_Button_X				= ConstPlatformInputCodes::PlatformGamepadButtonCode_X;
	static KeyMapCode GamepadCode_Button_Y				= ConstPlatformInputCodes::PlatformGamepadButtonCode_Y;
	static KeyMapCode GamepadCode_Button_DPad_Up			= ConstPlatformInputCodes::PlatformGamepadButtonCode_DPad_Up;
	static KeyMapCode GamepadCode_Button_DPad_Down		= ConstPlatformInputCodes::PlatformGamepadButtonCode_DPad_Down;
	static KeyMapCode GamepadCode_Button_DPad_Left		= ConstPlatformInputCodes::PlatformGamepadButtonCode_DPad_Left;
	static KeyMapCode GamepadCode_Button_DPad_Right		= ConstPlatformInputCodes::PlatformGamepadButtonCode_DPad_Right;
	static KeyMapCode GamepadCode_Button_Start			= ConstPlatformInputCodes::PlatformGamepadButtonCode_Start;
	static KeyMapCode GamepadCode_Button_Back				= ConstPlatformInputCodes::PlatformGamepadButtonCode_Back;
	static KeyMapCode GamepadCode_Button_Thumbstick_Left	= ConstPlatformInputCodes::PlatformGamepadButtonCode_Thumbstick_Left;
	static KeyMapCode GamepadCode_Button_Thumbstick_Right	= ConstPlatformInputCodes::PlatformGamepadButtonCode_Thumbstick_Right;
	static KeyMapCode GamepadCode_Button_Shoulder_Left	= ConstPlatformInputCodes::PlatformGamepadButtonCode_Shoulder_Left;
	static KeyMapCode GamepadCode_Button_Shoulder_Right	= ConstPlatformInputCodes::PlatformGamepadButtonCode_Shoulder_Right;
	// Trigger
	static KeyMapCode GamepadCode_Trigger_Left = ConstPlatformInputCodes::PlatformGamepadTriggerCode_Left;
	static KeyMapCode GamepadCode_Trigger_Right = ConstPlatformInputCodes::PlatformGamepadTriggerCode_Right;
	// Thumbstick Axis
	static KeyMapCode GamepadCode_Thumbstick_Left_Axis_X = ConstPlatformInputCodes::PlatformGamepadThumbstick_Left_Axis_X;
	static KeyMapCode GamepadCode_Thumbstick_Left_Axis_Y = ConstPlatformInputCodes::PlatformGamepadThumbstick_Left_Axis_Y;
	static KeyMapCode GamepadCode_Thumbstick_Right_Axis_X = ConstPlatformInputCodes::PlatformGamepadThumbstick_Right_Axis_X;
	static KeyMapCode GamepadCode_Thumbstick_Right_Axis_Y = ConstPlatformInputCodes::PlatformGamepadThumbstick_Right_Axis_Y;



	typedef enum _GampadRumbleMotor
	{
		GampadRumbleMotor_Left	= BIT_SHIFT(0),
		GampadRumbleMotor_Right = BIT_SHIFT(1),
	} GampadRumbleMotor;

}

