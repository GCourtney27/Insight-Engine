#pragma once

// -------------------------------------
// Define platform specific key codes. |
// -------------------------------------

#if defined IE_PLATFORM_WINDOWS

// Mouse Codes
// -----------
// Mouse Buttons
#define PlatformMouseCode_Button_Left	 VK_LBUTTON
#define PlatformMouseCode_Button_Right	 VK_RBUTTON
#define PlatformMouseCode_Button_Middle	 VK_MBUTTON

// Keyboard Codes
// --------------
// Functions
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
		KeymapCode_Mouse_Button_Left	= PlatformMouseCode_Button_Left,
		KeymapCode_Mouse_Button_Right	= PlatformMouseCode_Button_Right,
		KeymapCode_Mouse_Button_Middle	= PlatformMouseCode_Button_Middle,
		KeymapCode_Mouse_Wheel_Up		= 3,
		KeymapCode_Mouse_Wheel_Down		= 4,
		KeymapCode_Mouse_Wheel_Left		= 5,
		KeymapCode_Mouse_Wheel_Right	= 6,
		KeymapCode_Mouse_MoveX			= 7,
		KeymapCode_Mouse_MoveY			= 8,
		
		// Keyboard
		// --------
		// Functions
		KeymapCode_Keyboard_Shift_Left	= PlatformKeyboardCode_Shift_Left,
		KeymapCode_Keyboard_Shift_Right = PlatformKeyboardCode_Shift_Right,
		KeymapCode_Keyboard_Alt			= PlatformKeyboardCode_Alt,
		KeymapCode_Keyboard_Control		= PlatformKeyboardCode_Control,
		KeymapCode_Keyboard_Arrow_Left	= PlatformKeyboardCode_Arrow_Left,
		KeymapCode_Keyboard_Arrow_Right = PlatformKeyboardCode_Arrow_Right,
		KeymapCode_Keyboard_Arrow_Up	= PlatformKeyboardCode_Arrow_Up,
		KeymapCode_Keyboard_Arrow_Down	= PlatformKeyboardCode_Arrow_Down,
		// Numbers
		KeymapCode_Keyboard_0 = PlatformKeyboardCode_0,
		KeymapCode_Keyboard_1 = PlatformKeyboardCode_1,
		KeymapCode_Keyboard_2 = PlatformKeyboardCode_2,
		KeymapCode_Keyboard_3 = PlatformKeyboardCode_3,
		KeymapCode_Keyboard_4 = PlatformKeyboardCode_4,
		KeymapCode_Keyboard_5 = PlatformKeyboardCode_5,
		KeymapCode_Keyboard_6 = PlatformKeyboardCode_6,
		KeymapCode_Keyboard_7 = PlatformKeyboardCode_7,
		KeymapCode_Keyboard_8 = PlatformKeyboardCode_8,
		KeymapCode_Keyboard_9 = PlatformKeyboardCode_9,
		// Letters
		KeymapCode_Keyboard_A = PlatformKeyboardCode_A,
		KeymapCode_Keyboard_B = PlatformKeyboardCode_B,
		KeymapCode_Keyboard_C = PlatformKeyboardCode_C,
		KeymapCode_Keyboard_D = PlatformKeyboardCode_D,
		KeymapCode_Keyboard_E = PlatformKeyboardCode_E,
		KeymapCode_Keyboard_F = PlatformKeyboardCode_F,
		KeymapCode_Keyboard_G = PlatformKeyboardCode_G,
		KeymapCode_Keyboard_H = PlatformKeyboardCode_H,
		KeymapCode_Keyboard_I = PlatformKeyboardCode_I,
		KeymapCode_Keyboard_J = PlatformKeyboardCode_J,
		KeymapCode_Keyboard_K = PlatformKeyboardCode_K,
		KeymapCode_Keyboard_L = PlatformKeyboardCode_L,
		KeymapCode_Keyboard_M = PlatformKeyboardCode_M,
		KeymapCode_Keyboard_N = PlatformKeyboardCode_N,
		KeymapCode_Keyboard_O = PlatformKeyboardCode_O,
		KeymapCode_Keyboard_P = PlatformKeyboardCode_P,
		KeymapCode_Keyboard_Q = PlatformKeyboardCode_Q,
		KeymapCode_Keyboard_R = PlatformKeyboardCode_R,
		KeymapCode_Keyboard_S = PlatformKeyboardCode_S,
		KeymapCode_Keyboard_T = PlatformKeyboardCode_T,
		KeymapCode_Keyboard_U = PlatformKeyboardCode_U,
		KeymapCode_Keyboard_V = PlatformKeyboardCode_V,
		KeymapCode_Keyboard_W = PlatformKeyboardCode_W,
		KeymapCode_Keyboard_X = PlatformKeyboardCode_X,
		KeymapCode_Keyboard_Y = PlatformKeyboardCode_Y,
		KeymapCode_Keyboard_Z = PlatformKeyboardCode_Z,

		
		
		// TODO Gamepad
		
	} KeymapCode;

}

