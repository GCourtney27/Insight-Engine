#pragma once

#include "EngineDefines.h"
#include "InsightStd.h"

#include "Core/Public/Input/KeyCodes.h"


namespace Insight
{
	namespace System
	{
#define IE_DECLARE_HANDLE(name) struct name##__{ int Unused; }; typedef struct name##__ *name
		IE_DECLARE_HANDLE(DLLHandle);

		enum
		{
			kDefaultStackSize = IE_MEGABYTES(32),
		};
		enum
		{
			kJoinable = 0x01,
			kDetached = 0x02,
		};

		typedef void(*JobEntryPoint)(void*);

#if IE_WINDOWS
		struct ThreadId
		{
			DWORD Id;
			HANDLE Handle;

			bool operator == (const ThreadId& rhs) { return rhs.Id == this->Id; }
			bool operator != (const ThreadId& rhs) { return rhs.Id != this->Id; }
		};
#endif
		/*
			Return the state of a given key.
			@param Key - The key to get the state of.
			@returns The state of the key.
		*/
		EInputEventType GetAsyncKeyState(KeyMapCode Key);
		
		/*
			Create a message box to be immediatly displayed to th user.
			@param Message - The message to display inside the message box.
			@param Title - The message to be displayer in the titlebar of the message box window.
			@param pParentWindow - A pointer to the native window's handle.
		*/
		void CreateMessageBox(const wchar_t* Message, const wchar_t* Title, void* pParentWindow);
		
		/*
			Loads a DLL into the curent application's address space.
			@param Filepath - The filepath to the dll relative to the exe.
			@returns A handle to the newly loaded dll. Null if file not found or a load error occured.
		*/
		DLLHandle LoadDynamicLibrary(const wchar_t* Filepath);
		
		/*
			Frees a DLL form the address space.
			@param Handle - The handle to the DLL to be unloaded.
			@returns A non-zero value if the funtion succeeds.
		*/
		Int32 FreeDynamicLibrary(DLLHandle Handle);
		
		/*
			Formats and returns the last error message the platform produced.
			@returns The formatted message.
		*/
		wchar_t* GetLastPlatformError();

		/*
			Gets the current working directory of the application, not including the name of the executable.
			@param BufferSize - The size of `Buffer` parameter in bytes.
			@param Buffer - The character buffer to pupulate with the path.
		*/
		void GetWorkingDirectory(size_t BufferSize, TChar* Buffer);

		/*
			Set the working directory of the application.
			@param Path - The new working directory path.
		*/
		void SetWorkingDirectory(TChar* Path);

		/*
			Returns the number of CPU cores the current thread posseses.
		*/
		UInt32 GetProcessorCount();


		ThreadId CreateAndRunThread(const char* Name, const UInt32 CoreIdx, JobEntryPoint EntryPoint, void* UserData = NULL, const UInt64 StackSize = kDefaultStackSize, const Int32 Flags = kJoinable);

		void SetThreadName(ThreadId Thread, const char* NewName);
	}
}
