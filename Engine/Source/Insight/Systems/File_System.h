#pragma once

#include <Insight/Core.h>

namespace Insight {

	class INSIGHT_API FileSystem
	{
	public:
		FileSystem();
		~FileSystem();

		static FileSystem& Get() { return *s_Instance; }

		std::string GetRelativeAssetDirectoryPath(std::string path);

	private:
		static FileSystem* s_Instance;
	};

}
