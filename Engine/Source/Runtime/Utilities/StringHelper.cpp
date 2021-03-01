#include <Engine_pch.h>

#include "StringHelper.h"

namespace Insight 
{

	namespace StringHelper
	{
		std::wstring StringToWide(const std::string& str)
		{
			return std::wstring(str.begin(), str.end());
		}

		std::string WideToString(const std::wstring& wStr)
		{
#pragma warning (push)
#pragma warning (disable : 4244) // Disable conversion warnings.
			return std::string(wStr.begin(), wStr.end());
#pragma warning (pop)
		}

		EString GetDirectoryFromPath(const EString& filepath)
		{
			size_t off1 = filepath.find_last_of(TEXT('\\'));
			size_t off2 = filepath.find_last_of(TEXT('/'));
			if (off1 == std::string::npos && off2 == std::string::npos) //If no slash or backslash in path?
			{
				return TEXT("");
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
			return EString();
		}

		EString GetFileExtension(const EString& Filename)
		{
			size_t Offset = Filename.find_last_of(TEXT('.'));
			if (Offset == std::string::npos)
			{
				return EString{};
			}
			return EString(Filename.substr(Offset + 1));
		}

		EString GetFilenameFromDirectory(const EString& filename)
		{
			EString result = filename;

			// Erase everything up to the beginning of the filename
			const size_t last_slash_idx = result.find_last_of(TEXT("\\/"));
			if (EString::npos != last_slash_idx)
				result.erase(0, last_slash_idx + 1);

			return result;
		}

		EString GetFilenameFromDirectoryNoExtension(const EString& filename)
		{
			EString result = filename;

			// Erase everything up to the beginning of the filename
			const size_t last_slash_idx = result.find_last_of(TEXT("\\/"));
			if (EString::npos != last_slash_idx)
				result.erase(0, last_slash_idx + 1);

			// Erase the file extension
			const size_t period_idx = result.rfind('.');
			if (EString::npos != period_idx)
				result.erase(period_idx);

			return result;
		}
	}
}
