// Copyright Insight Interactive. All Rights Reserved.
#pragma once

// -------------------------------------
// Define platform specific key codes. |
// -------------------------------------

#if defined (IE_PLATFORM_WINDOWS) || defined (IE_PLATFORM_UWP)

// Mouse Codes
// -----------
// Mouse Buttons
#define PlatformMouseCode_Button_Left	 VK_LBUTTON
#define PlatformMouseCode_Button_Right	 VK_RBUTTON
#define PlatformMouseCode_Button_Middle	 VK_MBUTTON

// Keyboard Codes
// --------------
// Functions
#define PlatformKeyboardCode_Shift			VK_SHIFT
#define PlatformKeyboardCode_Shift_Left		VK_LSHIFT
#define PlatformKeyboardCode_Shift_Right	VK_RSHIFT
#define PlatformKeyboardCode_Alt			VK_MENU
#define PlatformKeyboardCode_Control		VK_CONTROL
#define PlatformKeyboardCode_Arrow_Left		VK_LEFT
#define PlatformKeyboardCode_Arrow_Right	VK_RIGHT
#define PlatformKeyboardCode_Arrow_Up		VK_UP
#define PlatformKeyboardCode_Arrow_Down		VK_DOWN
// Numbers
#define PlatformKeyboardCode_0	VK_NUMPAD0
#define PlatformKeyboardCode_1	VK_NUMPAD1
#define PlatformKeyboardCode_2	VK_NUMPAD2
#define PlatformKeyboardCode_3	VK_NUMPAD3
#define PlatformKeyboardCode_4	VK_NUMPAD4
#define PlatformKeyboardCode_5	VK_NUMPAD5
#define PlatformKeyboardCode_6	VK_NUMPAD6
#define PlatformKeyboardCode_7	VK_NUMPAD7
#define PlatformKeyboardCode_8	VK_NUMPAD8
#define PlatformKeyboardCode_9	VK_NUMPAD9
// Letters
#define PlatformKeyboardCode_A	0x41
#define PlatformKeyboardCode_B	0x42
#define PlatformKeyboardCode_C	0x43
#define PlatformKeyboardCode_D	0x44
#define PlatformKeyboardCode_E	0x45
#define PlatformKeyboardCode_F	0x46
#define PlatformKeyboardCode_G	0x47
#define PlatformKeyboardCode_H	0x48
#define PlatformKeyboardCode_I	0x49
#define PlatformKeyboardCode_J	0x4A
#define PlatformKeyboardCode_K	0x4B
#define PlatformKeyboardCode_L	0x4C
#define PlatformKeyboardCode_M	0x4D
#define PlatformKeyboardCode_N	0x4E
#define PlatformKeyboardCode_O	0x4F
#define PlatformKeyboardCode_P	0x50
#define PlatformKeyboardCode_Q	0x51
#define PlatformKeyboardCode_R	0x52
#define PlatformKeyboardCode_S	0x53
#define PlatformKeyboardCode_T	0x54
#define PlatformKeyboardCode_U	0x55
#define PlatformKeyboardCode_V	0x56
#define PlatformKeyboardCode_W	0x57
#define PlatformKeyboardCode_X	0x58
#define PlatformKeyboardCode_Y	0x59
#define PlatformKeyboardCode_Z	0x5A

// Gamepad Codes
// -------------
#define XBoxCode_Button_A 0x1000
#define XBoxCode_Button_B 0x2000
#define XBoxCode_Button_X 0x4000
#define XBoxCode_Button_Y 0x8000

#elif defined IE_PLATFORM_MAC
// Mac platform codes
#endif

namespace Insight {


	typedef enum InputEventType
	{
		InputEventType_Pressed	= 0,
		InputEventType_Released	= 1,
		InputEventType_Typed	= 2,
		InputEventType_Moved	= 3,
	} InputEventType;


	typedef enum KeyMapCode
	{
		// Mouse
		// -----
		// Buttons
		KeyMapCode_Mouse_Button_Left = PlatformMouseCode_Button_Left,
		KeyMapCode_Mouse_Button_Right = PlatformMouseCode_Button_Right,
		KeyMapCode_Mouse_Button_Middle = PlatformMouseCode_Button_Middle,
		KeyMapCode_Mouse_Wheel_Up = 3,
		KeyMapCode_Mouse_Wheel_Down = 4,
		KeyMapCode_Mouse_Wheel_Left = 5,
		KeyMapCode_Mouse_Wheel_Right = 6,
		KeyMapCode_Mouse_MoveX = 7,
		KeyMapCode_Mouse_MoveY = 8,


		// Keyboard
		// --------
		// Functions
		KeyMapCode_Keyboard_Shift = PlatformKeyboardCode_Shift,
		KeyMapCode_Keyboard_Shift_Left = PlatformKeyboardCode_Shift_Left,
		KeyMapCode_Keyboard_Shift_Right = PlatformKeyboardCode_Shift_Right,
		KeyMapCode_Keyboard_Alt = PlatformKeyboardCode_Alt,
		KeyMapCode_Keyboard_Control = PlatformKeyboardCode_Control,
		KeyMapCode_Keyboard_Arrow_Left = PlatformKeyboardCode_Arrow_Left,
		KeyMapCode_Keyboard_Arrow_Right = PlatformKeyboardCode_Arrow_Right,
		KeyMapCode_Keyboard_Arrow_Up = PlatformKeyboardCode_Arrow_Up,
		KeyMapCode_Keyboard_Arrow_Down = PlatformKeyboardCode_Arrow_Down,
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
		GamepadCode_Button_A = 91,
		GamepadCode_Button_B = 92,
		GamepadCode_Button_X = 93,
		GamepadCode_Button_Y = 94,
		GamepadCode_Button_DPad_Up = 95,
		GamepadCode_Button_DPad_Down = 96,
		GamepadCode_Button_DPad_Left = 97,
		GamepadCode_Button_DPad_Right = 97,
		GamepadCode_Button_Start = 98,
		GamepadCode_Button_Back = 99,
		GamepadCode_Button_Thubstick_Left = 100,
		GamepadCode_Button_Thubstick_Right = 101,
		GamepadCode_Button_Shoulder_Left = 102,
		GamepadCode_Button_Shoulder_Right = 103,
		// Trigger
		GamepadCode_Trigger_Left = 104,
		GamepadCode_Trigger_Right = 105,
		// Thumbstick Axis
		GamepadCode_Thumbstick_Left_Axis_X = 106,
		GamepadCode_Thumbstick_Left_Axis_Y = 107,
		GamepadCode_Thumbstick_Right_Axis_X = 108,
		GamepadCode_Thumbstick_Right_Axis_Y = 109,
		GamepadCode_Misc_1 = 110,
		GamepadCode_Misc_2 = 111,
		GamepadCode_Misc_3 = 112,


	} KeymapCode;

}

