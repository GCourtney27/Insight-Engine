#pragma once
#include <Runtime/Core.h>

namespace Insight 
{
	namespace StringHelper
	{
		std::wstring StringToWide(const std::string& str);
		std::string WideToString(const std::wstring& wStr);
		FString GetDirectoryFromPath(const FString& filepath);
		FString GetFileExtension(const FString& filename);
		FString GetFilenameFromDirectory(const FString& filename);
		FString GetFilenameFromDirectoryNoExtension(const FString& filename);
	}
}