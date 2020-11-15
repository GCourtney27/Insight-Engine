#include <Engine_pch.h>

#include "File_System.h"

#include "Insight/Core/Application.h"
#include "Insight/Core/Scene/scene.h"
#include "Insight/Core/ie_Exception.h"
#include "Insight/Utilities/String_Helper.h"

#include "Insight/Rendering/APost_Fx.h"
#include "Insight/Rendering/ASky_Light.h"
#include "Insight/Rendering/ASky_Sphere.h"
#include "Insight/Rendering/Lighting/ASpot_Light.h"
#include "Insight/Rendering/Lighting/APoint_Light.h"
#include "Insight/Rendering/Lighting/ADirectional_Light.h"

namespace Insight {

	std::string FileSystem::UserDocumentsFolder = "";
	std::string FileSystem::ProjectDirectory = "";
	std::string FileSystem::ExecutableDirectory = "";
	std::wstring FileSystem::ExecutableDirectoryW = L"";

	FileSystem::FileSystem()
	{
	}

	FileSystem::~FileSystem()
	{
	}

	bool FileSystem::Init(const char* ProjectName)
	{
		SetUserDocumentsFolderDirectory();
		SetProjectDirectory(ProjectName);
		SetExecutableDirectory();
		
		FileSystem::ExecutableDirectory = GetExecutableDirectory();
		return true;
	}

	void FileSystem::SaveEngineUserSettings(Renderer::GraphicsSettings Settings)
	{

		rapidjson::Document RawSettingsFile;
		const std::string SettingsDir = ProjectDirectory + "/PROFSAVE.ini";
		if (!json::load(SettingsDir.c_str(), RawSettingsFile)) {
			IE_DEBUG_LOG(LogSeverity::Error, "Failed to load graphics settings from file: \"{0}\". Default graphics settings will be applied.", SettingsDir);

		}

		{
			rapidjson::StringBuffer StrBuffer;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> Writer(StrBuffer);

			Writer.StartObject();
			{
				Writer.Key("Renderer");
				Writer.StartArray();
				{
					Writer.StartObject();

					Writer.Key("TargetAPI");
					Writer.Int((int)Settings.TargetRenderAPI);
					Writer.Key("TextureQuality");
					Writer.Double(Settings.MipLodBias);
					Writer.Key("TextureFiltering");
					Writer.Int(Settings.MaxAnisotropy);
					Writer.Key("RayTraceEnabled");
					Writer.Bool(Settings.RayTraceEnabled);

					Writer.EndObject();
				}
				Writer.EndArray();
			}
			Writer.EndObject();
			

			// Final Export
			std::string sceneName = ProjectDirectory + "/PROFSAVE.ini";
			std::ofstream offstream(sceneName.c_str());
			offstream << StrBuffer.GetString();

			if (!offstream.good()) {
				IE_DEBUG_LOG(LogSeverity::Error, "Failed to save graphics properties to PROFSAVE.ini.");
			}
		}

	}

	std::string FileSystem::GetProjectRelativeContentDirectory(std::string Path)
	{
		return ProjectDirectory + "/Content/" + Path;
	}

	Renderer::GraphicsSettings FileSystem::LoadGraphicsSettingsFromJson()
	{
		using GraphicsSettings = Renderer::GraphicsSettings;
		GraphicsSettings UserGraphicsSettings = {};

		{
			ScopedPerfTimer("LoadSceneFromJson::LoadGraphicsSettingsFromJson", OutputType_Seconds);

			rapidjson::Document RawSettingsFile;
			const std::string SettingsDir = ProjectDirectory + "/PROFSAVE.ini";
			if (!json::load(SettingsDir.c_str(), RawSettingsFile)) {
				IE_DEBUG_LOG(LogSeverity::Error, "Failed to load graphics settings from file: \"{0}\". Default graphics settings will be applied.", SettingsDir);
				return UserGraphicsSettings;
			}
			int MaxAniso, TargetRenderAPI;
			const rapidjson::Value& RendererSettings = RawSettingsFile["Renderer"];
			json::get_int(RendererSettings[0], "TargetAPI", TargetRenderAPI);
			json::get_float(RendererSettings[0], "TextureQuality", UserGraphicsSettings.MipLodBias);
			json::get_int(RendererSettings[0], "TextureFiltering", MaxAniso);
			json::get_bool(RendererSettings[0], "RayTraceEnabled", UserGraphicsSettings.RayTraceEnabled);
			UserGraphicsSettings.MaxAnisotropy = MaxAniso;
			UserGraphicsSettings.TargetRenderAPI = (Renderer::TargetRenderAPI)TargetRenderAPI;
		}

		return UserGraphicsSettings;
	}

	bool FileSystem::LoadSceneFromJson(const std::string& FileName, Scene* pScene)
	{
		// Load in Meta.json
		{
			ScopedPerfTimer("LoadSceneFromJson::LoadMetaData", OutputType_Seconds);

			rapidjson::Document rawMetaFile;
			const std::string metaDir = FileName + "/Meta.json";
			if (!json::load(metaDir.c_str(), rawMetaFile)) {
				IE_DEBUG_LOG(LogSeverity::Error, "Failed to load meta file from scene: \"{0}\" from file.", FileName);
				return false;
			}
			std::string sceneName;
			int NumActorsInScene;
			json::get_string(rawMetaFile, "SceneName", sceneName);
			pScene->SetDisplayName(sceneName);
			Renderer::GetWindowRef().SetWindowTitle(sceneName);
			
			json::get_int(rawMetaFile, "NumSceneActors", NumActorsInScene);
			pScene->ResizeSceneGraph(NumActorsInScene);

			IE_DEBUG_LOG(LogSeverity::Verbose, "Scene meta data loaded.");
		}

		// Load in Resources.json
		{
			ScopedPerfTimer("LoadSceneFromJson::LoadResources", OutputType_Seconds);

			rapidjson::Document RawResourceFile;
			const std::string ResorurceDir = FileName + "/Resources.json";
			if (!json::load(ResorurceDir.c_str(), RawResourceFile)) {
				IE_DEBUG_LOG(LogSeverity::Error, "Failed to load resource file from scene: \"{0}\" from file.", FileName);
				return false;
			}

			ResourceManager::Get().LoadResourcesFromJson(RawResourceFile);

			IE_DEBUG_LOG(LogSeverity::Verbose, "Scene resouces loaded.");
		}

		// Load in Actors.json last once resources have been intialized
		{
			ScopedPerfTimer("LoadSceneFromJson::LoadActors", OutputType_Seconds);

			rapidjson::Document RawActorsFile;
			const std::string ActorsDir = FileName + "/Actors.json";
			if (!json::load(ActorsDir.c_str(), RawActorsFile)) {
				IE_DEBUG_LOG(LogSeverity::Error, "Failed to load actor file from scene: \"{0}\" from file.", FileName);
				return false;
			}

			Runtime::AActor* pNewActor = nullptr;
			uint32_t ActorSceneIndex = 0;

			const rapidjson::Value& SceneObjects = RawActorsFile["Set"];
			for (rapidjson::SizeType a = 0; a < SceneObjects.Size(); a++)
			{
				const rapidjson::Value& jsonActor = SceneObjects[a];

				std::string ActorDisplayName;
				std::string ActorType;
				json::get_string(jsonActor, "DisplayName", ActorDisplayName);
				json::get_string(jsonActor, "Type", ActorType);

				if (ActorType == "Actor") {
					pNewActor = new Runtime::AActor(ActorSceneIndex, ActorDisplayName);
					pNewActor->LoadFromJson(&jsonActor);
				}
				else if (ActorType == "PointLight") {
					pNewActor = new APointLight(ActorSceneIndex, ActorDisplayName);
					pNewActor->LoadFromJson(&jsonActor);
				}
				else if (ActorType == "SpotLight") {
					pNewActor = new ASpotLight(ActorSceneIndex, ActorDisplayName);
					pNewActor->LoadFromJson(&jsonActor);
				}
 				else if (ActorType == "DirectionalLight") {
					pNewActor = new ADirectionalLight(ActorSceneIndex, ActorDisplayName);
					pNewActor->LoadFromJson(&jsonActor);
				}
				else if (ActorType == "SkySphere") {
					pNewActor = new ASkySphere(ActorSceneIndex, ActorDisplayName);
					pNewActor->LoadFromJson(&jsonActor);
				}
				else if (ActorType == "SkyLight") {
					pNewActor = new ASkyLight(ActorSceneIndex, ActorDisplayName);
					pNewActor->LoadFromJson(&jsonActor);
				}
				else if (ActorType == "PostFxVolume") {
					pNewActor = new APostFx(ActorSceneIndex, ActorDisplayName);
					pNewActor->LoadFromJson(&jsonActor);
				}

				if (pNewActor == nullptr) {
					IE_DEBUG_LOG(LogSeverity::Error, "Failed to parse actor \"{0}\" into scene", (ActorDisplayName == "") ? "INVALID NAME" : ActorDisplayName);
					continue;
				}

				pScene->AddActor(pNewActor);
				ActorSceneIndex++;
			}

			IE_DEBUG_LOG(LogSeverity::Verbose, "Scene actors loaded.");
		}

		return true;
	}

	bool FileSystem::WriteSceneToJson(Scene* pScene)
	{
		// Save Out Meta.json
		{
			rapidjson::StringBuffer StrBuffer;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> Writer(StrBuffer);

			Writer.StartObject();
			Writer.Key("SceneName");
			Writer.String(pScene->GetDisplayName().c_str());
			Writer.Key("NumSceneActors");
			Writer.Int(pScene->GetNumSceneActors());
			Writer.EndObject();

			// Final Export
			std::string sceneName = FileSystem::ProjectDirectory + "/Content/Scenes/" + pScene->GetDisplayName() + ".iescene/Meta.json";
			std::ofstream offstream(sceneName.c_str());
			offstream << StrBuffer.GetString();

			if (!offstream.good()) {
				IE_DEBUG_LOG(LogSeverity::Error, "Failed to save meta data for scene: {0}", pScene->GetDisplayName());
				return false;
			}
		}

		// Save Out Actors.json
		{
			rapidjson::StringBuffer StrBuffer;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> Writer(StrBuffer);

			pScene->WriteToJson(&Writer);

			// Final Export
			std::string sceneName = FileSystem::ProjectDirectory + "/Content/Scenes/" + pScene->GetDisplayName() + ".iescene/Actors.json";
			std::ofstream offstream(sceneName.c_str());
			offstream << StrBuffer.GetString();

			if (!offstream.good()) {
				IE_DEBUG_LOG(LogSeverity::Error, "Failed to save actors for scene: {0}", pScene->GetDisplayName());
				return false;
			}
		}

		return true;
	}

	bool FileSystem::FileExistsInContentDirectory(const std::string& Path)
	{
		std::string RawPath(GetProjectDirectory());
		RawPath += "Content/" + Path;
#if defined (IE_PLATFORM_BUILD_WIN32)
		return PathFileExistsA(RawPath.c_str());
#elif defined (IE_PLATFORM_UWP)
		// TODO 
#endif
	}

	void FileSystem::SetExecutableDirectory()
	{
		WCHAR Path[512];
		UINT RawPathSize = _countof(Path);
		DWORD PathSize = GetModuleFileName(nullptr, Path, RawPathSize);
		if (PathSize == 0 || PathSize == RawPathSize) {
			throw ieException("Failed to get module path name or path may have been truncated.");
		}

		WCHAR* LastSlash = wcsrchr(Path, L'\\');
		if (LastSlash) {
			*(LastSlash + 1) = L'\0';
		}
		FileSystem::ExecutableDirectoryW = std::wstring{ Path };
		FileSystem::ExecutableDirectory = StringHelper::WideToString(FileSystem::ExecutableDirectoryW);
	}

	void FileSystem::SetProjectDirectory(const char* ProjectName)
	{
		FileSystem::ProjectDirectory += FileSystem::GetUserDocumentsFolderPath();
		FileSystem::ProjectDirectory += "/Insight Projects/";
		FileSystem::ProjectDirectory += ProjectName;
		FileSystem::ProjectDirectory += "/";
	}

	void FileSystem::SetUserDocumentsFolderDirectory()
	{
		wchar_t Folder[1024];
		HRESULT hr = SHGetFolderPathW(NULL, CSIDL_MYDOCUMENTS, 0, 0, Folder);
		if (FAILED(hr)) {
			IE_DEBUG_LOG(LogSeverity::Error, "Failed to get path to user documents folder.");
		}
		UserDocumentsFolder = StringHelper::WideToString( Folder );
	}


}
