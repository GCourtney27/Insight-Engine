#pragma once

#include "Mesh.h"

#include "Core/Public/Scene/SceneNode.h"
#include "Graphics/Geometry/MeshNode.h"

#if IE_WIN32
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#elif IE_UWP_DESKTOP
#include "ofbx.h"
//#define TINYOBJLOADER_IMPLEMENTATION
//#include <tinyobjloader/tiny_obj_loader.h>
#endif

namespace Insight {

	class Material;
	

	class INSIGHT_API Model : public SceneNode
	{
	public:
		Model(const FString& Path, Material* Material);
		Model() {}
		Model(Model&& Model) noexcept;
		~Model();

		bool Create(const FString& path, Material* pMaterial);
		void OnImGuiRender();
		void RenderSceneHeirarchy();
		void BindResources(bool IsDeferredPass);

		ieTransform& GetMeshRootTransformRef() { return m_pRoot->GetTransformRef(); }

		Material& GetMaterialRef() { return *m_pMaterial; }
		FString GetDirectory() { return m_Directory; }
		FString GetAssetDirectoryRelativePath() { return m_AssetDirectoryRelativePath; }

		// Visibility
		bool GetCanBeRendered() { return m_Visible; }
		bool SetCanBeRendered(bool Enabled) { m_Visible = Enabled; }
		bool GetCanCastShadows() { return m_CastsShadows; }
		bool SetCanCastShadows(bool Enabled) { m_CastsShadows = Enabled; }

		std::unique_ptr<Mesh>& GetMeshAtIndex(int index) { return m_Meshes[index]; }
		const size_t GetNumChildMeshes() const { return m_Meshes.size(); }

		void CalculateParent(const FMatrix& parentMat);
		void Render();
		void Destroy();

	private:
		bool LoadModelFromFile(const FString& path);
		
#if IE_WIN32
		std::unique_ptr<MeshNode> AssimpParseNode_r(aiNode* pNode);
		std::unique_ptr<Mesh> AssimpProcessMesh(aiMesh* pMesh, const aiScene* pScene);
#elif IE_UWP_DESKTOP
		std::unique_ptr<Mesh> OFBXProcessMesh(const ofbx::Mesh& FBXMesh);
		//std::unique_ptr<Mesh> TinyOBJProcessMesh();
#endif

	private:
		std::vector<std::unique_ptr<Mesh>> m_Meshes;
		std::unique_ptr<MeshNode> m_pRoot;
		
		Material* m_pMaterial = nullptr;

		FString m_AssetDirectoryRelativePath;
		FString m_Directory;
		FString m_FileName;

		bool m_CastsShadows = true;
		bool m_Visible = true;
	};

}

