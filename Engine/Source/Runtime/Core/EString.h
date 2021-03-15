#pragma once

#include <Runtime/Core.h>

namespace Insight
{
#ifdef UNICODE
	typedef wchar_t TChar;
	typedef std::wstring EString;
#else
	typedef char TChar;
	typedef std::string EString;
#endif // UNICODE
	
	typedef wchar_t WChar;
	typedef char Char;

	template <typename Type>
	inline EString ToString(Type Val)
	{
		return 
#ifdef UNICODE
			std::to_wstring(Val);
#else
			std::to_string(Val);
#endif // UNICODE
	}

	
}
