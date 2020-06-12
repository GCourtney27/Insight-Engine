#include <ie_pch.h>

#include "Insight/Core/Application.h"
#include "Insight/Core/Scene/Scene.h"
#include "Insight/Systems/Model_Manager.h"
#include "Insight/Utilities/String_Helper.h"

#include "File_System.h"

#include "Insight/Rendering/ASky_Sphere.h"
#include "Insight/Rendering/Lighting/ASpot_Light.h"
#include "Insight/Rendering/Lighting/APoint_Light.h"
#include "Insight/Rendering/Lighting/ADirectional_Light.h"



// TODO make a third part build file with imgui and rapidjson for all cpp files
#include <rapidjson/json.cpp>

namespace Insight {

	FileSystem* FileSystem::s_Instance = nullptr;

	FileSystem::FileSystem()
	{
		IE_CORE_ASSERT(!s_Instance, "There is already an instance of a file system!");
		s_Instance = this;
	}

	FileSystem::~FileSystem()
	{
	}

	std::string FileSystem::GetRelativeAssetDirectoryPath(std::string path)
	{
		std::string relativePath;
#if defined IE_DEBUG
		relativePath += "../Assets/" + path;
#elif defined IE_RELEASE
		relativePath += "../../../Assets/" + path;
#endif
		return relativePath;
	}

	bool FileSystem::LoadSceneFromJson(const std::string& fileName, Scene& scene)
	{
		// Load in Meta.json
		{
			ScopedTimer timer("LoadSceneFromJson::LoadMetaData");

			rapidjson::Document rawMetaFile;
			const std::string metaDir = fileName + "/Meta.json";
			if (!json::load(metaDir.c_str(), rawMetaFile)) {
				IE_CORE_ERROR("Failed to load meta file from scene: \"{0}\" from file.", fileName);
				return false;
			}
			std::string sceneName;
			json::get_string(rawMetaFile, "SceneName", sceneName); // Find something that says 'SceneName' and load sceneName variable
			scene.SetDisplayName(sceneName);
			Application::Get().GetWindow().SetWindowTitle(sceneName);

			IE_CORE_TRACE("Scene meta data loaded.");
		}

		// Load in Resources.json
		{
			ScopedTimer timer("LoadSceneFromJson::LoadResources");

			rapidjson::Document rawResourceFile;
			const std::string resorurceDir = fileName + "/Resources.json";
			if (!json::load(resorurceDir.c_str(), rawResourceFile)) {
				IE_CORE_ERROR("Failed to load resource file from scene: \"{0}\" from file.", fileName);
				return false;
			}

			IE_CORE_TRACE("Scene resouces loaded.");
		}

		// Load in Actors.json last once resources have been intialized
		{
			ScopedTimer timer("LoadSceneFromJson::LoadActors");

			rapidjson::Document rawActorsFile;
			const std::string actorsDir = fileName + "/Actors.json";
			if (!json::load(actorsDir.c_str(), rawActorsFile)) {
				IE_CORE_ERROR("Failed to load actor file from scene: \"{0}\" from file.", fileName);
				return false;
			}

			AActor* newActor = nullptr;
			UINT actorSceneIndex = 0;

			const rapidjson::Value& sceneObjects = rawActorsFile["Set"];
			for (rapidjson::SizeType a = 0; a < sceneObjects.Size(); a++)
			{
				const rapidjson::Value& jsonActor = sceneObjects[a];

				std::string actorDisplayName;
				std::string actorType;
				json::get_string(jsonActor, "DisplayName", actorDisplayName);
				json::get_string(jsonActor, "Type", actorType);

				if (actorType == "Actor") {
					newActor = new AActor(actorSceneIndex, actorDisplayName);
					newActor->LoadFromJson(jsonActor);
				}
				else if (actorType == "PointLight") {
					newActor = new APointLight(actorSceneIndex, actorDisplayName);
					newActor->LoadFromJson(jsonActor);
				}
				else if (actorType == "SpotLight") {
					newActor = new ASpotLight(actorSceneIndex, actorDisplayName);
					newActor->LoadFromJson(jsonActor);
				}
				else if (actorType == "DirectionalLight") {
					newActor = new ADirectionalLight(actorSceneIndex, actorDisplayName);
					newActor->LoadFromJson(jsonActor);
				}
				else if (actorType == "SkySphere") {
					newActor = new ASkySphere(actorSceneIndex, actorDisplayName);
					newActor->LoadFromJson(jsonActor);
				}
				else if (actorType == "PostFxVolume") {
					newActor = new APostFx(actorSceneIndex, actorDisplayName);
					newActor->LoadFromJson(jsonActor);
				}

				if (newActor == nullptr) {
					IE_CORE_ERROR("Failed to parse actor \"{0}\" into scene", (actorDisplayName == "") ? "INVALID NAME" : actorDisplayName);
					continue;
				}

				scene.GetRootNode()->AddChild(newActor);
				actorSceneIndex++;
			}

			IE_CORE_TRACE("Scene actors loaded.");
		}

		return true;
	}

}
