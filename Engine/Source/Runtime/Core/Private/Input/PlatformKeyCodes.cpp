#include <Engine_pch.h>

#include "Core/Public/Input/PlatformKeyCodes.h"

namespace ConstPlatformInputCodes
{
#if IE_WIN32 || IE_UWP_DESKTOP

	const int PlatformMouseCode_Button_Left = VK_LBUTTON;
	const int PlatformMouseCode_Button_Right = VK_RBUTTON;
	const int PlatformMouseCode_Button_Middle = VK_MBUTTON;

	const int PlatformKeyboardCode_Shift = VK_SHIFT;
	const int PlatformKeyboardCode_Shift_Left = VK_LSHIFT;
	const int PlatformKeyboardCode_Shift_Right = VK_RSHIFT;
	const int PlatformKeyboardCode_Alt = VK_MENU;
	const int PlatformKeyboardCode_Control = VK_CONTROL;
	const int PlatformKeyboardCode_Arrow_Left = VK_LEFT;
	const int PlatformKeyboardCode_Arrow_Right = VK_RIGHT;
	const int PlatformKeyboardCode_Arrow_Up = VK_UP;
	const int PlatformKeyboardCode_Arrow_Down = VK_DOWN;

	const int PlatformKeyboardCode_0 = VK_NUMPAD0;
	const int PlatformKeyboardCode_1 = VK_NUMPAD1;
	const int PlatformKeyboardCode_2 = VK_NUMPAD2;
	const int PlatformKeyboardCode_3 = VK_NUMPAD3;
	const int PlatformKeyboardCode_4 = VK_NUMPAD4;
	const int PlatformKeyboardCode_5 = VK_NUMPAD5;
	const int PlatformKeyboardCode_6 = VK_NUMPAD6;
	const int PlatformKeyboardCode_7 = VK_NUMPAD7;
	const int PlatformKeyboardCode_8 = VK_NUMPAD8;
	const int PlatformKeyboardCode_9 = VK_NUMPAD9;


	const int PlatformKeyboardCode_A = 0x41;
	const int PlatformKeyboardCode_B = 0x42;
	const int PlatformKeyboardCode_C = 0x43;
	const int PlatformKeyboardCode_D = 0x44;
	const int PlatformKeyboardCode_E = 0x45;
	const int PlatformKeyboardCode_F = 0x46;
	const int PlatformKeyboardCode_G = 0x47;
	const int PlatformKeyboardCode_H = 0x48;
	const int PlatformKeyboardCode_I = 0x49;
	const int PlatformKeyboardCode_J = 0x4A;
	const int PlatformKeyboardCode_K = 0x4B;
	const int PlatformKeyboardCode_L = 0x4C;
	const int PlatformKeyboardCode_M = 0x4D;
	const int PlatformKeyboardCode_N = 0x4E;
	const int PlatformKeyboardCode_O = 0x4F;
	const int PlatformKeyboardCode_P = 0x50;
	const int PlatformKeyboardCode_Q = 0x51;
	const int PlatformKeyboardCode_R = 0x52;
	const int PlatformKeyboardCode_S = 0x53;
	const int PlatformKeyboardCode_T = 0x54;
	const int PlatformKeyboardCode_U = 0x55;
	const int PlatformKeyboardCode_V = 0x56;
	const int PlatformKeyboardCode_W = 0x57;
	const int PlatformKeyboardCode_X = 0x58;
	const int PlatformKeyboardCode_Y = 0x59;
	const int PlatformKeyboardCode_Z = 0x5A;


	const int  PlatformGamepadButtonCode_A = VK_GAMEPAD_A;
	const int  PlatformGamepadButtonCode_B = VK_GAMEPAD_B;
	const int  PlatformGamepadButtonCode_X = VK_GAMEPAD_X;
	const int  PlatformGamepadButtonCode_Y = VK_GAMEPAD_Y;
	const int  PlatformGamepadButtonCode_DPad_Up = VK_GAMEPAD_DPAD_UP;
	const int  PlatformGamepadButtonCode_DPad_Down = VK_GAMEPAD_DPAD_DOWN;
	const int  PlatformGamepadButtonCode_DPad_Left = VK_GAMEPAD_DPAD_LEFT;
	const int  PlatformGamepadButtonCode_DPad_Right = VK_GAMEPAD_DPAD_RIGHT;
	const int  PlatformGamepadButtonCode_Start = VK_GAMEPAD_MENU;
	const int  PlatformGamepadButtonCode_Back = VK_GAMEPAD_VIEW;
	const int  PlatformGamepadButtonCode_Thumbstick_Left = VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON;
	const int  PlatformGamepadButtonCode_Thumbstick_Right = VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON;
	const int  PlatformGamepadButtonCode_Shoulder_Left = VK_GAMEPAD_LEFT_SHOULDER;
	const int  PlatformGamepadButtonCode_Shoulder_Right = VK_GAMEPAD_RIGHT_SHOULDER;
	
	const int  PlatformGamepadTriggerCode_Left = VK_GAMEPAD_LEFT_TRIGGER;
	const int  PlatformGamepadTriggerCode_Right = VK_GAMEPAD_RIGHT_TRIGGER;
	
	const int  PlatformGamepadThumbstick_Left_Axis_X = VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT;
	const int  PlatformGamepadThumbstick_Left_Axis_Y = VK_GAMEPAD_LEFT_THUMBSTICK_UP;
	const int  PlatformGamepadThumbstick_Right_Axis_X = VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT;
	const int  PlatformGamepadThumbstick_Right_Axis_Y = VK_GAMEPAD_RIGHT_THUMBSTICK_UP;

#elif IE_PLATFORM_BUILD_MAC
#	error Mac platform is not currently supported.
#else
#	error Unidentified platform. Cannot define proper keycodes.
#endif
}
