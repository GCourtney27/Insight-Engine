#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <strstream>
#include "..\\Engine.h"
#include "..\\Graphics\RenderableGameObject.h"
#include "..\\Objects\\Scene.h"
#include "..\\Objects\\Entity.h"
#include "..\\Framework\\Singleton.h"

class FileSystem : public Singleton<FileSystem>
{
public:
	FileSystem() {}
	bool Initialize(Engine* engine);

	static bool LoadSceneFromJSON(const std::string& sceneLocation, Scene* scene, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	static bool WriteSceneToJSON(std::list<Entity*>* gameObjectsToWrite);

	static bool LoadSceneFromFile(const std::string& sceneLocation, Scene* scene, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	static bool LoadSceneFromFileGO(const std::string& sceneLocation, std::vector<RenderableGameObject*>& gameObjectsToPopulate, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	static bool WriteSceneToFileGO(std::vector<RenderableGameObject*>& gameObjectsToWrite);
	static bool WriteSceneToFile(std::list<Entity*>* gameObjectsToWrite);


private:
	Engine* m_engine;
};