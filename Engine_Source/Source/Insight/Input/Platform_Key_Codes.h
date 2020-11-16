#pragma once


// -------------------------------------
// Define platform specific key codes. |
// -------------------------------------

namespace ConstPlatformInputCodes
{

	// Mouse Codes
	// -----------
	// Mouse Buttons
	extern const int PlatformMouseCode_Button_Left;
	extern const int PlatformMouseCode_Button_Right;
	extern const int PlatformMouseCode_Button_Middle;

	// Keyboard Codes
	// --------------
	// Functions
	extern const int PlatformKeyboardCode_Shift;
	extern const int PlatformKeyboardCode_Shift_Left;
	extern const int PlatformKeyboardCode_Shift_Right;
	extern const int PlatformKeyboardCode_Alt;
	extern const int PlatformKeyboardCode_Control;
	extern const int PlatformKeyboardCode_Arrow_Left;
	extern const int PlatformKeyboardCode_Arrow_Right;
	extern const int PlatformKeyboardCode_Arrow_Up;
	extern const int PlatformKeyboardCode_Arrow_Down;
	// Numbers
	extern const int PlatformKeyboardCode_0;
	extern const int PlatformKeyboardCode_1;
	extern const int PlatformKeyboardCode_2;
	extern const int PlatformKeyboardCode_3;
	extern const int PlatformKeyboardCode_4;
	extern const int PlatformKeyboardCode_5;
	extern const int PlatformKeyboardCode_6;
	extern const int PlatformKeyboardCode_7;
	extern const int PlatformKeyboardCode_8;
	extern const int PlatformKeyboardCode_9;
	// Letters
	extern const int PlatformKeyboardCode_A;
	extern const int PlatformKeyboardCode_B;
	extern const int PlatformKeyboardCode_C;
	extern const int PlatformKeyboardCode_D;
	extern const int PlatformKeyboardCode_E;
	extern const int PlatformKeyboardCode_F;
	extern const int PlatformKeyboardCode_G;
	extern const int PlatformKeyboardCode_H;
	extern const int PlatformKeyboardCode_I;
	extern const int PlatformKeyboardCode_J;
	extern const int PlatformKeyboardCode_K;
	extern const int PlatformKeyboardCode_L;
	extern const int PlatformKeyboardCode_M;
	extern const int PlatformKeyboardCode_N;
	extern const int PlatformKeyboardCode_O;
	extern const int PlatformKeyboardCode_P;
	extern const int PlatformKeyboardCode_Q;
	extern const int PlatformKeyboardCode_R;
	extern const int PlatformKeyboardCode_S;
	extern const int PlatformKeyboardCode_T;
	extern const int PlatformKeyboardCode_U;
	extern const int PlatformKeyboardCode_V;
	extern const int PlatformKeyboardCode_W;
	extern const int PlatformKeyboardCode_X;
	extern const int PlatformKeyboardCode_Y;
	extern const int PlatformKeyboardCode_Z;

	// Gamepad Buttons
	// ---------------
	extern const int PlatformGamepadButtonCode_A;
	extern const int PlatformGamepadButtonCode_B;
	extern const int PlatformGamepadButtonCode_X;
	extern const int PlatformGamepadButtonCode_Y;
	extern const int PlatformGamepadButtonCode_DPad_Up;
	extern const int PlatformGamepadButtonCode_DPad_Down;
	extern const int PlatformGamepadButtonCode_DPad_Left;
	extern const int PlatformGamepadButtonCode_DPad_Right;
	extern const int PlatformGamepadButtonCode_Start;
	extern const int PlatformGamepadButtonCode_Back;
	extern const int PlatformGamepadButtonCode_Thumbstick_Left;
	extern const int PlatformGamepadButtonCode_Thumbstick_Right;
	extern const int PlatformGamepadButtonCode_Shoulder_Left;
	extern const int PlatformGamepadButtonCode_Shoulder_Right;

	extern const int PlatformGamepadTriggerCode_Left;
	extern const int PlatformGamepadTriggerCode_Right;

	extern const int PlatformGamepadThumbstick_Left_Axis_X;
	extern const int PlatformGamepadThumbstick_Left_Axis_Y;
	extern const int PlatformGamepadThumbstick_Right_Axis_X;
	extern const int PlatformGamepadThumbstick_Right_Axis_Y;



	// Gamepad Press Masks
	// -------------------
#define XBoxCode_Button_PressMask_DPad_Up			0x0001
#define XBoxCode_Button_PressMask_DPad_Down			0x0002
#define XBoxCode_Button_PressMask_DPad_Left			0x0004
#define XBoxCode_Button_PressMask_DPad_Right		0x0008
#define XBoxCode_Button_PressMask_Start				0x0010
#define XBoxCode_Button_PressMask_Back				0x0020
#define XBoxCode_Button_PressMask_Thumbstick_Left	0x0040
#define XBoxCode_Button_PressMask_Thumbstick_Right	0x0080
#define XBoxCode_Button_PressMask_Shoulder_Left		0x0100
#define XBoxCode_Button_PressMask_Shoulder_Right	0x0200
#define XBoxCode_Button_PressMask_A					0x1000
#define XBoxCode_Button_PressMask_B					0x2000
#define XBoxCode_Button_PressMask_X					0x4000
#define XBoxCode_Button_PressMask_Y					0x8000

}
