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
		bool LoadSceneFromJson(const std::string& fileName, Scene& scene);
		
	private:
		static FileSystem* s_Instance;
	};

}
