#pragma once

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
		
		static inline const char* GetExecutableDirectory() { return FileSystem::ExecutableDirectory.c_str(); }
		static inline const wchar_t* GetExecutbleDirectoryW() { return FileSystem::ExecutableDirectoryW.c_str(); }
		static std::string GetRelativeContentDirectory(const std::string& Path);
		static std::wstring GetRelativeContentDirectoryW(const std::wstring& Path);

		static Renderer::GraphicsSettings LoadGraphicsSettingsFromJson();
		static bool LoadSceneFromJson(const std::string& FileName, Scene* pScene);
		static bool WriteSceneToJson(Scene* pScene);
		static bool FileExistsInContentDirectory(const std::string& Path);
		static std::string GetShaderPath(const char* Shader);
		static std::wstring GetShaderPathW(const wchar_t* Shader);

	protected:
		static std::string ExecutableDirectory;
		static std::wstring ExecutableDirectoryW;

	private:
		static void SetExecutableDirectory();
	};

}
