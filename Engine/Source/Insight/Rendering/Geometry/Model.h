#pragma once

#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Insight/Core/Scene/Scene_Node.h"
#include "Insight/Rendering/Geometry/Mesh_Node.h"

namespace Insight {

	class INSIGHT_API Model : public SceneNode
	{
	public:
		typedef std::stack<XMMATRIX> InstanceMatrixStack;
	public:
		Model(const std::string& path);
		Model() {}
		~Model();
		
		bool Init(const std::string& path);
		void RenderSceneHeirarchy();

		void PushInstanceWorldMatrix(XMMATRIX& instanceMat) { m_InstanceMatrixStack.push(instanceMat); }
		InstanceMatrixStack* GetInstanceMatrixStack() { return &m_InstanceMatrixStack; }

		unique_ptr<Mesh>& GetMeshAtIndex(const int& index) { return m_Meshes[index]; }
		const size_t& GetNumChildMeshes() const { return m_Meshes.size(); }

		void PreRender(XMMATRIX& parentMat);
		void Render();
		void Destroy();
		bool LoadModelFromFile(const std::string& path);// TEMP
	private:
		void ParseNode_r(aiNode* pNode, const aiScene* pScene);
		unique_ptr<Mesh> ProcessMesh(aiMesh* pMesh);
		//std::vector<Texture> LoadMaterialTextures
	private:
		std::vector<unique_ptr<Mesh>> m_Meshes;
		unique_ptr<MeshNode> m_pRoot;
		InstanceMatrixStack m_InstanceMatrixStack;

		ID3D12Resource* m_ConstantBufferUploadHeaps = nullptr;
		ID3D12GraphicsCommandList* m_pCommandList = nullptr;
		int ConstantBufferPerObjectAlignedSize = (sizeof(CB_VS_PerObject) + 255) & ~255;

		int m_ManagerIndex;
		//std::vector<Texture> m_Textures
		std::string m_Directory;
		std::string m_FileName;
	};

}

