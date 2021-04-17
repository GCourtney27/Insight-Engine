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

		DLLHandle LoadDynamicLibrary(const wchar_t* Filepath)
		{
			return (DLLHandle)
#if IE_PLATFORM_BUILD_WIN32
			LoadLibrary(Filepath);
#elif IE_PLATFORM_BUILD_UWP
			LoadPackagedLibrary(Filepath, 0);
#endif
		}

		Int32 FreeDynamicLibrary(DLLHandle Handle)
		{
			IE_ASSERT(Handle != NULL); // Trying to free a handle to a null library.

#if IE_PLATFORM_BUILD_WIN32 || IE_PLATFORM_BUILD_UWP
			return (Int32)FreeLibrary((HMODULE)Handle);
#endif
		}


		wchar_t* GetLastPlatformError()
		{
#if IE_PLATFORM_BUILD_WIN32 || IE_PLATFORM_BUILD_UWP
			LPVOID lpMsgBuf;
			LPVOID lpDisplayBuf;
			DWORD dw = ::GetLastError();

			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				dw,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&lpMsgBuf,
				0, NULL);

			lpDisplayBuf = (LPVOID)::LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)"") + 40) * sizeof(TCHAR));
			StringCchPrintf(
				(LPTSTR)lpDisplayBuf,
				::LocalSize(lpDisplayBuf) / sizeof(TCHAR),
				TEXT("%s failed with error %d: %s"),
				"", dw, lpMsgBuf
			);
			return (wchar_t*)lpDisplayBuf;
#endif
			return NULL;
		}
	}
}