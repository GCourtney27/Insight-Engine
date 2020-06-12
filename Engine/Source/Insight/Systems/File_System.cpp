#include <ie_pch.h>

#include "Insight/Core/Scene/Scene.h"

#include "File_System.h"
#include "Insight/Rendering/ASky_Sphere.h"
#include "Insight/Rendering/Lighting/ASpot_Light.h"
#include "Insight/Rendering/Lighting/APoint_Light.h"
#include "Insight/Rendering/Lighting/ADirectional_Light.h"
#include "Insight/Utilities/String_Helper.h"

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
		// TODO Load in Scene_Meta.json first

		// TODO Load in Resources.json second

		// Load in Actors.json last once resources have been intialized
		rapidjson::Document rawActorsFile;
		const std::string actorsDir = fileName + "/Actors.json";
		if (!json::load(actorsDir.c_str(), rawActorsFile)) {
			IE_CORE_ERROR("Failed to load scene: \"{0}\" from file.", fileName);
			return false;
		}

		// TODO Get the scene name from Scene_Meta.json
		std::string sceneName;
		json::get_string(rawActorsFile, "SceneName", sceneName); // Find something that says 'SceneName' and load sceneName variable
		scene.SetDisplayName(sceneName);

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
				IE_CORE_ERROR("Failed to parse actor {0} into scene", actorDisplayName);
				continue;
			}


			scene.GetRootNode()->AddChild(newActor);
			actorSceneIndex++;
		}

		return true;
	}

}
