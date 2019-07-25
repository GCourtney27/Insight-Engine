#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <strstream>
#include "Graphics/RenderableGameObject.h"


class FileLoader
{
public:
	static bool LoadSceneFromFile(const std::string& sceneLocation, std::vector<RenderableGameObject*>& gameObjectsToPopulate, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	static bool WriteSceneToFile(std::vector<RenderableGameObject*>& gameObjectsToWrite);
};