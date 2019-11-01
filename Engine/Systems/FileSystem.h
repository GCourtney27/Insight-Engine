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
#include <future>

class FileSystem : public Singleton<FileSystem>
{
public:
	FileSystem() {}
	bool Initialize(Engine* engine);

	bool LoadSceneFromJSON(const std::string& sceneLocation, Scene* scene, ID3D11Device * device, ID3D11DeviceContext * deviceContext);
	static bool WriteSceneToJSON(Scene* scene);


private:
	Engine* m_pEngine;
	
};