#pragma once

namespace Insight {

	class INSIGHT_API StringHelper
	{
	public:
		static std::wstring StringToWide(const std::string& str);
		static std::string WideToString(const std::wstring& wStr);
		static std::string GetDirectoryFromPath(const std::string & filepath);
		static std::string GetFileExtension(const std::string & filename);
		static std::string GetFilenameFromDirectory(const std::string & filename);
		static std::string GetFilenameFromDirectoryW(const std::wstring & filename);
		static std::wstring GetFilenameFromDirectoryAsWideW(const std::wstring & filename);
		std::string GetFilenameFromDirectoryNoExtension(const std::string& filename);
	};
}