#pragma once
#include <comdef.h>

#include "Insight/Utilities/StringHelper.h"
#include "Insight/Core/Exception.h"

namespace Insight {

	#define ThrowIfFailed( hr, msg ) if( FAILED( hr ) ) throw COMException (hr, msg, __FILE__, __FUNCTION__, __LINE__ );
	#define THROW_COM_ERROR(msg) throw COMException(NULL, msg, __FILE__, __FUNCTION__, __LINE__)

	class INSIGHT_API COMException : public ieException
	{
	public:
		COMException(HRESULT hr, const std::string& msg, const std::string& file, const std::string& function, int line)
			: ieException(msg.c_str())
		{
#if defined (IE_PLATFORM_BUILD_WIN32)
			_com_error error(hr);
#elif defined (IE_PLATFORM_BUILD_UWP)
			_com_error error(hr, StringHelper::StringToWide(msg).c_str());
#endif
			whatmsg = L"Msg: " + StringHelper::StringToWide(std::string(msg)) + L"\n";
			whatmsg += error.ErrorMessage();
			whatmsg += L"\nFile: " + StringHelper::StringToWide(file);
			whatmsg += L"\nFunction: " + StringHelper::StringToWide(function);
			whatmsg += L"\nLine: " + StringHelper::StringToWide(std::to_string(line));
		}

		const wchar_t * what() const
		{
			return whatmsg.c_str();
		}

	private:
		std::wstring whatmsg;
	};

}

