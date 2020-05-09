#include "FileSystem.h"
#include "..\\Graphics\\AdapterReader.h"
#include "..\\Graphics\Shaders.h"
#include "..\Editor\Editor.h"
#include "..\Graphics\Graphics.h"

#include "..\\Components\MeshRenderComponent.h"
#include "..\\Components\\EditorSelectionComponent.h"
#include "..\Components\LuaScriptComponent.h"
#include "..\Components\RigidBodyComponent.h"
#include "..\Graphics\Material.h"

#include "..\Objects\Player.h"

#include "json.h"
#include "rapidjson/document.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/prettywriter.h"

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

	// Physics system
	static int colliderTag = 1;

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
		json::get_float(position[0], "x", px);
		json::get_float(position[0], "y", py);
		json::get_float(position[0], "z", pz);
		
		const rapidjson::Value& rotation = transform[1]["Rotation"];
		json::get_float(rotation[0], "x", rx);
		json::get_float(rotation[0], "y", ry);
		json::get_float(rotation[0], "z", rz);
		
		const rapidjson::Value& scale = transform[2]["Scale"];
		json::get_float(scale[0], "x", sx);
		json::get_float(scale[0], "y", sy);
		json::get_float(scale[0], "z", sz);

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
		MeshRenderer* mr = nullptr;
		json::get_string(meshRenderer[0], "Model", model_FilePath);
		if (model_FilePath != "NONE" && !foundModel)
			foundModel = true;
		if (foundModel)
		{
			mr = entity->AddComponent<MeshRenderer>();
			mr->InitFromJSON(entity, meshRenderer);

			// Determine what kind of material the object posseses
			// so they can be drawn in the apropriat order in the render pipeline
			if (mr->GetModel()->GetMaterial()->GetMaterialFlags() == Material::eFlags::FOLIAGE)
				scene->GetRenderManager().AddFoliageObject(mr);
			
			if (mr->GetModel()->GetMaterial()->GetMaterialFlags() == Material::eFlags::NOFLAGS)
				scene->GetRenderManager().AddOpaqueObject(mr);

			if (mr->GetModel()->GetMaterial()->GetMaterialFlags() == Material::eFlags::WATER)
				scene->GetRenderManager().AddFoliageObject(mr);
		}
		entity->SetHasMeshRenderer(foundModel);
		
		// LUA SCRIPT(s)
		const rapidjson::Value& luaScript = allComponents[1]["LuaScript"];
		LuaScript* ls = entity->AddComponent<LuaScript>();
		ls->InitFromJSON(entity, luaScript);
		scene->GetLuaManager().AddScript(ls);

		// EDITOR SELECTION
		const rapidjson::Value& editorSelection = allComponents[2]["EditorSelection"];
		bool canBeSelected = false;
		std::string mode;
		json::get_string(editorSelection[0], "Mode", mode);
		if (mode != "OFF")
			canBeSelected = true;
		if(canBeSelected)
			entity->AddComponent<EditorSelection>()->InitFromJSON(entity, editorSelection);
		entity->SetHasEditorSelection(canBeSelected);

		// RIGID BODY
		if (scene->IsPhysicsEnabled())
		{
			const rapidjson::Value& rigidBody = allComponents[3]["RigidBody"];
			std::string hasPhysics;
			RigidBody* ps = nullptr;
			json::get_string(rigidBody[0], "ColliderType", hasPhysics);
			if (hasPhysics != "NONE")
			{
				ps = entity->AddComponent<RigidBody>();
				ps->InitFromJSON(entity, rigidBody);
				ps->SetColliderTag(colliderTag);
				colliderTag++;

				scene->GetPhysicsSystem().AddEntity(ps);
			}
		}


		scene->AddEntity(entity);

		id = nullptr;
		entity = nullptr;

	}// End Load Entity

	//const rapidjson::Value& worldObjects = masterDocument["World"]; // Arr
	//for (rapidjson::SizeType o = 0; o < worldObjects.Size(); o++)
	//{
	//	const rapidjson::Value& object = worldObjects[o]; // Current object in Objects array  (unique entity, perform calls on this object)

	//	std::string objectType;
	//	json::get_string(object, "Type", objectType);
	//	if (objectType == "Sky")
	//	{
	//		if (entity == nullptr)
	//		{
	//			id = new ID();
	//			entity = new Entity(scene, (*id));
	//		}
	//		const rapidjson::Value& allComponents = object["Components"]; // Gets json array of all components

	//		// MESH RENDERER
	//		const rapidjson::Value& meshRenderer = allComponents[0]["MeshRenderer"];
	//		bool foundModel = false;
	//		std::string model_FilePath;
	//		json::get_string(meshRenderer[0], "Model", model_FilePath);
	//		if (model_FilePath != "NONE" && !foundModel)
	//			foundModel = true;
	//		if (foundModel)
	//			entity->AddComponent<MeshRenderer>()->InitFromJSON(entity, meshRenderer);

	//		Graphics::Instance()->SetSkybox(entity);
	//		Graphics::Instance()->InitSkybox();
	//		break;
	//	}
	//}

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
		if((*iter)->CanBeJSONSaved())
			(*iter)->WriteToJSON(writer);
	}

	writer.EndArray(); // End Objects
	writer.EndObject(); // End File

	// Final Export
	std::string sceneName = "..\\Assets\\Scenes\\NewScene\\" + scene->GetSceneName() + ".json";
	DEBUGLOG("[FileSystem] " + sceneName);
	std::ofstream offstream(sceneName.c_str());
	offstream << sb.GetString();

	if (!offstream.good())
		ErrorLogger::Log("Failed to write to json file");

	return true;
}
