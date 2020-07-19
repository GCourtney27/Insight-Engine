#pragma once

#include <Insight/Core.h>

namespace Insight {

	class Scene;

	class INSIGHT_API FileSystem
	{
	public:
		FileSystem();
		~FileSystem();

		static bool Init(const char* ProjectName);

		static std::string GetExecutbleDirectory();
		static std::string GetUserDocumentsFolderPath();
		static std::string GetProjectRelativeAssetDirectory(std::string Path);
		static bool LoadSceneFromJson(const std::string& FileName, Scene* pScene);
		static bool WriteSceneToJson(Scene* pScene);
		static bool FileExists(const std::string& Path);

	public:
		static std::string ProjectDirectory;
	private:

	};

}
