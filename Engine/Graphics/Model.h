#pragma once
#include "Mesh.h"
#include "Material.h"

using namespace DirectX;

class Model
{
public:
	bool Initialize(const std::string & filepath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_PerObject>& CB_VS_PerObject, Material* material);
	void Draw(const XMMATRIX & worldMatrix, const XMMATRIX & projectionMatrix, const XMMATRIX & viewMatrix);

	std::string GetDirectory() { return directory; }
	std::string GetModelDirectory() { return modelDirectory; }

	Material* GetMaterial() { return m_pMaterial; }

private:
	std::vector<Mesh> meshes; // A model can have a bunch of meshes 

	bool LoadModel(const std::string & filepath);
	void ProcessNode(aiNode * node, const aiScene * scene, const XMMATRIX & parentTransformMatrix);
	Mesh ProcessMesh(aiMesh * mesh, const aiScene * scene, const XMMATRIX & transformMatrix);
	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial * pMat, unsigned int index, aiTextureType textureType);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);
	int GetTextureIndex(aiString* pStr);

	Material* m_pMaterial = nullptr;

	ID3D11Device * device = nullptr;
	ID3D11DeviceContext * deviceContext = nullptr;
	ConstantBuffer<CB_VS_PerObject> * cb_vs_PerObject = nullptr;
	std::string directory = "";
	std::string modelDirectory = "";
};