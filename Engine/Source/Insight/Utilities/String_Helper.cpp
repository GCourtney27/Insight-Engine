#include <ie_pch.h>

#include "String_Helper.h"
#include <locale>
#include <clocale>
namespace Insight {

	std::wstring StringHelper::StringToWide(const std::string& str)
	{
		std::wstring widestr(str.begin(), str.end());
		return widestr;
	}

	std::string StringHelper::WideToString(const std::wstring& wStr)
	{
		std::setlocale(LC_ALL, "");
		const std::locale locale("");
		typedef std::codecvt<wchar_t, char, std::mbstate_t> converter_type;
		const converter_type& converter = std::use_facet<converter_type>(locale);
		std::vector<char> to(wStr.length() * converter.max_length());
		std::mbstate_t state;
		const wchar_t* from_next;
		char* to_next;
		const converter_type::result result = converter.out(state, wStr.data(), wStr.data() + wStr.length(), from_next, &to[0], &to[0] + to.size(), to_next);
		if (result == converter_type::ok || result == converter_type::noconv) {
			const std::string s(&to[0], to_next);
			return s;
		}

		return "";
	}

	std::string StringHelper::GetDirectoryFromPath(const std::string & filepath)
	{
		size_t off1 = filepath.find_last_of('\\');
		size_t off2 = filepath.find_last_of('/');
		if (off1 == std::string::npos && off2 == std::string::npos) //If no slash or backslash in path?
		{
			return "";
		}
		if (off1 == std::string::npos)
		{
			return filepath.substr(0, off2);
		}
		if (off2 == std::string::npos)
		{
			return filepath.substr(0, off1);
		}
		//If both exists, need to use the greater offset
		//return filepath.substr(0, std::max(off1, off2));
		return std::string();
	}

	std::string StringHelper::GetFileExtension(const std::string & filename)
	{
		size_t off = filename.find_last_of('.');
		if (off == std::string::npos)
		{
			return {};
		}
		return std::string(filename.substr(off + 1));
	}

	std::string StringHelper::GetFilenameFromDirectory(const std::string& filename)
	{
		std::string result = filename;

		// Erase everything up to the beginning of the filename
		const size_t last_slash_idx = result.find_last_of("\\/");
		if (std::string::npos != last_slash_idx)
			result.erase(0, last_slash_idx + 1);

		return result;
	}

	std::string StringHelper::GetFilenameFromDirectoryW(const std::wstring& filename)
	{
		std::string result = WideToString(filename);

		// Erase everything up to the beginning of the filename
		const size_t last_slash_idx = result.find_last_of("\\/");
		if (std::string::npos != last_slash_idx)
			result.erase(0, last_slash_idx + 1);

		return result;
	}

	std::wstring StringHelper::GetFilenameFromDirectoryAsWideW(const std::wstring& filename)
	{
		std::wstring wstr = StringToWide(GetFilenameFromDirectoryW(filename));
		return wstr;
	}

	std::string StringHelper::GetFilenameFromDirectoryNoExtension(const std::string& filename)
	{
		std::string result = filename;

		// Erase everything up to the beginning of the filename
		const size_t last_slash_idx = result.find_last_of("\\/");
		if (std::string::npos != last_slash_idx)
			result.erase(0, last_slash_idx + 1);

		// Erase the file extension
		const size_t period_idx = result.rfind('.');
		if (std::string::npos != period_idx)
			result.erase(period_idx);

		return result;
	}

}
