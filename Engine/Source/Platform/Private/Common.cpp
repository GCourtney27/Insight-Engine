#include <Engine_pch.h>

#include "Platform/Public/Common.h"


namespace Insight
{
	namespace Platform
	{
		EInputEventType GetAsyncKeyState(KeyMapCode Key)
		{
#if IE_PLATFORM_BUILD_WIN32
			short KeyState = ::GetAsyncKeyState(Key);
			bool Pressed = (BIT_SHIFT(15)) & KeyState;

			if (Pressed)
				return IET_Pressed;
			else
				return IET_Released;
#elif IE_PLATFORM_BUILD_UWP
			const winrt::Windows::System::VirtualKey VirtualKey = (winrt::Windows::System::VirtualKey)Key;
			auto state = winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().GetAsyncKeyState(VirtualKey);
			if (state == winrt::Windows::UI::Core::CoreVirtualKeyStates::Down)
				return IET_Pressed;
			else
				return IET_Released;
#endif
		}

		void CreateMessageBox(const wchar_t* Message, const wchar_t* Title, void* pParentWindow)
		{
#if IE_PLATFORM_BUILD_WIN32
			::MessageBox(RCast<HWND>(pParentWindow), Message, Title, MB_OK);
#elif IE_PLATFORM_BUILD_UWP
			#pragma message CreateMessageBox not defined for UWP platform!
#endif
		}
	}
}