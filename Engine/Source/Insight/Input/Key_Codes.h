#pragma once

namespace Insight {


	typedef enum InputEventType
	{
		InputEventType_Pressed	= 0,
		InputEventType_Released	= 1,
		InputEventType_Typed	= 2,
		InputEventType_Moved	= 3,
	} InputEventType;


	typedef enum KeymapCode
	{
		// Mouse
		KeymapCode_Mouse_Button_Left	= 0,
		KeymapCode_Mouse_Button_Right	= 1,
		KeymapCode_Mouse_Button_Middle	= 2,
		KeymapCode_Mouse_Wheel_Up		= 3,
		KeymapCode_Mouse_Wheel_Down		= 4,
		KeymapCode_Mouse_Wheel_Left		= 5,
		KeymapCode_Mouse_Wheel_Right	= 6,
		KeymapCode_Mouse_MoveX			= 7,
		KeymapCode_Mouse_MoveY			= 8,

		// Keyboard
		KeymapCode_Keyboard_A = 65,
		KeymapCode_Keyboard_B = 66,
		KeymapCode_Keyboard_C = 67,
		KeymapCode_Keyboard_D = 68,
		KeymapCode_Keyboard_E = 69,
		KeymapCode_Keyboard_F = 70,
		KeymapCode_Keyboard_G = 71,
		KeymapCode_Keyboard_H = 72,
		KeymapCode_Keyboard_I = 73,
		KeymapCode_Keyboard_J = 74,
		KeymapCode_Keyboard_K = 75,
		KeymapCode_Keyboard_L = 76,
		KeymapCode_Keyboard_M = 77,
		KeymapCode_Keyboard_N = 78,
		KeymapCode_Keyboard_O = 79,
		KeymapCode_Keyboard_P = 80,
		KeymapCode_Keyboard_Q = 81,
		KeymapCode_Keyboard_R = 82,
		KeymapCode_Keyboard_S = 83,
		KeymapCode_Keyboard_T = 84,
		KeymapCode_Keyboard_U = 85,
		KeymapCode_Keyboard_V = 86,
		KeymapCode_Keyboard_W = 87,
		KeymapCode_Keyboard_X = 88,
		KeymapCode_Keyboard_Y = 89,
		KeymapCode_Keyboard_Z = 90,
		
		// TODO Gamepad
		
	} KeymapCode;

}
