#pragma once
#include "Mesh.h"

using namespace DirectX;

class Model
{
public:
	bool Initialize(const std::string & filepath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader);
	void Draw(const XMMATRIX & worldMatrix, const XMMATRIX & viewProjectionMatrix);

	std::string GetDirectory() { return directory; }
	std::string GetModelDirectory() { return modelDirectory; }
private:
	std::vector<Mesh> meshes; // A model can have a bunch of meshes 
	bool LoadModel(const std::string & filepath);
	void ProcessNode(aiNode * node, const aiScene * scene, const XMMATRIX & parentTransformMatrix);
	Mesh ProcessMesh(aiMesh * mesh, const aiScene * scene, const XMMATRIX & transformMatrix);
	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial * pMat, unsigned int index, aiTextureType textureType);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);
	int GetTextureIndex(aiString* pStr);

	ID3D11Device * device = nullptr;
	ID3D11DeviceContext * deviceContext = nullptr;
	ConstantBuffer<CB_VS_vertexshader> * cb_vs_vertexshader = nullptr;
	std::string directory = "";
	std::string modelDirectory = "";
};