#include <Engine_pch.h>

#include "FileSystem.h"

#include "Runtime/Core/Application.h"
#include "Runtime/Core/Scene/scene.h"
#include "Runtime/Core/Exception.h"
#include "Runtime/Core/Utilities/StringHelper.h"

#include "Runtime/Graphics/APostFx.h"
#include "Runtime/Graphics/ASkyLight.h"
#include "Runtime/Graphics/ASkySphere.h"
#include "Runtime/Graphics/Lighting/ASpotLight.h"
#include "Runtime/Graphics/Lighting/APointLight.h"
#include "Runtime/Graphics/Lighting/ADirectionalLight.h"

namespace Insight {


	FileSystem::FileSystem()
	{
	}

	FileSystem::~FileSystem()
	{
	}

	bool FileSystem::Init()
	{
		return true;
	}

	std::unique_ptr<char> FileSystem::ReadRawData(const TChar* Path, size_t& OutDataSize)
	{
		FILE* pFile = _wfopen(Path, TEXT("rb"));
		if (!pFile)
		{
			IE_LOG(Error, TEXT("Failed to read raw file with path: \"%s\""), Path);
			OutDataSize = -1;
			return nullptr;
		}

		// Get the size of the file.
		fseek(pFile, 0, SEEK_END);
		OutDataSize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);
		
		// Fill the buffer with the data in the file.
		std::unique_ptr<char> FileContents(new char[OutDataSize]);
		fread(FileContents.get(), 1, OutDataSize, pFile);

		fclose(pFile);

		return FileContents;
	}

	void FileSystem::SaveEngineUserSettings(const Renderer::GraphicsSettings& Settings)
	{
		/*rapidjson::Document RawSettingsFile;
		const std::string SettingsDir = "Content/Engine.ini";
		if (!json::load(SettingsDir.c_str(), RawSettingsFile)) {
			IE_LOG(Error, "Failed to load graphics settings from file: \"%s\". Default graphics settings will be applied.", SettingsDir.c_str());

		}*/

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
			std::string sceneName = "Content/Engine.ini";
			std::ofstream offstream(sceneName.c_str());
			offstream << StrBuffer.GetString();

			if (!offstream.good()) {
				IE_LOG(Error, TEXT("Failed to save graphics properties to Engine.ini."));
			}
		}

	}

	std::wstring FileSystem::GetRelativeContentDirectoryW(const std::wstring& Path)
	{
		return std::wstring(L"Content/" + Path);
	}

	Renderer::GraphicsSettings FileSystem::LoadGraphicsSettingsFromJson()
	{
		using GraphicsSettings = Renderer::GraphicsSettings;
		GraphicsSettings UserGraphicsSettings = {};

		{
			ScopedSecondTimer(TEXT("LoadSceneFromJson::LoadGraphicsSettingsFromJson"));
			
			rapidjson::Document RawSettingsFile;
			const std::string SettingsDir = StringHelper::WideToString(GetRelativeContentDirectoryW(L"Engine.ini"));
			if (!json::load(SettingsDir.c_str(), RawSettingsFile)) {
				IE_LOG(Error, TEXT("Failed to load graphics settings from file: \"%s\". Default graphics settings will be applied."), SettingsDir.c_str());
				return UserGraphicsSettings;
			}
			int MaxAniso, TargetRenderAPI;
			const rapidjson::Value& RendererSettings = RawSettingsFile["Renderer"];
			json::get_int(RendererSettings[0], "TargetAPI", TargetRenderAPI);
			json::get_float(RendererSettings[0], "TextureQuality", UserGraphicsSettings.MipLodBias);
			json::get_int(RendererSettings[0], "TextureFiltering", MaxAniso);
			json::get_bool(RendererSettings[0], "RayTraceEnabled", UserGraphicsSettings.RayTraceEnabled);
			UserGraphicsSettings.MaxAnisotropy = MaxAniso;
			UserGraphicsSettings.TargetRenderAPI = (Renderer::ETargetRenderAPI)TargetRenderAPI;
		}

		return UserGraphicsSettings;
	}

	bool FileSystem::LoadSceneFromJson(const std::string& FileName, Scene* pScene)
	{
		// Load in Meta.json
		{
			ScopedSecondTimer(TEXT("LoadSceneFromJson::LoadMetaData"));

			rapidjson::Document rawMetaFile;
			const std::string metaDir = FileName + "/Meta.json";
			if (!json::load(metaDir.c_str(), rawMetaFile)) {
				IE_LOG(Error, TEXT("Failed to load meta file from scene: \"%s\" from file."), FileName.c_str());
				return false;
			}
			std::string sceneName;
			int NumActorsInScene;
			json::get_string(rawMetaFile, "SceneName", sceneName);
			pScene->SetDisplayName(sceneName);
			Renderer::GetWindowRef().SetWindowTitle(StringHelper::StringToWide(sceneName));
			
			json::get_int(rawMetaFile, "NumSceneActors", NumActorsInScene);
			pScene->ResizeSceneGraph(NumActorsInScene);

			IE_LOG(Log, TEXT("Scene meta data loaded."));
		}

		// Load in Resources.json
		{
			ScopedSecondTimer(TEXT("LoadSceneFromJson::LoadResources"));

			rapidjson::Document RawResourceFile;
			const std::string ResorurceDir = FileName + "/Resources.json";
			if (!json::load(ResorurceDir.c_str(), RawResourceFile)) {
				IE_LOG(Error, TEXT("Failed to load resource file from scene: \"%s\" from file."), FileName.c_str());
				return false;
			}

			ResourceManager::Get().LoadResourcesFromJson(RawResourceFile);

			IE_LOG(Log, TEXT("Scene resouces loaded."));
		}

		// Load in Actors.json last once resources have been intialized
		{
			ScopedSecondTimer(TEXT("LoadSceneFromJson::LoadActors"));

			rapidjson::Document RawActorsFile;
			const std::string ActorsDir = FileName + "/Actors.json";
			if (!json::load(ActorsDir.c_str(), RawActorsFile)) {
				IE_LOG(Error, TEXT("Failed to load actor file from scene: \"%s\" from file."), FileName.c_str());
				return false;
			}

			GameFramework::AActor* pNewActor = nullptr;
			uint32_t ActorSceneIndex = 0;

			const rapidjson::Value& SceneObjects = RawActorsFile["Set"];
			for (rapidjson::SizeType a = 0; a < SceneObjects.Size(); a++)
			{
				const rapidjson::Value& jsonActor = SceneObjects[a];

				std::string DisplayName;
				std::string Type;
				json::get_string(jsonActor, "DisplayName", DisplayName);
				json::get_string(jsonActor, "Type", Type);
				EString ActorDisplayName = StringHelper::StringToWide(DisplayName);
				EString ActorType = StringHelper::StringToWide(Type);

				if (ActorType == TEXT("Actor")) {
					pNewActor = new GameFramework::AActor(ActorSceneIndex, ActorDisplayName);
					pNewActor->LoadFromJson(&jsonActor);
				}
				else if (ActorType == TEXT("PointLight")) {
					pNewActor = new APointLight(ActorSceneIndex, ActorDisplayName);
					pNewActor->LoadFromJson(&jsonActor);
				}
				else if (ActorType == TEXT("SpotLight")) {
					pNewActor = new ASpotLight(ActorSceneIndex, ActorDisplayName);
					pNewActor->LoadFromJson(&jsonActor);
				}
 				else if (ActorType == TEXT("DirectionalLight")) {
					pNewActor = new ADirectionalLight(ActorSceneIndex, ActorDisplayName);
					pNewActor->LoadFromJson(&jsonActor);
				}
				else if (ActorType == TEXT("SkySphere")) {
					pNewActor = new ASkySphere(ActorSceneIndex, ActorDisplayName);
						pNewActor->LoadFromJson(&jsonActor);
				}
				else if (ActorType == TEXT("SkyLight")) {
					pNewActor = new ASkyLight(ActorSceneIndex, ActorDisplayName);
						pNewActor->LoadFromJson(&jsonActor);
				}
				else if (ActorType == TEXT("PostFxVolume")) {
					pNewActor = new APostFx(ActorSceneIndex, ActorDisplayName);
					pNewActor->LoadFromJson(&jsonActor);
				}

				if (pNewActor == nullptr) {
					IE_LOG(Error, TEXT("Failed to parse actor \"%s\" into scene"), (ActorDisplayName.c_str() == TEXT("")) ? TEXT("INVALID NAME") : ActorDisplayName.c_str());
					continue;
				}

				pScene->AddActor(pNewActor);
				ActorSceneIndex++;
			}

			IE_LOG(Log, TEXT("Scene actors loaded."));
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
			std::string sceneName = "../Content/Scenes/" + pScene->GetDisplayName() + ".iescene/Meta.json";
			std::ofstream offstream(sceneName.c_str());
			offstream << StrBuffer.GetString();

			if (!offstream.good()) {
				IE_LOG(Error, TEXT("Failed to save meta data for scene: %s"), pScene->GetDisplayName().c_str());
				return false;
			}
		}

		// Save Out Actors.json
		{
			rapidjson::StringBuffer StrBuffer;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> Writer(StrBuffer);

			pScene->WriteToJson(&Writer);

			// Final Export
			std::string sceneName = "../Content/Scenes/" + pScene->GetDisplayName() + ".iescene/Actors.json";
			std::ofstream offstream(sceneName.c_str());
			offstream << StrBuffer.GetString();

			if (!offstream.good()) {
				IE_LOG(Error, TEXT("Failed to save actors for scene: %s"), pScene->GetDisplayName().c_str());
				return false;
			}
		}

		return true;
	}

	bool FileSystem::FileExistsInContentDirectory(const std::string& Path)
	{
		std::string RawPath;
#if IE_PLATFORM_BUILD_WIN32
		RawPath += "Content/" + Path;
		return PathFileExistsA(RawPath.c_str());
#elif IE_PLATFORM_BUILD_UWP
		RawPath += "Content/" + Path;
		#pragma message ("UWP: FileExistsInContentDirectory not implemented for this platform.")
		return false;
#endif
	}

	EString FileSystem::GetShaderPathW(const TChar* Shader)
	{
		EString Path = L"Shaders/";
		Path.append(Shader);
		return Path;
	}
}
