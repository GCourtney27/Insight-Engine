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
		
		static inline const char* GetWorkingDirectory() { return FileSystem::WorkingDirectory.c_str(); }
		static inline const wchar_t* GetWorkingDirectoryW() { return FileSystem::WorkingDirectoryW.c_str(); }
		static std::wstring GetRelativeContentDirectoryW(const std::wstring& Path);

		static Renderer::GraphicsSettings LoadGraphicsSettingsFromJson();
		static bool LoadSceneFromJson(const std::string& FileName, Scene* pScene);
		static bool WriteSceneToJson(Scene* pScene);
		static bool FileExistsInContentDirectory(const std::string& Path);
		static std::string GetShaderPath(const char* Shader);
		static std::wstring GetShaderPathW(const wchar_t* Shader);

	protected:
		static std::string WorkingDirectory;
		static std::wstring WorkingDirectoryW;

	private:
		static void SetWorkingDirectory();
	};

}
