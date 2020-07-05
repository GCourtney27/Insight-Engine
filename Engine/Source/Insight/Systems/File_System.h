#pragma once

#include <Insight/Core.h>

namespace Insight {

	class Scene;

	class INSIGHT_API FileSystem
	{
	public:
		FileSystem();
		~FileSystem();

		static FileSystem& Get() { return *s_Instance; }

		std::string GetRelativeAssetDirectoryPath(std::string Path);
		bool LoadSceneFromJson(const std::string& FileName, Scene* pScene);
		bool WriteSceneToJson(Scene* pScene);
		
	private:
		static FileSystem* s_Instance;
	};

}
