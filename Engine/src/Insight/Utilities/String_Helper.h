#pragma once
#include <string>

namespace Insight {

	class StringHelper
	{
	public:
		static std::wstring StringToWide(std::string str);
		static std::string GetDirectoryFromPath(const std::string & filepath);
		static std::string GetFileExtension(const std::string & filename);
		static std::string GetFilenameFromDirectory(const std::string & filename);
		std::string GetFilenameFromDirectoryNoExtension(const std::string& filename);
	};
}