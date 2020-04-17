#include "ie_pch.h"

#include "Model.h"

namespace Insight {



	Model::Model(const std::string& path)
	{
		LoadModel(path);
	}

	Model::~Model()
	{
		Destroy();
	}

	void Model::Init(const std::string path)
	{
		LoadModel(path);
	}

	void Model::Draw()
	{
		for (UINT i = 0; i < m_Meshes.size(); i++)
			m_Meshes[i].Draw();
	}

	void Model::Destroy()
	{
		for (UINT i = 0; i < m_Meshes.size(); i++)
			m_Meshes[i].Destroy();
	}

	void Model::LoadModel(const std::string& path)
	{
		Assimp::Importer importer;
		const aiScene* pScene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals);

		if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
			IE_CORE_TRACE("Assimp import error: {0}", importer.GetErrorString());

		m_Directory = path.substr(0, path.find_last_of('/'));
		ProcessNode(pScene->mRootNode, pScene);
	}

	void Model::ProcessNode(aiNode* pNode, const aiScene* pScene)
	{
		for (UINT i = 0; i < pNode->mNumMeshes; i++)
		{
			aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[i]];
			m_Meshes.push_back(ProcessMesh(pMesh, pScene));
		}
		for (UINT i = 0; i < pNode->mNumChildren; i++)
			ProcessNode(pNode->mChildren[i], pScene);
	}

	Mesh Model::ProcessMesh(aiMesh* pMesh, const aiScene* pScene)
	{
		using namespace DirectX;
		std::vector<Vertex> verticies;
		std::vector<UINT> indices;
		//std::vector<Texture> textures;

		// Load Verticies
		for (UINT i = 0; i < pMesh->mNumVertices; i++)
		{
			Vertex vertex;
			XMFLOAT3 varContainer;
			// Position
			varContainer.x = pMesh->mVertices[i].x;
			varContainer.y = pMesh->mVertices[i].y;
			varContainer.z = pMesh->mVertices[i].z;
			vertex.Position = varContainer;
			// Normals
			varContainer.x = pMesh->mNormals[i].x;
			varContainer.y = pMesh->mNormals[i].y;
			varContainer.z = pMesh->mNormals[i].z;
			vertex.Normal = varContainer;
			// Texture Coords/Tangents
			if (pMesh->mTextureCoords[0])
			{
				XMFLOAT2 uvs;
				uvs.x = pMesh->mTextureCoords[0][i].x;
				uvs.y = pMesh->mTextureCoords[0][i].y;
				vertex.TexCoords = uvs;
				/*varContainer.x = pMesh->mTangents[i].x;
				varContainer.y = pMesh->mTangents[i].y;
				varContainer.z = pMesh->mTangents[i].z;
				vertex.Tangent = varContainer;*/
			}
			else
			{
				vertex.TexCoords = XMFLOAT2(0.0f, 0.0f);
			}

			verticies.push_back(vertex);
		}

		// Load Indices
		for (UINT i = 0; i < pMesh->mNumFaces; i++)
		{
			aiFace face = pMesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		// Load Textures
		/*if (pMesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = pScene->mMaterials[pMesh->mMaterialIndex];
			std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		}*/
		return Mesh(verticies, indices);

	}

}
