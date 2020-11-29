#pragma once

#include "Mesh.h"


#include "Insight/Core/Scene/Scene_Node.h"
#include "Insight/Rendering/Geometry/Mesh_Node.h"


#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace Insight {

	class Material;
	

	class INSIGHT_API Model : public SceneNode
	{
	public:
		Model(const std::string& Path, Material* Material);
		Model() {}
		Model(Model&& Model) noexcept;
		~Model();

		bool Create(const std::string& path, Material* pMaterial);
		void OnImGuiRender();
		void RenderSceneHeirarchy();
		void BindResources(bool IsDeferredPass);

		ieTransform& GetMeshRootTransformRef() { return m_pRoot->GetTransformRef(); }

		Material& GetMaterialRef() { return *m_pMaterial; }
		std::string GetDirectory() { return m_Directory; }
		std::string GetAssetDirectoryRelativePath() { return m_AssetDirectoryRelativePath; }

		// Visibility
		bool GetCanBeRendered() { return m_Visible; }
		bool SetCanBeRendered(bool Enabled) { m_Visible = Enabled; }
		bool GetCanCastShadows() { return m_CastsShadows; }
		bool SetCanCastShadows(bool Enabled) { m_CastsShadows = Enabled; }

		unique_ptr<Mesh>& GetMeshAtIndex(int index) { return m_Meshes[index]; }
		const size_t GetNumChildMeshes() const { return m_Meshes.size(); }

		void CalculateParent(const ieMatrix4x4& parentMat);
		void Render();
		void Destroy();

	private:
		bool LoadModelFromFile(const std::string& path);
		
		unique_ptr<MeshNode> ParseNode_r(aiNode* pNode);
		unique_ptr<Mesh> ProcessMesh(aiMesh* pMesh, const aiScene* pScene);


	private:
		std::vector<unique_ptr<Mesh>> m_Meshes;
		unique_ptr<MeshNode> m_pRoot;
		
		Material* m_pMaterial = nullptr;

		std::string m_AssetDirectoryRelativePath;
		std::string m_Directory;
		std::string m_FileName;

		bool m_CastsShadows = true;
		bool m_Visible = true;
	};

}

