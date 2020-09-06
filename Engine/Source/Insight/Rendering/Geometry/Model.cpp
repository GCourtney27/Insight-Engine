#include <ie_pch.h>

#include "Model.h"
#include "Insight/Utilities/String_Helper.h"
#include "Insight/Systems/File_System.h"
#include "Insight/Rendering/Material.h"

#include "imgui.h"

namespace Insight {

	Model::Model(const std::string& Path, Material* Material)
	{
		Create(Path, Material);
	}

	Model::Model(Model&& model) noexcept
	{
		IE_CORE_WARN("Model being moved in memory.");

		m_Meshes = std::move(model.m_Meshes);
		m_pRoot = std::move(model.m_pRoot);

		m_pMaterial = std::move(model.m_pMaterial);
		m_AssetDirectoryRelativePath = std::move(model.m_AssetDirectoryRelativePath);
		m_Directory = std::move(model.m_Directory);
		m_FileName = std::move(model.m_FileName);

		model.m_pRoot = nullptr;
		model.m_pMaterial = nullptr;
	}

	Model::~Model()
	{
		Destroy();
	}

	void Model::Destroy()
	{
		for (UINT i = 0; i < m_Meshes.size(); i++) {
			m_Meshes[i]->Destroy();
		}
	}

	bool Model::Create(const std::string& path, Material* pMaterial)
	{
		m_pMaterial = pMaterial;

		m_AssetDirectoryRelativePath = path;
		m_Directory = FileSystem::GetProjectRelativeAssetDirectory(path);
		m_FileName = StringHelper::GetFilenameFromDirectory(m_Directory);
		SceneNode::SetDisplayName("Static Mesh");

		return LoadModelFromFile(m_Directory);
	}

	void Model::OnImGuiRender()
	{
		ImGui::Text("Asset: ");
		ImGui::SameLine();
		ImGui::Text(m_FileName.c_str());

		ImGui::Text("Transform - StaticMesh");
		ImGui::DragFloat3("Position##StaticMesh", &m_pRoot->GetTransformRef().GetPositionRef().x, 0.05f, -1000.0f, 1000.0f);
		ImGui::DragFloat3("Scale##StaticMesh", &m_pRoot->GetTransformRef().GetScaleRef().x, 0.05f, -1000.0f, 1000.0f);
		ImGui::DragFloat3("Rotation##StaticMesh", &m_pRoot->GetTransformRef().GetRotationRef().x, 0.05f, -1000.0f, 1000.0f);

		ImGui::Text("Rendering");
		ImGui::Checkbox("Casts Shadows ##StaticMesh", &m_CastsShadows);
		ImGui::Checkbox("Visible ##StaticMesh", &m_Visible);
	}

	void Model::RenderSceneHeirarchy()
	{
		if (ImGui::TreeNode(SceneNode::GetDisplayName())) {
			SceneNode::RenderSceneHeirarchy();
			m_pRoot->RenderSceneHeirarchy();

			ImGui::TreePop();
			ImGui::Spacing();
		}
	}

	void Model::BindResources(bool IsDeferredPass)
	{
		m_pMaterial->BindResources(IsDeferredPass);
	}

	void Model::CalculateParent(const ieMatrix4x4& parentMat)
	{
		auto worldMat = XMMatrixMultiply(m_pRoot->GetTransformRef().GetLocalMatrixRef(), parentMat);
		for (unique_ptr<Mesh>& mesh : m_Meshes) {
			mesh->PreRender(worldMat);
		}
	}

	void Model::Render()
	{
		int numMeshChildren = (int)m_Meshes.size();
		for (int i = 0; i < numMeshChildren; ++i) {
			m_Meshes[i]->Render();
		}
	}

	bool Model::LoadModelFromFile(const std::string& path)
	{
		Assimp::Importer Importer;
		const aiScene* pScene = Importer.ReadFile(
			path, 
			aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_ConvertToLeftHanded 
		);

		if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) {
			IE_CORE_ERROR("Assimp import error: {0}", Importer.GetErrorString());
			return false;
		}

		for (size_t i = 0; i < pScene->mNumMeshes; ++i) {
			m_Meshes.push_back(std::move(ProcessMesh(pScene->mMeshes[i], pScene)));
		}

		m_pRoot = ParseNode_r(pScene->mRootNode);
		return true;
	}

	unique_ptr<MeshNode> Model::ParseNode_r(aiNode* pNode)
	{
		ieTransform transform;
		if (pNode->mParent) {
			transform.SetLocalMatrix(XMMatrixTranspose(XMMATRIX(&pNode->mTransformation.a1)));
		}

		// Create a pointer to all the meshes this node owns
		std::vector<Mesh*> curMeshPtrs;
		curMeshPtrs.reserve(pNode->mNumMeshes);
		for (UINT i = 0; i < pNode->mNumMeshes; i++) {
			const auto meshIndex = pNode->mMeshes[i];
			curMeshPtrs.push_back(m_Meshes.at(meshIndex).get());
		}
		
		auto pMeshNode = std::make_unique<MeshNode>(curMeshPtrs, transform, pNode->mName.C_Str());
		for (UINT i = 0; i < pNode->mNumChildren; ++i) {
			pMeshNode->AddChild(ParseNode_r(pNode->mChildren[i]));
		}

		return pMeshNode;
	}

	unique_ptr<Mesh> Model::ProcessMesh(aiMesh* pMesh, const aiScene* pScene)
	{
		std::vector<Vertex3D> Verticies; Verticies.reserve(pMesh->mNumVertices);
		std::vector<DWORD> Indices;
		
		// Load Verticies
		for (uint32_t i = 0; i < pMesh->mNumVertices; i++) {

			Vertex3D Vertex;

			// Position
			Vertex.Position.x = pMesh->mVertices[i].x;
			Vertex.Position.y = pMesh->mVertices[i].y;
			Vertex.Position.z = pMesh->mVertices[i].z;

			// Normals
			Vertex.Normal.x = (float)pMesh->mNormals[i].x;
			Vertex.Normal.y = (float)pMesh->mNormals[i].y;
			Vertex.Normal.z = (float)pMesh->mNormals[i].z;


			// Texture Coords/Tangents
			if (pMesh->mTextureCoords[0]) {

				Vertex.TexCoords.x = (float)pMesh->mTextureCoords[0][i].x;
				Vertex.TexCoords.y = (float)pMesh->mTextureCoords[0][i].y;

				Vertex.Tangent.x = pMesh->mTangents[i].x;
				Vertex.Tangent.y = pMesh->mTangents[i].y;
				Vertex.Tangent.z = pMesh->mTangents[i].z;

				Vertex.BiTangent.x = pMesh->mBitangents[i].x;
				Vertex.BiTangent.y = pMesh->mBitangents[i].y;
				Vertex.BiTangent.z = pMesh->mBitangents[i].z;
				
			} else {

				Vertex.TexCoords	= ieFloat2(0.0f, 0.0f);
				Vertex.Tangent		= ieFloat3(0.0f, 0.0f, 0.0f);
				Vertex.BiTangent	= ieFloat3(0.0f, 0.0f, 0.0f);
			}

			Verticies.push_back(Vertex);
		}

		// Load Indices
		for (uint32_t i = 0; i < pMesh->mNumFaces; i++) {

			aiFace face = pMesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {

				Indices.push_back(face.mIndices[j]);
			}
		}

		return std::make_unique<Mesh>(Verticies, Indices);
	}

}
