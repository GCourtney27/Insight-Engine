#pragma once


// -------------------------------------
// Define platform specific key codes. |
// -------------------------------------

#if defined (IE_PLATFORM_BUILD_WIN32)

// Mouse Codes
// -----------
// Mouse Buttons
namespace ConstInputCodes {
	const int PlatformMouseCode_Button_Left;
}
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

// Gamepad Buttons
// ---------------
#define PlatformGamepadButtonCode_A					VK_GAMEPAD_A
#define PlatformGamepadButtonCode_B					VK_GAMEPAD_B
#define PlatformGamepadButtonCode_X					VK_GAMEPAD_X
#define PlatformGamepadButtonCode_Y					VK_GAMEPAD_Y
#define PlatformGamepadButtonCode_DPad_Up			VK_GAMEPAD_DPAD_UP
#define PlatformGamepadButtonCode_DPad_Down			VK_GAMEPAD_DPAD_DOWN
#define PlatformGamepadButtonCode_DPad_Left			VK_GAMEPAD_DPAD_LEFT
#define PlatformGamepadButtonCode_DPad_Right		VK_GAMEPAD_DPAD_RIGHT
#define PlatformGamepadButtonCode_Start				VK_GAMEPAD_MENU
#define PlatformGamepadButtonCode_Back				VK_GAMEPAD_VIEW
#define PlatformGamepadButtonCode_Thumbstick_Left	VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON
#define PlatformGamepadButtonCode_Thumbstick_Right	VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON
#define PlatformGamepadButtonCode_Shoulder_Left		VK_GAMEPAD_LEFT_SHOULDER
#define PlatformGamepadButtonCode_Shoulder_Right	VK_GAMEPAD_RIGHT_SHOULDER

#define PlatformGamepadTriggerCode_Left				VK_GAMEPAD_LEFT_TRIGGER
#define PlatformGamepadTriggerCode_Right			VK_GAMEPAD_RIGHT_TRIGGER

#define PlatformGamepadThumbstick_Left_Axis_X		VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT
#define PlatformGamepadThumbstick_Left_Axis_Y		VK_GAMEPAD_LEFT_THUMBSTICK_UP
#define PlatformGamepadThumbstick_Right_Axis_X		VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT
#define PlatformGamepadThumbstick_Right_Axis_Y		VK_GAMEPAD_RIGHT_THUMBSTICK_UP



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

#elif defined IE_PLATFORM_BUILD_MAC
// Mac platform codes
#else
#error Unidentified platform. Cannot define proper keycodes.
#endif
