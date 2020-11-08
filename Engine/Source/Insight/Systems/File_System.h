#pragma once

#include <string_view>

#include <Insight/Core.h>
#include "Insight/Rendering/Renderer.h"

namespace Insight {

	class Scene;

	class INSIGHT_API FileSystem
	{
	public:
		FileSystem();
		~FileSystem();

		static bool Init(const char* ProjectName);

		static void SaveEngineUserSettings(Renderer::GraphicsSettings Settings);
		
		static inline std::string_view GetExecutbleDirectory() { return std::string_view{ FileSystem::ExecutableDirectory.c_str() }; }
		static inline std::wstring_view GetExecutbleDirectoryW() { return std::wstring_view{ FileSystem::ExecutableDirectoryW.c_str() }; }
		static inline std::string_view GetProjectDirectory() { return std::string_view{ FileSystem::ProjectDirectory.c_str() }; }
		static inline std::string_view GetUserDocumentsFolderPath() { return std::string_view{ FileSystem::UserDocumentsFolder.c_str() }; }
		static std::string GetProjectRelativeAssetDirectory(std::string Path);

		static Renderer::GraphicsSettings LoadGraphicsSettingsFromJson();
		static bool LoadSceneFromJson(const std::string& FileName, Scene* pScene);
		static bool WriteSceneToJson(Scene* pScene);
		static bool FileExistsInAssetDirectory(const std::string& Path);

	protected:
		static std::string UserDocumentsFolder;
		static std::string ProjectDirectory;
		static std::string ExecutableDirectory;
		static std::wstring ExecutableDirectoryW;

	private:
		static void SetExecutableDirectory();
		static void SetProjectDirectory(const char* ProjectName);
		static void SetUserDocumentsFolderDirectory();
	};

}
