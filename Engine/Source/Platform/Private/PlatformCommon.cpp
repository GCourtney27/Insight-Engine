#include <Engine_pch.h>

#include "Platform/Public/PlatformCommon.h"

#include "Core/Public/Cast.h"

namespace Insight
{
	namespace Platform
	{
		EInputEventType GetAsyncKeyState(KeyMapCode Key)
		{
			bool Pressed = false;
#if IE_WIN32
			short KeyState = ::GetAsyncKeyState(Key);
			Pressed = ( (1 << 15) & KeyState );
#elif IE_UWP_DESKTOP
			const winrt::Windows::System::VirtualKey VirtualKey = (winrt::Windows::System::VirtualKey)Key;
			auto State = winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().GetAsyncKeyState(VirtualKey);
			Pressed = ( State == winrt::Windows::UI::Core::CoreVirtualKeyStates::Down );
#endif
			return Pressed ? IET_Pressed : IET_Released;
		}

		void CreateMessageBox(const wchar_t* Message, const wchar_t* Title, void* pParentWindow)
		{
#if IE_WIN32
			::MessageBox(RCast<HWND>(pParentWindow), Message, Title, MB_OK);
#elif IE_UWP_DESKTOP
			#pragma message ("CreateMessageBox not defined for UWP platform!")
#endif
		}

		DLLHandle LoadDynamicLibrary(const wchar_t* Filepath)
		{
			return (DLLHandle)(
#if IE_WIN32
				LoadLibrary(Filepath)
#elif IE_UWP_DESKTOP
				LoadPackagedLibrary(Filepath, 0)
#endif
			);
		}

		Int32 FreeDynamicLibrary(DLLHandle Handle)
		{
			IE_ASSERT(Handle != NULL); // Trying to free a handle to a null library.

#if IE_WIN32 || IE_UWP_DESKTOP
			return (Int32)FreeLibrary((HMODULE)Handle);
#endif
		}


		wchar_t* GetLastPlatformError()
		{
#if IE_WIN32 || IE_UWP_DESKTOP
			LPVOID lpMsgBuf;
			LPVOID lpDisplayBuf;
			DWORD ErrCode = ::GetLastError();

			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				ErrCode,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&lpMsgBuf,
				0, NULL);

			lpDisplayBuf = (LPVOID)::LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)L"") + 40) * sizeof(TCHAR));
			StringCchPrintf(
				(LPTSTR)lpDisplayBuf,
				::LocalSize(lpDisplayBuf) / sizeof(TCHAR),
				TEXT("%s failed with error %d: %s"),
				"", ErrCode, lpMsgBuf
			);
			return (wchar_t*)lpDisplayBuf;
#endif
			return NULL;
		}

		void GetWorkingDirectory(size_t BufferSize, TChar* Buffer)
		{
#if IE_WIN32
			// Fetch the current directory (includes the exe name).
			DWORD Result = ::GetModuleFileName(NULL, Buffer, (DWORD)BufferSize);
			if (Result == 0)
				IE_LOG(Error, TEXT("Failed to read application root current directory. Windows Error: %i"), GetLastError());

			// Start at the end and remove the exe name character by character.
			for (size_t i = BufferSize - 1; i != 0; --i)
			{
				if (Buffer[i] != TChar('\\') && Buffer[i] != TChar('/'))
				{
					// Erase the character
					Buffer[i] = 0;
				}
				else
				{
					// Found the first set of slashes; the exe has been erased.
					break;
				}
			}
#elif IE_UWP_DESKTOP
			(void)BufferSize;

			// The default working directory is the .exe root in UWP apps. Which is automatically registered and cannot be changed.
			ZeroMemRanged(Buffer, BufferSize);
			Buffer[0] = "\0";
#endif
		}

		void SetWorkingDirectory(TChar* Path)
		{
#if IE_WIN32
			bool Result = ::SetCurrentDirectory(Path);
			if (!Result)
				IE_LOG(Error, TEXT("Failed to set the current directory of the exe. Windows Error: %i"), GetLastError());
#elif IE_UWP_DESKTOP
			(void)Path;
#endif
		}
	}
}