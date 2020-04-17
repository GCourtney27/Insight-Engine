#pragma once

#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <vector>

namespace Insight {

	class Model
	{
	public:
		Model(const std::string& path);
		Model() {}
		~Model();
		void Init(const std::string path);

		void Draw();
		void Destroy();
	private:
		void LoadModel(const std::string& path);
		void ProcessNode(aiNode* pNode, const aiScene* pScene);
		Mesh ProcessMesh(aiMesh* pMesh, const aiScene* pScene);
		//std::vector<Texture> LoadMaterialTextures
	private:
		std::vector<Mesh> m_Meshes;
		//std::vector<Texture> m_Textures
		std::string m_Directory;
	};

}

