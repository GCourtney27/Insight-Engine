#include <Engine_pch.h>

#include "Platform/Public/System.h"

#include "Core/Public/Cast.h"

namespace Insight
{
	namespace System
	{

		//
		//	Per-Platform Functions and Structures
		//
#	if IE_WIN32
		struct Win32ThreadData
		{
			JobEntryPoint EntryPoint;
			void* UserData;
			const char* Name;
			DWORD Id;
		};

		unsigned int WINAPI Win32ThreadEntry(void* pArgs)
		{
			Win32ThreadData Data = *(Win32ThreadData*)pArgs;
			delete pArgs;

			Data.EntryPoint(Data.UserData);

			return EXIT_SUCCESS;
		}

#	endif



		//
		// System Funtion Implementations
		//

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
			IE_LOG(Log, TEXT("Loading module: %s"), Filepath);
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
		
		UInt32 GetProcessorCount()
		{
			UInt32 ProcessorCount = 0u;
#if IE_WIN32 || IE_UWP_DESKTOP
			SYSTEM_INFO SystemInfo = {};
			GetSystemInfo(&SystemInfo);
			ProcessorCount = SystemInfo.dwNumberOfProcessors;
#endif
			return ProcessorCount;
		}
		
		ThreadId CreateAndRunThread(const char* Name, const UInt32 CoreIdx, JobEntryPoint EntryPoint, void* UserData/* = NULL*/, const UInt64 StackSize/* = kDefaultStackSize*/, const Int32 Flags/* = kJoinable*/)
		{
			ThreadId Thread;
#if IE_WIN32
			Win32ThreadData* ThreadData = new Win32ThreadData();

			UInt32 ThreadFlags = 0;
			unsigned int NewThreadId = 0;
			HANDLE NewThreadHandle = (HANDLE)_beginthreadex(NULL, (unsigned int)StackSize, &Win32ThreadEntry, ThreadData, ThreadFlags, &NewThreadId);

			Thread.Id = NewThreadId;
			Thread.Handle = NewThreadHandle;
			
#endif
			SetThreadName(Thread, Name);
			return Thread;
		}

		void SetThreadName(ThreadId Thread, const char* NewName)
		{
#if IE_WIN32
			struct ThreadNameInfo
			{
				DWORD	dwType;		// Must be 0x1000.
				LPCSTR	szName;		// Pointer to name (in user address space).
				DWORD	dwThreadID; // Thread ID (-1=caller thread).
				DWORD	dwFlags;	// Reserved for future use, must be zero.
			};

			ThreadNameInfo Info;
			Info.dwType		= 0x1000;
			Info.szName		= NewName;
			Info.dwThreadID = Thread.Id;
			Info.dwFlags	= 0;

			__try
			{
				RaiseException(0x406D1388, 0, sizeof(Info) / sizeof(DWORD), (CONST ULONG_PTR*)& Info);
			}
			__except (EXCEPTION_CONTINUE_EXECUTION)
			{
			}
#endif
		}
	}
}