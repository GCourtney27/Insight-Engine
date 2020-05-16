#pragma once

#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Insight/Core/Scene/Scene_Node.h"
#include "Insight/Rendering/Geometry/Mesh_Node.h"

namespace Insight {

	class INSIGHT_API StaticMesh : public SceneNode
	{
	public:
		StaticMesh(const std::string& path);
		StaticMesh() 
			: SceneNode("Model") {}
		~StaticMesh();
		
		bool Init(const std::string& path);
		void RenderSceneHeirarchy();

		unique_ptr<Mesh>& GetMeshAtIndex(const int& index) { return m_Meshes[index]; }
		const size_t& GetNumChildMeshes() const { return m_Meshes.size(); }

		void Draw();
		void Destroy();
		bool LoadModelFromFile(const std::string& path);// TEMP
	private:
		unique_ptr<MeshNode> ParseNode_r(aiNode* pNode, const aiScene* pScene);
		unique_ptr<Mesh> ProcessMesh(aiMesh* pMesh);
		//std::vector<Texture> LoadMaterialTextures
	private:
		std::vector<unique_ptr<Mesh>> m_Meshes;
		unique_ptr<MeshNode> m_pRoot;

		//std::vector<Texture> m_Textures
		std::string m_Directory;
		std::string m_FileName;
	};

}

