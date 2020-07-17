#pragma once

#include <Insight/Core.h>

namespace Insight {

	class Scene;

	class INSIGHT_API FileSystem
	{
	public:
		FileSystem();
		~FileSystem();

		static std::string GetExecutbleDirectory();
		static std::string GetRelativeAssetDirectory(std::string Path);
		static bool LoadSceneFromJson(const std::string& FileName, Scene* pScene);
		static bool WriteSceneToJson(Scene* pScene);
	};

}
