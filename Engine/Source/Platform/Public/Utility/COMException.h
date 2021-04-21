#pragma once
#include <comdef.h>

#include "Runtime/Core/Utilities/StringHelper.h"
#include "Runtime/Core/Exception.h"

namespace Insight {
	
#	define ThrowIfFailed( hr, msg ) if( FAILED( hr ) ) throw COMException (hr, msg, __FILEW__, __FUNCTIONW__, __LINE__ );
#	define THROW_COM_ERROR(msg) throw COMException(NULL, msg, __FILEW__, __FUNCTIONW__, __LINE__)

	class INSIGHT_API COMException : public ieException
	{
	public:
		COMException(HRESULT hr, const EString& msg, const EString& file, const EString& function, int line)
			: ieException(msg.c_str())
		{
#if IE_PLATFORM_BUILD_WIN32
			_com_error error(hr);
#elif IE_PLATFORM_BUILD_UWP || IE_PLATFORM_BUILD_XBOX_ONE
			_com_error error(hr, msg.c_str());
#endif
			whatmsg =  TEXT("Msg: ") + EString(msg) + L"\n";
			whatmsg += error.ErrorMessage();
			whatmsg += TEXT("\nFile: ") + file;
			whatmsg += TEXT("\nFunction: ") + function;
			whatmsg += TEXT("\nLine: ") + ToString(line);
		}

		const wchar_t* what() const
		{
			return whatmsg.c_str();
		}

	private:
		EString whatmsg;
	};

}


