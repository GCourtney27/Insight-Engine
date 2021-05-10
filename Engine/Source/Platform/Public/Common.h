#pragma once

#include <Runtime/Core.h>

#include "Runtime/Core/Public/Input/KeyCodes.h"

namespace Insight
{
	namespace Platform
	{
#define IE_DECLARE_HANDLE(name) struct name##__{int unused;}; typedef struct name##__ *name
		IE_DECLARE_HANDLE(DLLHandle);

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

	}
}
