#include "FileSystem.h"
#include "..\\Graphics\\AdapterReader.h"
#include "..\\Graphics\Shaders.h"
#include <WICTextureLoader.h>

#include "..\\Components\MeshRenderComponent.h"
#include "..\\Components\\EditorSelectionComponent.h"

#include "document.h"

bool FileSystem::Initialize(Engine * engine)
{
	m_engine = engine;

	return true;
}

bool FileSystem::LoadSceneFromJSON(const std::string & sceneLocation, Scene * scene, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{

	return true;
}

bool FileSystem::WriteSceneToJSON(std::list<Entity*>* gameObjectsToWrite)
{

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

	//std::list<Entity*>* entities = m_engine->GetScene().GetAllEntities();
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

