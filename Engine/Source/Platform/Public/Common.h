#pragma once

#include <Runtime/Core.h>

#include "Runtime/Input/KeyCodes.h"

namespace Insight
{
	namespace Platform
	{
		EInputEventType GetAsyncKeyState(KeyMapCode Key);
		void CreateMessageBox(const wchar_t* Message, const wchar_t* Title, void* pParentWindow);

	}
}
