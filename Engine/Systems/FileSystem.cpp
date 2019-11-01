#include "FileSystem.h"
#include "..\\Graphics\\AdapterReader.h"
#include "..\\Graphics\Shaders.h"
#include "..\Editor\Editor.h"
#include <WICTextureLoader.h>

#include "..\\Components\MeshRenderComponent.h"
#include "..\\Components\\EditorSelectionComponent.h"
#include "..\Components\LuaScriptComponent.h"
#include "..\Graphics\Material.h"

#include "..\Objects\Player.h"

#include "json.h"
#include "document.h"
#include "filewritestream.h"
#include "istreamwrapper.h"
#include "writer.h"
#include "stringbuffer.h"
#include "ostreamwrapper.h"
#include "prettywriter.h"

#include <map>

bool FileSystem::Initialize(Engine * engine)
{
	m_pEngine = engine;

	return true;
}

bool FileSystem::LoadSceneFromJSON(const std::string & sceneLocation, Scene * scene, ID3D11Device * device, ID3D11DeviceContext * deviceContext)
{
	// Load document from file and verify it was found
	rapidjson::Document masterDocument;
	if(!json::load(sceneLocation.c_str(), masterDocument))
	{
		ErrorLogger::Log("Failed to load scene from JSON file.");
		return false;
	}
	// Parse scene name
	std::string sceneName;
	json::get_string(masterDocument, "SceneName", sceneName); // Find something that says 'SceneName' and load sceneName variable
	scene->SetSceneName(sceneName);

	ID* id = nullptr;
	Entity* entity = nullptr;

	// Load objects
	const rapidjson::Value& sceneObjects = masterDocument["Objects"]; // Arr


	for (rapidjson::SizeType o = 0; o < sceneObjects.Size(); o++)
	{
		const rapidjson::Value& object = sceneObjects[o]; // Current object in Objects array  (unique entity, perform calls on this object)

		std::string objectType;
		json::get_string(object, "Type", objectType);
		if (objectType == "Entity")
		{
			if (entity == nullptr)
			{
				id = new ID();
				entity = new Entity(scene, (*id));
			}
		}
		else if (objectType == "Player")
		{
			if (entity == nullptr)
			{
				id = new ID();
				entity = new Player(scene, (*id));
				m_pEngine->SetPlayer(dynamic_cast<Player*>(entity));
			}
		}
		//else if (objectType == "Sky")
		//{
		//	if (entity == nullptr)
		//	{
		//		id = new ID();
		//		entity = new Entity(scene, (*id));
		//		//Graphics::Instance()->InitSkybox(object);
		//		continue;
		//	}
		//}
		//else if (objectType == "Light") // TODO: Add light class
		//{
		//	if (entity != nullptr)
		//	{
		//		id = new ID();
		//		entity = new Entity(scene, (*id));
		//		//dynamic_cast<Light*>(entity);
		//	}
		//}
		entity->GetID().SetType(objectType);

		std::string objectName;
		json::get_string(object, "Name", objectName);
		entity->GetID().SetName(objectName);
		entity->GetID().SetTag("Untagged");

#pragma region Load Transform
		// Loop through Transform array
		float px, py, pz;
		float rx, ry, rz;
		float sx, sy, sz;
		const rapidjson::Value& transform = sceneObjects[o]["Transform"]; // At spot o in the sceneObjects array (Number of spots is how ever many objects are in the scene)
		const rapidjson::Value& position = transform[0]["Position"];
		for (rapidjson::SizeType p = 0; p < position.Size(); p++)
		{
			json::get_float(position[p], "x", px);
			json::get_float(position[p], "y", py);
			json::get_float(position[p], "z", pz);
		}
		const rapidjson::Value& rotation = transform[1]["Rotation"];
		for (rapidjson::SizeType r = 0; r < rotation.Size(); r++)
		{
			json::get_float(rotation[r], "x", rx);
			json::get_float(rotation[r], "y", ry);
			json::get_float(rotation[r], "z", rz);
		}
		const rapidjson::Value& scale = transform[2]["Scale"];
		for (rapidjson::SizeType s = 0; s < scale.Size(); s++)
		{
			json::get_float(scale[s], "x", sx);
			json::get_float(scale[s], "y", sy);
			json::get_float(scale[s], "z", sz);
		}
		entity->GetTransform().SetPosition(DirectX::XMFLOAT3(px, py, pz));
		entity->GetTransform().SetRotation(rx, ry, rz);
		entity->GetTransform().SetScale(sx, sy, sz);
#pragma endregion Load Entity Transform

		// Loop through Components array
		const rapidjson::Value& allComponents = object["Components"]; // Gets json array of all components

		// MESH RENDERER
		const rapidjson::Value& meshRenderer = allComponents[0]["MeshRenderer"];
		bool foundModel = false;
		std::string model_FilePath;
		json::get_string(meshRenderer[0], "Model", model_FilePath);
		if (model_FilePath != "NONE" && !foundModel)
			foundModel = true;
		if(foundModel)
			entity->AddComponent<MeshRenderer>()->InitFromJSON(entity, meshRenderer);
		
		
		// LUA SCRIPT(s)
		const rapidjson::Value& luaScript = allComponents[1]["LuaScript"];
		bool foundScript = false;
		std::string scriptFilePath;
		json::get_string(luaScript[0], "FilePath", scriptFilePath);
		if (scriptFilePath != "NONE")
			foundScript = true;
		if (foundScript)
			entity->AddComponent<LuaScript>()->InitFromJSON(entity, luaScript);
		

		// EDITOR SELECTION
		const rapidjson::Value& editorSelection = allComponents[2]["EditorSelection"];
		bool canBeSelected = false;
		std::string mode;
		json::get_string(editorSelection[0], "Mode", mode);
		if (mode != "OFF")
			canBeSelected = true;
		if(canBeSelected)
			entity->AddComponent<EditorSelection>()->InitFromJSON(entity, editorSelection);


		scene->AddEntity(entity);

		id = nullptr;
		entity = nullptr;

	}// End Load Entity

	return true;
}

bool FileSystem::WriteSceneToJSON(Scene* scene)
{
	using namespace rapidjson;

	StringBuffer sb;
	PrettyWriter<StringBuffer> writer(sb);
	writer.StartObject(); // Start File

	writer.Key("SceneName");
	writer.String(scene->GetSceneName().c_str());

	writer.Key("Objects");
	writer.StartArray();// Start Objects

	std::list<Entity*>::iterator iter;
	for (iter = scene->GetAllEntities()->begin(); iter != scene->GetAllEntities()->end(); iter++)
	{
		(*iter)->WriteToJSON(writer);
	}

	writer.EndArray(); // End Objects
	writer.EndObject(); // End File

	// Final Export
	std::string sceneName = "..\\Assets\\Scenes\\" + scene->GetSceneName() + ".json";
	DEBUGLOG("[FileSystem.cpp] " + sceneName);
	std::ofstream offstream(sceneName.c_str());
	offstream << sb.GetString();

	if (!offstream.good())
		ErrorLogger::Log("Failed to write to json file");

	return true;
}
