#include <ie_pch.h>

#include "Model.h"
#include "Insight/Utilities/String_Helper.h"
#include "imgui.h"

namespace Insight {


	Model::Model(const std::string& path)
	{
		Init(path);
	}

	Model::~Model()
	{
		//Destroy();
	}

	bool Model::Init(const std::string& path)
	{
		m_Directory = StringHelper::GetDirectoryFromPath(path);
		m_FileName = StringHelper::GetFilenameFromDirectory(path);
		SceneNode::SetDisplayName("Static Mesh");

		return LoadModelFromFile(path);
	}

	void Model::RenderSceneHeirarchy()
	{
		if (ImGui::TreeNode(Super::GetDisplayName())) {
			Super::RenderSceneHeirarchy();
			m_pRoot->RenderSceneHeirarchy();

			ImGui::TreePop();
			ImGui::Spacing();
		}
	}

	void Model::PreRender(XMMATRIX& parentMat)
	{
		// TODO Set the world view and projection matrix of each mesh in the constant buffer
		auto worldMat = m_pRoot->GetTransform().GetLocalMatrix() * parentMat;
		for (unique_ptr<Mesh>& mesh : m_Meshes)
		{
			mesh->PreRender(worldMat);
		}
	}

	void Model::Render()
	{
		int numMeshChildren = m_Meshes.size();
		for (int i = 0; i < numMeshChildren; ++i) {
			m_Meshes[i]->Render();
		}
	}

	void Model::Destroy()
	{
		for (UINT i = 0; i < m_Meshes.size(); i++) {
			m_Meshes[i]->Destroy();
		}
	}

	bool Model::LoadModelFromFile(const std::string& path)
	{
		Assimp::Importer importer;
		const aiScene* pScene = importer.ReadFile(
			path, 
			aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_ConvertToLeftHanded 
		);

		if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) {
			IE_CORE_TRACE("Assimp import error: {0}", importer.GetErrorString());
			return false;
		}

		for (size_t i = 0; i < pScene->mNumMeshes; ++i) {
			m_Meshes.push_back(std::move(ProcessMesh(pScene->mMeshes[i])));
		}
		m_pRoot = ParseNode_r(pScene->mRootNode, pScene);
		return true;
	}

	unique_ptr<MeshNode> Model::ParseNode_r(aiNode* pNode, const aiScene* pScene)
	{
		Transform transform;
		transform.SetLocalMatrix((DirectX::XMLoadFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&pNode->mTransformation))));

		// Create a pointer to all the meshes this node owns
		std::vector<Mesh*> curMeshPtrs;
		curMeshPtrs.reserve(pNode->mNumMeshes);
		for (UINT i = 0; i < pNode->mNumMeshes; i++) {
			const auto meshIndex = pNode->mMeshes[i];
			curMeshPtrs.push_back(m_Meshes.at(meshIndex).get());
		}
		
		auto pMeshNode = std::make_unique<MeshNode>(curMeshPtrs, transform, pNode->mName.C_Str());
		for (UINT i = 0; i < pNode->mNumChildren; ++i) {
			pMeshNode->AddChild(ParseNode_r(pNode->mChildren[i], pScene));
		}

		return pMeshNode;
	}

	unique_ptr<Mesh> Model::ProcessMesh(aiMesh* pMesh)
	{
		using namespace DirectX;
		std::vector<Vertex> verticies; //verticies.reserve(pMesh->mNumVertices);
		std::vector<DWORD> indices;
		
		// Load Verticies
		for (UINT i = 0; i < pMesh->mNumVertices; i++) {

			Vertex vertex;
			// Position
			vertex.Position.x = pMesh->mVertices[i].x;
			vertex.Position.y = pMesh->mVertices[i].y;
			vertex.Position.z = pMesh->mVertices[i].z;

			// Texture Coords/Tangents
			if (pMesh->mTextureCoords[0]) {

				vertex.TexCoords.x = (float)pMesh->mTextureCoords[0][i].x;
				vertex.TexCoords.y = (float)pMesh->mTextureCoords[0][i].y;

				vertex.Tangent.x = pMesh->mTangents[i].x;
				vertex.Tangent.y = pMesh->mTangents[i].y;
				vertex.Tangent.z = pMesh->mTangents[i].z;
				
				vertex.BiTangent.x = pMesh->mBitangents[i].x;
				vertex.BiTangent.y = pMesh->mBitangents[i].y;
				vertex.BiTangent.z = pMesh->mBitangents[i].z;
			} else {

				vertex.TexCoords = XMFLOAT2(0.0f, 0.0f);
				vertex.Tangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
				vertex.BiTangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
			}
			// Normals
			vertex.Normal.x = (float)pMesh->mNormals[i].x;
			vertex.Normal.y = (float)pMesh->mNormals[i].y;
			vertex.Normal.z = (float)pMesh->mNormals[i].z;

			verticies.push_back(vertex);
		}

		// Load Indices
		for (UINT i = 0; i < pMesh->mNumFaces; i++) {

			aiFace face = pMesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {

				indices.push_back(face.mIndices[j]);
			}
		}

		return std::make_unique<Mesh>(verticies, indices);
	}

}
