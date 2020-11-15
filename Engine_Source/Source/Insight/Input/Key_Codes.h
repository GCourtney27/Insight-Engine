// Copyright Insight Interactive. All Rights Reserved.
#pragma once

#include "Insight/Input/Platform_Key_Codes.h"

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

	//typedef enum _KeyMapCode
	//{
		// Mouse
		// -----
		// Buttons
		KeyMapCode_Mouse_Button_Left	= PlatformMouseCode_Button_Left,
		KeyMapCode_Mouse_Button_Right	= PlatformMouseCode_Button_Right,
		KeyMapCode_Mouse_Button_Middle	= PlatformMouseCode_Button_Middle,
		KeyMapCode_Mouse_Wheel_Up		= 3,
		KeyMapCode_Mouse_Wheel_Down		= 4,
		KeyMapCode_Mouse_Wheel_Left		= 5,
		KeyMapCode_Mouse_Wheel_Right	= 6,
		KeyMapCode_Mouse_MoveX			= 7,
		KeyMapCode_Mouse_MoveY			= 8,


		// Keyboard
		// --------
		// Functions
		KeyMapCode_Keyboard_Shift		= PlatformKeyboardCode_Shift,
		KeyMapCode_Keyboard_Shift_Left	= PlatformKeyboardCode_Shift_Left,
		KeyMapCode_Keyboard_Shift_Right = PlatformKeyboardCode_Shift_Right,
		KeyMapCode_Keyboard_Alt			= PlatformKeyboardCode_Alt,
		KeyMapCode_Keyboard_Control		= PlatformKeyboardCode_Control,
		KeyMapCode_Keyboard_Arrow_Left	= PlatformKeyboardCode_Arrow_Left,
		KeyMapCode_Keyboard_Arrow_Right	= PlatformKeyboardCode_Arrow_Right,
		KeyMapCode_Keyboard_Arrow_Up	= PlatformKeyboardCode_Arrow_Up,
		KeyMapCode_Keyboard_Arrow_Down	= PlatformKeyboardCode_Arrow_Down,
		// Numbers
		KeyMapCode_Keyboard_0 = PlatformKeyboardCode_0,
		KeyMapCode_Keyboard_1 = PlatformKeyboardCode_1,
		KeyMapCode_Keyboard_2 = PlatformKeyboardCode_2,
		KeyMapCode_Keyboard_3 = PlatformKeyboardCode_3,
		KeyMapCode_Keyboard_4 = PlatformKeyboardCode_4,
		KeyMapCode_Keyboard_5 = PlatformKeyboardCode_5,
		KeyMapCode_Keyboard_6 = PlatformKeyboardCode_6,
		KeyMapCode_Keyboard_7 = PlatformKeyboardCode_7,
		KeyMapCode_Keyboard_8 = PlatformKeyboardCode_8,
		KeyMapCode_Keyboard_9 = PlatformKeyboardCode_9,
		// Letters
		KeyMapCode_Keyboard_A = PlatformKeyboardCode_A,
		KeyMapCode_Keyboard_B = PlatformKeyboardCode_B,
		KeyMapCode_Keyboard_C = PlatformKeyboardCode_C,
		KeyMapCode_Keyboard_D = PlatformKeyboardCode_D,
		KeyMapCode_Keyboard_E = PlatformKeyboardCode_E,
		KeyMapCode_Keyboard_F = PlatformKeyboardCode_F,
		KeyMapCode_Keyboard_G = PlatformKeyboardCode_G,
		KeyMapCode_Keyboard_H = PlatformKeyboardCode_H,
		KeyMapCode_Keyboard_I = PlatformKeyboardCode_I,
		KeyMapCode_Keyboard_J = PlatformKeyboardCode_J,
		KeyMapCode_Keyboard_K = PlatformKeyboardCode_K,
		KeyMapCode_Keyboard_L = PlatformKeyboardCode_L,
		KeyMapCode_Keyboard_M = PlatformKeyboardCode_M,
		KeyMapCode_Keyboard_N = PlatformKeyboardCode_N,
		KeyMapCode_Keyboard_O = PlatformKeyboardCode_O,
		KeyMapCode_Keyboard_P = PlatformKeyboardCode_P,
		KeyMapCode_Keyboard_Q = PlatformKeyboardCode_Q,
		KeyMapCode_Keyboard_R = PlatformKeyboardCode_R,
		KeyMapCode_Keyboard_S = PlatformKeyboardCode_S,
		KeyMapCode_Keyboard_T = PlatformKeyboardCode_T,
		KeyMapCode_Keyboard_U = PlatformKeyboardCode_U,
		KeyMapCode_Keyboard_V = PlatformKeyboardCode_V,
		KeyMapCode_Keyboard_W = PlatformKeyboardCode_W,
		KeyMapCode_Keyboard_X = PlatformKeyboardCode_X,
		KeyMapCode_Keyboard_Y = PlatformKeyboardCode_Y,
		KeyMapCode_Keyboard_Z = PlatformKeyboardCode_Z,


		// Gamepad
		// -------
		// Buttons
		GamepadCode_Button_A				= PlatformGamepadButtonCode_A,
		GamepadCode_Button_B				= PlatformGamepadButtonCode_B,
		GamepadCode_Button_X				= PlatformGamepadButtonCode_X,
		GamepadCode_Button_Y				= PlatformGamepadButtonCode_Y,
		GamepadCode_Button_DPad_Up			= PlatformGamepadButtonCode_DPad_Up,
		GamepadCode_Button_DPad_Down		= PlatformGamepadButtonCode_DPad_Down,
		GamepadCode_Button_DPad_Left		= PlatformGamepadButtonCode_DPad_Left,
		GamepadCode_Button_DPad_Right		= PlatformGamepadButtonCode_DPad_Right,
		GamepadCode_Button_Start			= PlatformGamepadButtonCode_Start,
		GamepadCode_Button_Back				= PlatformGamepadButtonCode_Back,
		GamepadCode_Button_Thumbstick_Left	= PlatformGamepadButtonCode_Thumbstick_Left,
		GamepadCode_Button_Thumbstick_Right	= PlatformGamepadButtonCode_Thumbstick_Right,
		GamepadCode_Button_Shoulder_Left	= PlatformGamepadButtonCode_Shoulder_Left,
		GamepadCode_Button_Shoulder_Right	= PlatformGamepadButtonCode_Shoulder_Right,
		// Trigger
		GamepadCode_Trigger_Left			= PlatformGamepadTriggerCode_Left,
		GamepadCode_Trigger_Right			= PlatformGamepadTriggerCode_Right,
		// Thumbstick Axis
		GamepadCode_Thumbstick_Left_Axis_X	= PlatformGamepadThumbstick_Left_Axis_X,
		GamepadCode_Thumbstick_Left_Axis_Y	= PlatformGamepadThumbstick_Left_Axis_Y,
		GamepadCode_Thumbstick_Right_Axis_X	= PlatformGamepadThumbstick_Right_Axis_X,
		GamepadCode_Thumbstick_Right_Axis_Y	= PlatformGamepadThumbstick_Right_Axis_Y,


	//} KeyMapCode;

	typedef enum _GampadRumbleMotor
	{
		GampadRumbleMotor_Left	= BIT_SHIFT(0),
		GampadRumbleMotor_Right = BIT_SHIFT(1),
	} GampadRumbleMotor;

}

