#include "FileSystem.h"
#include "..\\Graphics\\AdapterReader.h"
#include "..\\Graphics\Shaders.h"
#include "..\Editor\Editor.h"
#include <WICTextureLoader.h>

#include "..\\Components\MeshRenderComponent.h"
#include "..\\Components\\EditorSelectionComponent.h"
#include "..\Components\LuaScriptComponent.h"

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
	m_engine = engine;

	return true;
}

struct com_mesh;
struct com_physics;

struct com_base
{
	virtual void Load(const rapidjson::Value& value) = 0;
	static com_base* Create(std::string& name);
	//{
	//	com_base* p = nullptr;
	//	if (name == "mesh")
	//	{
	//		p = new com_mesh;
	//	}
	//	else if (name == "physics")
	//	{
	//		p = new com_physics;
	//	}

	//	return p;
	//}
};

struct com_mesh : com_base
{
	void Load(const rapidjson::Value& value)
	{
		json::get_int(value, "size", size);
	}

	int size;
};

struct com_physics: com_base
{
	void Load(const rapidjson::Value& value)
	{
		json::get_float(value, "rate", rate);
	}

	float rate;
};

com_base* com_base::Create(std::string& name)
{
	com_base* p = nullptr;
	if (name == "mesh")
	{
		p = new com_mesh;
	}
	else if (name == "physics")
	{
		p = new com_physics;
	}

	return p;
}


bool FileSystem::LoadSceneFromJSONExample(const std::string & sceneLocation, Scene * scene, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	rapidjson::Document document;
	json::load(sceneLocation.c_str(), document);

	std::string name;
	json::get_string(document, "name", name);

	int i;
	json::get_int(document, "int_value", i);

	const rapidjson::Value& block = document["data"];
	json::get_string(block, "name", name);
	json::get_int(block, "int_value", i);

	const rapidjson::Value& components = document["components"];
	for (rapidjson::SizeType i = 0; i < components.Size(); i++)
	{
		const rapidjson::Value& component = components[i];

		std::string type;
		json::get_string(component, "type", type);

		com_base* com = com_base::Create(type);
		com->Load(component);

	}

	return true;
}

bool FileSystem::LoadSceneFromJSON(const std::string & sceneLocation, Scene * scene, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
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
		//else if (objectType == "Light") // TODO: Add light class
		//{
		//	if (entity != nullptr)
		//	{
		//		id = new ID();
		//		entity = new Entity(scene, (*id));
		//		//dynamic_cast<Light*>(entity);
		//	}
		//}
		
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
#pragma endregion Load Entity transform

		// Loop through Components array
		const rapidjson::Value& allComponents = sceneObjects[o]["Components"]; // Gets json array of all components

		// MESH RENDERER
		const rapidjson::Value& meshRenderer = allComponents[0]["MeshRenderer"];
		MeshRenderer* finalMesh = nullptr;
		std::string modelFilePath;
		for (rapidjson::SizeType m = 0; m < meshRenderer.Size(); m++)
		{
			json::get_string(meshRenderer[m], "Model", modelFilePath);
			if (modelFilePath != "NONE")
				finalMesh = entity->AddComponent<MeshRenderer>();
			else
				break;
		}
		if (finalMesh != nullptr)
			finalMesh->Initialize(entity, modelFilePath, device, deviceContext, cb_vs_vertexshader);
		
		// LUA SCRIPT(s)
		const rapidjson::Value& luaScript = allComponents[1]["LuaScript"];
		LuaScript* finalScript = nullptr;
		std::string scriptFilePath;
		for (rapidjson::SizeType l = 0; l < luaScript.Size(); l++)
		{
			json::get_string(luaScript[l], "FilePath", scriptFilePath);
			if (scriptFilePath != "NONE")
				finalScript = entity->AddComponent<LuaScript>();
			else
				break;
		}
		if (finalScript != nullptr)
			finalScript->Initialize(entity, scriptFilePath);
		

		// EDITOR SELECTION
		const rapidjson::Value& editorSelection = allComponents[2]["EditorSelection"];
		EditorSelection* finalComp = nullptr;
		for (rapidjson::SizeType e = 0; e < editorSelection.Size(); e++)
		{
			json::get_string(editorSelection[e], "Mode", modelFilePath);
			if (modelFilePath != "OFF")
				finalComp = entity->AddComponent<EditorSelection>();
			else
				break;
		}
		if(finalComp != nullptr)
			finalComp->Initialize(entity, 20.0f, entity->GetTransform().GetPosition());





		scene->AddEntity(entity);

		id = nullptr;
		entity = nullptr;

	}// End Load Entity

	return true;
}

bool FileSystem::WriteSceneToJSON(Scene* scene)
{

	using namespace rapidjson;
	//std::stringstream stream;

	StringBuffer s;
	PrettyWriter<StringBuffer> writer(s);
	writer.StartObject(); // Start File

	writer.Key("SceneName");
	writer.String(scene->GetSceneName().c_str());

	writer.Key("Objects");
	writer.StartArray();// Start Objects

	std::list<Entity*>::iterator iter;
	for (iter = scene->GetAllEntities()->begin(); iter != scene->GetAllEntities()->end(); iter++)
	{
		writer.StartObject(); // Start Entity

		writer.Key("Type");
		writer.String("Entity");

		writer.Key("Name");
		writer.String((*iter)->GetID().GetName().c_str());

#pragma region Write Transform
		writer.Key("Transform");
		writer.StartArray(); // Start Transform

		// POSITION
		writer.StartObject(); // Start Array Object
		writer.Key("Position");
		writer.StartArray(); // Start Position
		// X
		writer.StartObject(); // Start X
		writer.Key("x");
		writer.Double((*iter)->GetTransform().GetPosition().x);
		writer.EndObject(); // End X
		// Y
		writer.StartObject(); // Start Y
		writer.Key("y");
		writer.Double((*iter)->GetTransform().GetPosition().y);
		writer.EndObject(); // End Y

		// Z
		writer.StartObject(); // Start Z
		writer.Key("z");
		writer.Double((*iter)->GetTransform().GetPosition().z);
		writer.EndObject(); // End Z
		writer.EndArray(); // End Pisition
		writer.EndObject(); // End Array Object

		// ROTATION
		writer.StartObject(); // Start Array Object
		writer.Key("Rotation");
		writer.StartArray(); // Start Position
		// X
		writer.StartObject(); // Start X
		writer.Key("x");
		writer.Double((*iter)->GetTransform().GetRotation().x);
		writer.EndObject(); // End X
		// Y
		writer.StartObject(); // Start Y
		writer.Key("y");
		writer.Double((*iter)->GetTransform().GetPosition().y);
		writer.EndObject(); // End Y

		// Z
		writer.StartObject(); // Start Z
		writer.Key("z");
		writer.Double((*iter)->GetTransform().GetRotation().z);
		writer.EndObject(); // End Z
		writer.EndArray(); // End Pisition
		writer.EndObject(); // End Array Object

		// SCALE
		writer.StartObject(); // Start Array Object
		writer.Key("Scale");
		writer.StartArray(); // Start Position
		// X
		writer.StartObject(); // Start X
		writer.Key("x");
		writer.Double((*iter)->GetTransform().GetScale().x);
		writer.EndObject(); // End X
		// Y
		writer.StartObject(); // Start Y
		writer.Key("y");
		writer.Double((*iter)->GetTransform().GetScale().y);
		writer.EndObject(); // End Y

		// Z
		writer.StartObject(); // Start Z
		writer.Key("z");
		writer.Double((*iter)->GetTransform().GetScale().z);
		writer.EndObject(); // End Z
		writer.EndArray(); // End Pisition
		writer.EndObject(); // End Array Object

		writer.EndArray(); // End Transform
#pragma endregion Write Transform to file

		writer.Key("Components");
		writer.StartArray(); // Start Comonents Array

		// MESH RENDERER
		writer.StartObject();// Start Mesh Renderer
		writer.Key("MeshRenderer");
		writer.StartArray(); // Start Mesh Renderer Array

		writer.StartObject();
		writer.Key("Model");
		writer.String((*iter)->GetComponent<MeshRenderer>()->GetModel()->GetModelDirectory().c_str());
		writer.EndObject();

		writer.EndArray(); // End Mesh Renderer Array
		writer.EndObject();// End Mesh Renderer
		
		// LUA SCRIPT(S)
		writer.StartObject();// Start Lua Script
		writer.Key("LuaScript");
		writer.StartArray(); // Start Lua Script Array

		writer.StartObject();
		writer.Key("FilePath");
		writer.String((*iter)->GetComponent<LuaScript>()->GetFilePath().c_str());
		writer.EndObject();

		writer.EndArray(); // End Lua Script Array
		writer.EndObject();// End Lua Script

		// EDITOR SELECTION
		writer.StartObject();// Start Editor Selection
		writer.Key("EditorSelection");
		writer.StartArray(); // Start Editor Selection Array

		writer.StartObject();
		writer.Key("Mode");
		writer.String("DEFAULT");
		writer.EndObject();

		writer.EndArray(); // End Editor Selection Array
		writer.EndObject();// End Editor Selection

		writer.EndArray(); // End Components Array

		writer.EndObject(); // End Entity
	}


	writer.EndArray(); // End Objects
	writer.EndObject(); // End File

	// Final Export
	std::string sceneName = "Data\\Scenes\\" + scene->GetSceneName();
	std::ofstream offstream(/*sceneName.c_str()*/"Data\\Scenes\\scene_json.txt");
	offstream << s.GetString();

	if (!offstream.good())
		ErrorLogger::Log("Failed to write to json file");

	return true;
}

bool FileSystem::LoadSceneFromFile(const std::string & sceneLocation, Scene * scene, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	std::fstream f(sceneLocation);
	if (!f.is_open())
		return false;

	ID* id = nullptr;
	Entity* entity = nullptr;

	std::string name;
	std::string modelFilepath;
	float px, py, pz;
	float rx, ry, rz;
	float sx, sy, sz;

	while (!f.eof())
	{
		if (entity == nullptr)
		{
			id = new ID();
			entity = new Entity(scene, *id);
		}

		char line[128];
		f.getline(line, 128);

		std::strstream s;
		s << line;

		char lineInformation;

		if (line[0] == 'n')
		{
			s >> lineInformation >> name;
		}

		if (line[0] == 'f')
		{
			s >> lineInformation >> modelFilepath;
			MeshRenderer* me = entity->AddComponent<MeshRenderer>();
			me->Initialize(entity, modelFilepath, device, deviceContext, cb_vs_vertexshader);
		}

		if (line[0] == 'p')
		{
			s >> lineInformation >> px >> py >> pz;
		}

		if (line[0] == 'r')
		{
			s >> lineInformation >> rx >> ry >> rz;
		}

		if (line[0] == 's')
		{
			s >> lineInformation >> sx >> sy >> sz;

			//entity->SetTag(name);
			entity->GetID().SetName(name);
			entity->GetID().SetTag("DEFAULT_TAG");

			entity->GetTransform().SetPosition(DirectX::XMFLOAT3(px, py, pz));
			entity->GetTransform().SetRotation(rx, ry, rz);
			entity->GetTransform().SetScale(sx, sy, sz);

			EditorSelection* esc = entity->AddComponent<EditorSelection>();
			esc->Initialize(entity, 20.0f, entity->GetTransform().GetPosition());

			scene->AddEntity(entity);

			id = nullptr;
			entity = nullptr;
		}
	}
	delete id;
	delete entity;

	return true;
}

bool FileSystem::LoadSceneFromFileGO(const std::string & sceneLocation, std::vector<RenderableGameObject*>& gameObjectsToPopulate, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	std::fstream f(sceneLocation);
	if (!f.is_open())
		return false;
	

	RenderableGameObject* go = new RenderableGameObject();
	std::string name;
	std::string modelFilepath;
	float px, py, pz;
	float rx, ry, rz;
	float sx, sy, sz;

	while (!f.eof())
	{
		if (go == nullptr)
			go = new RenderableGameObject();
			

		char line[128];
		f.getline(line, 128);

		std::strstream s;
		s << line;

		
		char lineInformation;

		if (line[0] == 'n')
		{
			s >> lineInformation >> name;
		}

		if (line[0] == 'f')
		{
			s >> lineInformation >> modelFilepath;
		}

		if (line[0] == 'p')
		{
			s >> lineInformation >> px >> py >> pz;
		}

		if (line[0] == 'r')
		{
			s >> lineInformation >> rx >> ry >> rz;
		}

		if (line[0] == 's')
		{
			s >> lineInformation >> sx >> sy >> sz;

			if (!go->Initialize(modelFilepath, device, deviceContext, cb_vs_vertexshader))
				return false;
			go->SetName(name);

			go->SetPosition(DirectX::XMFLOAT3(px, py, pz));
			go->SetRotation(rx, ry, rz);
			go->SetScale(sx, sy, sz);
			
			gameObjectsToPopulate.push_back(go);
			go = nullptr;
		}

	}

	return true;
}

bool FileSystem::WriteSceneToFileGO(std::vector<RenderableGameObject*>& gameObjectsToWrite)
{
	std::ofstream os("Data//Scenes//Scene01.txt");
	size_t gos = gameObjectsToWrite.size();
	
	for (int i = 0; i < gos; i++)
	{
		os << "n " << gameObjectsToWrite[i]->GetName() << std::endl;
		os << "f " << gameObjectsToWrite[i]->GetModel()->GetModelDirectory() << std::endl;
		os << "p " << gameObjectsToWrite[i]->GetPositionFloat3().x << " " << gameObjectsToWrite[i]->GetPositionFloat3().y << " " << gameObjectsToWrite[i]->GetPositionFloat3().z << std::endl;
		os << "r " << gameObjectsToWrite[i]->GetRotationFloat3().x << " " << gameObjectsToWrite[i]->GetRotationFloat3().y << " " << gameObjectsToWrite[i]->GetRotationFloat3().z << std::endl;
		os << "s " << gameObjectsToWrite[i]->GetScale().x << " " << gameObjectsToWrite[i]->GetScale().y << " " << gameObjectsToWrite[i]->GetScale().z << std::endl;
	}
	
	return true;
}

bool FileSystem::WriteSceneToFile(std::list<Entity*>* entitiesToWrite)
{
	std::ofstream os("Data//Scenes//Scene01.txt");

	std::list<Entity*>::iterator iter;
	for (iter = entitiesToWrite->begin(); iter != entitiesToWrite->end(); iter++)
	{
		MeshRenderer* mr = (*iter)->GetComponent<MeshRenderer>();

		os << "n " << (*iter)->GetID().GetName() << std::endl;
		os << "f " << mr->GetModel()->GetModelDirectory() << std::endl;
		os << "p " << (*iter)->GetTransform().GetPositionFloat3().x << " " << (*iter)->GetTransform().GetPositionFloat3().y << " " << (*iter)->GetTransform().GetPositionFloat3().z << std::endl;
		os << "r " << (*iter)->GetTransform().GetRotationFloat3().x << " " << (*iter)->GetTransform().GetRotationFloat3().y << " " << (*iter)->GetTransform().GetRotationFloat3().z << std::endl;
		os << "s " << (*iter)->GetTransform().GetScale().x << " " << (*iter)->GetTransform().GetScale().y << " " << (*iter)->GetTransform().GetScale().z << std::endl;
		

	}
	return true;
}

