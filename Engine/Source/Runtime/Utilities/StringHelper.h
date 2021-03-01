#pragma once
#include <Runtime/CoreMacros.h>

namespace Insight {

	namespace StringHelper
	{
		std::wstring StringToWide(const std::string& str);
		std::string WideToString(const std::wstring& wStr);
		EString GetDirectoryFromPath(const EString& filepath);
		EString GetFileExtension(const EString& filename);
		EString GetFilenameFromDirectory(const EString& filename);
		EString GetFilenameFromDirectoryNoExtension(const EString& filename);
	}
}