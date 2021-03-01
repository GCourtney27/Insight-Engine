#pragma once

#include <Runtime/CoreMacros.h>

namespace Insight
{
	
#ifdef UNICODE
	typedef wchar_t TChar;
	typedef std::wstring EString;
#else
	typedef char TChar;
	typedef std::string EString;
#endif
	
	template <typename Type>
	EString ToString(Type Val)
	{
		return 
#ifdef UNICODE
			std::to_wstring(Val);
#else
			std::to_string(Val);
#endif
	}

	typedef wchar_t WChar;
	typedef char Char;

}
