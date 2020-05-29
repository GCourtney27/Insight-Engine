#include <ie_pch.h>

#include "File_System.h"

namespace Insight {

	FileSystem* FileSystem::s_Instance = nullptr;

	FileSystem::FileSystem()
	{
		IE_CORE_ASSERT(!s_Instance, "There is already an instance of a file system!");
		s_Instance = this;
	}

	FileSystem::~FileSystem()
	{
	}

	std::string FileSystem::GetRelativeAssetDirectoryPath(std::string path)
	{
		std::string relativePath;
#if defined IE_DEBUG
		relativePath += "../" + path;
#elif defined IE_RELEASE
		relativePath += "../../../" + path;
#endif
		return relativePath;
	}

}
