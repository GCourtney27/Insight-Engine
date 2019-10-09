#include "FileSystem.h"
#include "..\\Graphics\\AdapterReader.h"
#include "..\\Graphics\Shaders.h"
#include <WICTextureLoader.h>

#include "..\\Components\MeshRenderComponent.h"
#include "..\\Components\\EditorSelectionComponent.h"

#include "json.h"
#include "document.h"
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
		entity->GetID().SetTag("DEFAULT_TAG");

#pragma region Load Transform
		// Loop through transform array
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

		std::string modelFilePath = "Data\\Objects\\Dandelion\\Var1\\Textured_Flower.obj";
		//std::string modelFilePath = "Data\\Objects\\cube.obj";
		MeshRenderer* me = entity->AddComponent<MeshRenderer>();
		me->Initialize(modelFilePath, device, deviceContext, cb_vs_vertexshader);

		EditorSelection* esc = entity->AddComponent<EditorSelection>();
		esc->Initialize(20.0f, entity->GetTransform().GetPosition());

		scene->AddEntity(entity);

		id = nullptr;
		entity = nullptr;

	}// End Load Entity

	return true;
}

bool FileSystem::WriteSceneToJSON(Scene* scene)
{
	std::ofstream os("Data\\Scenes\\scene_json_test.txt");


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
			me->Initialize(modelFilepath, device, deviceContext, cb_vs_vertexshader);
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
			esc->Initialize(20.0f, entity->GetTransform().GetPosition());

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

