#include "FileLoader.h"
#include "Graphics/AdapterReader.h"
#include "Graphics/Shaders.h"
#include <WICTextureLoader.h>

bool FileLoader::LoadSceneFromFile(const std::string & sceneLocation, std::vector<RenderableGameObject*>& gameObjectsToPopulate, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	std::fstream f(sceneLocation);
	if (!f.is_open())
		return false;

	RenderableGameObject* go = new RenderableGameObject();
	std::string name;
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
			//if (!go->Initialize("Data\\Objects\\nanosuit\\nanosuit.obj", device, deviceContext, cb_vs_vertexshader))
			if (!go->Initialize("Data\\Objects\\Dandelion\\Var1\\Textured_Flower.obj", device, deviceContext, cb_vs_vertexshader))
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

bool FileLoader::WriteSceneToFile(std::vector<RenderableGameObject*>& gameObjectsToWrite)
{
	std::ofstream os("Data//Scenes//Scene01.txt");
	size_t gos = gameObjectsToWrite.size();
	
	for (int i = 0; i < gos; i++)
	{
		os << "n " << gameObjectsToWrite[i]->GetName() << std::endl;
		os << "p " << gameObjectsToWrite[i]->GetPositionFloat3().x << " " << gameObjectsToWrite[i]->GetPositionFloat3().y << " " << gameObjectsToWrite[i]->GetPositionFloat3().z << std::endl;
		os << "r " << gameObjectsToWrite[i]->GetRotationFloat3().x << " " << gameObjectsToWrite[i]->GetRotationFloat3().y << " " << gameObjectsToWrite[i]->GetRotationFloat3().z << std::endl;
		os << "s " << gameObjectsToWrite[i]->GetScale().x << " " << gameObjectsToWrite[i]->GetScale().y << " " << gameObjectsToWrite[i]->GetScale().z << std::endl;
	}
	
	return true;
}
