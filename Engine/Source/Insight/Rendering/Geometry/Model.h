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
		bool Init(const std::string& path);

		Mesh& GetMeshAtIndex(const int& index) { return m_Meshes[index]; }
		const size_t& GetNumChildMeshes() const { return m_Meshes.size(); }

		void Draw();
		void Destroy();
	private:
		bool LoadModelFromFile(const std::string& path);
		void ProcessNode_r(aiNode* pNode, const aiScene* pScene);
		Mesh ProcessMesh(aiMesh* pMesh, const aiScene* pScene);
		//std::vector<Texture> LoadMaterialTextures
	private:
		std::vector<Mesh> m_Meshes;
		//std::vector<Texture> m_Textures
		std::string m_Directory;
		std::string m_FileName;
		//Transform m_Transform;
	};

}

