#include <Engine_pch.h>

#include "File_System.h"

#include "Retina/Core/Application.h"
#include "Retina/Core/Scene/scene.h"
#include "Retina/Core/rn_Exception.h"
#include "Retina/Utilities/String_Helper.h"

#include "Retina/Rendering/APost_Fx.h"
#include "Retina/Rendering/ASky_Light.h"
#include "Retina/Rendering/ASky_Sphere.h"
#include "Retina/Rendering/Lighting/ASpot_Light.h"
#include "Retina/Rendering/Lighting/APoint_Light.h"
#include "Retina/Rendering/Lighting/ADirectional_Light.h"

namespace Retina {

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
		
		FileSystem::ExecutableDirectory = GetExecutbleDirectory();
		return true;
	}

	void FileSystem::SaveEngineUserSettings(Renderer::GraphicsSettings Settings)
	{

		rapidjson::Document RawSettingsFile;
		const std::string SettingsDir = ProjectDirectory + "/PROFSAVE.ini";
		if (!json::load(SettingsDir.c_str(), RawSettingsFile)) {
			RN_CORE_ERROR("Failed to load graphics settings from file: \"{0}\". Default graphics settings will be applied.", SettingsDir);

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
				RN_CORE_ERROR("Failed to save graphics properties to PROFSAVE.ini.");
			}
		}

	}

	std::string FileSystem::GetProjectRelativeAssetDirectory(std::string Path)
	{
		return ProjectDirectory + "/Assets/" + Path;
	}

	Renderer::GraphicsSettings FileSystem::LoadGraphicsSettingsFromJson()
	{
		using GraphicsSettings = Renderer::GraphicsSettings;
		GraphicsSettings UserGraphicsSettings = {};

		{
			ScopedPerfTimer("LoadSceneFromJson::LoadGraphicsSettingsFromJson", eOutputType_Seconds);

			rapidjson::Document RawSettingsFile;
			const std::string SettingsDir = ProjectDirectory + "/PROFSAVE.ini";
			if (!json::load(SettingsDir.c_str(), RawSettingsFile)) {
				RN_CORE_ERROR("Failed to load graphics settings from file: \"{0}\". Default graphics settings will be applied.", SettingsDir);
				return UserGraphicsSettings;
			}

			const rapidjson::Value& RendererSettings = RawSettingsFile["Renderer"];
			int TargetAPI, TextureFiltering;
			float TextureQuality;
			bool RTEnabled;
			json::get_int(RendererSettings[0], "TargetAPI", TargetAPI);
			json::get_float(RendererSettings[0], "TextureQuality", TextureQuality);
			json::get_int(RendererSettings[0], "TextureFiltering", TextureFiltering);
			json::get_bool(RendererSettings[0], "RayTraceEnabled", RTEnabled);
			UserGraphicsSettings.TargetRenderAPI = (Renderer::eTargetRenderAPI)TargetAPI;
			UserGraphicsSettings.MaxAnisotropy = TextureFiltering;
			UserGraphicsSettings.MipLodBias = TextureQuality;
			UserGraphicsSettings.RayTraceEnabled = RTEnabled;
		}

		return UserGraphicsSettings;
	}

	bool FileSystem::LoadSceneFromJson(const std::string& FileName, Scene* pScene)
	{
		// Load in Meta.json
		{
			ScopedPerfTimer("LoadSceneFromJson::LoadMetaData", eOutputType_Seconds);

			rapidjson::Document rawMetaFile;
			const std::string metaDir = FileName + "/Meta.json";
			if (!json::load(metaDir.c_str(), rawMetaFile)) {
				RN_CORE_ERROR("Failed to load meta file from scene: \"{0}\" from file.", FileName);
				return false;
			}
			std::string sceneName;
			int NumActorsInScene;
			json::get_string(rawMetaFile, "SceneName", sceneName);
			pScene->SetDisplayName(sceneName);
			Application::Get().GetWindow().SetWindowTitle(sceneName);
			
			json::get_int(rawMetaFile, "NumSceneActors", NumActorsInScene);
			pScene->ResizeSceneGraph(NumActorsInScene);

			RN_CORE_TRACE("Scene meta data loaded.");
		}

		// Load in Resources.json
		{
			ScopedPerfTimer("LoadSceneFromJson::LoadResources", eOutputType_Seconds);

			rapidjson::Document RawResourceFile;
			const std::string ResorurceDir = FileName + "/Resources.json";
			if (!json::load(ResorurceDir.c_str(), RawResourceFile)) {
				RN_CORE_ERROR("Failed to load resource file from scene: \"{0}\" from file.", FileName);
				return false;
			}

			ResourceManager::Get().LoadResourcesFromJson(RawResourceFile);

			RN_CORE_TRACE("Scene resouces loaded.");
		}

		// Load in Actors.json last once resources have been intialized
		{
			ScopedPerfTimer("LoadSceneFromJson::LoadActors", eOutputType_Seconds);

			rapidjson::Document RawActorsFile;
			const std::string ActorsDir = FileName + "/Actors.json";
			if (!json::load(ActorsDir.c_str(), RawActorsFile)) {
				RN_CORE_ERROR("Failed to load actor file from scene: \"{0}\" from file.", FileName);
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
					RN_CORE_ERROR("Failed to parse actor \"{0}\" into scene", (ActorDisplayName == "") ? "INVALID NAME" : ActorDisplayName);
					continue;
				}

				pScene->AddActor(pNewActor);
				ActorSceneIndex++;
			}

			RN_CORE_TRACE("Scene actors loaded.");
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
			std::string sceneName = FileSystem::ProjectDirectory + "/Assets/Scenes/" + pScene->GetDisplayName() + ".iescene/Meta.json";
			std::ofstream offstream(sceneName.c_str());
			offstream << StrBuffer.GetString();

			if (!offstream.good()) {
				RN_CORE_ERROR("Failed to save meta data for scene: {0}", pScene->GetDisplayName());
				return false;
			}
		}

		// Save Out Actors.json
		{
			rapidjson::StringBuffer StrBuffer;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> Writer(StrBuffer);

			pScene->WriteToJson(&Writer);

			// Final Export
			std::string sceneName = FileSystem::ProjectDirectory + "/Assets/Scenes/" + pScene->GetDisplayName() + ".iescene/Actors.json";
			std::ofstream offstream(sceneName.c_str());
			offstream << StrBuffer.GetString();

			if (!offstream.good()) {
				RN_CORE_ERROR("Failed to save actors for scene: {0}", pScene->GetDisplayName());
				return false;
			}
		}

		return true;
	}

	bool FileSystem::FileExistsInAssetDirectory(const std::string& Path)
	{
		std::string RawPath(GetProjectDirectory());
		RawPath += "Assets/" + Path;
		return PathFileExistsA(RawPath.c_str());
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
			RN_CORE_ERROR("Failed to get path to user documents folder.");
		}

		UserDocumentsFolder = StringHelper::WideToString(std::wstring{ Folder });
	}


}
