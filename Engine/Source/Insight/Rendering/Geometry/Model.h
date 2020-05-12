#pragma once

#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Insight/Core/Scene/Scene_Node.h"

namespace Insight {

	class INSIGHT_API Model : public SceneNode
	{
	public:
		Model(const std::string& path);
		Model() 
			: SceneNode("Model")
		{}
		~Model();
		
		bool Init(const std::string& path);
		void RenderSceneHeirarchy();

		unique_ptr<Mesh>& GetMeshAtIndex(const int& index) { return m_Meshes[index]; }
		const size_t& GetNumChildMeshes() const { return m_Meshes.size(); }

		void Draw();
		void Destroy();
	private:
		bool LoadModelFromFile(const std::string& path);
		void ParseNode_r(aiNode* pNode, const aiScene* pScene);
		unique_ptr<Mesh> ProcessMesh(aiMesh* pMesh);
		//std::vector<Texture> LoadMaterialTextures
	private:
		std::vector<unique_ptr<Mesh>> m_Meshes;
		//std::vector<Texture> m_Textures
		std::string m_Directory;
		std::string m_FileName;
	};

}

