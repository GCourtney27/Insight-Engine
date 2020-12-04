#include <Engine_pch.h>

#include "Model.h"
#include "Insight/Utilities/String_Helper.h"
#include "Insight/Rendering/Material.h"

#include "Insight/UI/UI_Lib.h"
#include "DirectX12/TK/Inc/Model.h"

namespace Insight {

	Model::Model(const std::string& Path, Material* Material)
	{
		Create(Path, Material);
	}

	Model::Model(Model&& model) noexcept
	{
		IE_DEBUG_LOG(LogSeverity::Warning, "Model being moved in memory.");

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
		m_Directory = StringHelper::WideToString(FileSystem::GetRelativeContentDirectoryW(StringHelper::StringToWide(path)));
		m_FileName = StringHelper::GetFilenameFromDirectory(m_Directory);
		SceneNode::SetDisplayName("Static Mesh");

		return LoadModelFromFile(m_Directory);
	}

	void Model::OnImGuiRender()
	{
		UI::Text("Asset: ");
		UI::SameLine();
		UI::Text(m_FileName.c_str());
		UI::PushID(m_FileName.c_str());

		UI::Text("Transform");
		UI::DrawVector3Control("Position", m_pRoot->GetTransformRef().GetPositionRef());
		UI::DrawVector3Control("Rotation", m_pRoot->GetTransformRef().GetRotationRef());
		UI::DrawVector3Control("Scale", m_pRoot->GetTransformRef().GetScaleRef(), 1.0f);

		UI::Text("Rendering");
		UI::Checkbox("Casts Shadows ##StaticMesh", &m_CastsShadows);
		UI::Checkbox("Visible ##StaticMesh", &m_Visible);

		UI::PopID();
	}

	void Model::RenderSceneHeirarchy()
	{
		if (UI::TreeNode(SceneNode::GetDisplayName())) {
			SceneNode::RenderSceneHeirarchy();
			m_pRoot->RenderSceneHeirarchy();

			UI::TreePop();
			UI::Spacing();
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
#if defined (IE_PLATFORM_BUILD_WIN32)
		Assimp::Importer Importer;
		const aiScene* pScene = Importer.ReadFile(
			path, 
			aiProcess_ImproveCacheLocality | aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded
		);

		if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) {
			IE_DEBUG_LOG(LogSeverity::Error, "Assimp import error: {0}", Importer.GetErrorString());
			return false;
		}

		for (size_t i = 0; i < pScene->mNumMeshes; ++i) {
			m_Meshes.push_back(std::move(ProcessMesh(pScene->mMeshes[i], pScene)));
		}

		m_pRoot = ParseNode_r(pScene->mRootNode);
#elif defined (IE_PLATFORM_BUILD_UWP)

		FILE* fp = fopen(path.c_str(), "rb");
		HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
		
		if (!fp) return false;

		fseek(fp, 0, SEEK_END);
		long file_size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		auto* content = new ofbx::u8[file_size];
		fread(content, 1, file_size, fp);
		ofbx::IScene* pScene = ofbx::load((ofbx::u8*)content, file_size, (ofbx::u64)ofbx::LoadFlags::TRIANGULATE);
		if (!pScene)
		{
			IE_DEBUG_LOG(LogSeverity::Warning, "ofbx import error: {0}", ofbx::getError());
		}
		else 
		{

			int obj_idx = 0;
			int indices_offset = 0;
			int normals_offset = 0;
			int mesh_count = pScene->getMeshCount();
			for (int i = 0; i < mesh_count; ++i)
			{
				const ofbx::Mesh& mesh = *(pScene->getMesh(i));
				const ofbx::Geometry& geom = *(mesh.getGeometry());

				int vertex_count = geom.getVertexCount();
				const ofbx::Vec3* vertices = geom.getVertices();
				std::vector<Vertex3D> Verticies; Verticies.reserve(vertex_count);
				std::vector<DWORD> Indices;

				for (int i = 0; i < vertex_count; ++i)
				{
					Vertex3D Vert;

					Vert.Position.x = vertices[i].x;
					Vert.Position.y = vertices[i].y;
					Vert.Position.z = vertices[i].z;

					bool has_normals = geom.getNormals() != nullptr;
					if (has_normals)
					{
						const ofbx::Vec3* normals = geom.getNormals();

						Vert.Normal.x = normals[i].x;
						Vert.Normal.y = normals[i].y;
						Vert.Normal.z = normals[i].z;
					}

					bool has_uvs = geom.getUVs() != nullptr;
					if (has_uvs)
					{
						const ofbx::Vec2* uvs = geom.getUVs();
						int count = geom.getIndexCount();

						Vert.TexCoords.x = uvs[i].x;
						Vert.TexCoords.y = uvs[i].y;

						const ofbx::Vec3* tans = geom.getTangents();
						Vert.Tangent.x = tans[i].x;
						Vert.Tangent.y = tans[i].y;
						Vert.Tangent.z = tans[i].z;
						
						ieVector3 TempTangent(tans[i].x, tans[i].y, tans[i].z);
						ieVector3 Normal(Vert.Normal.x, Vert.Normal.y, Vert.Normal.z);
						ieVector3 BiTangent = TempTangent.Cross(Normal);
						Vert.BiTangent.x = BiTangent.x;
						Vert.BiTangent.y = BiTangent.y;
						Vert.BiTangent.z = BiTangent.z;
					}

					Verticies.push_back(Vert);
				}

				const int* faceIndices = geom.getFaceIndices();
				int index_count = geom.getIndexCount();
				for (int i = 0; i < index_count; ++i)
				{
					int idx = (faceIndices[i] < 0) ? -faceIndices[i] : (faceIndices[i] + 1);
					int vertex_idx = indices_offset + idx;
					Indices.push_back(vertex_idx);
				}

				m_Meshes.push_back(std::make_unique<Mesh>(Verticies, Indices));

				//indices_offset += vertex_count;
				//normals_offset += index_count;
				//++obj_idx;
			}

			ieTransform transform;
			XMMATRIX mat = XMMatrixIdentity();
			transform.SetWorldMatrix(mat);
			std::vector<Mesh*> curMeshPtrs;
			curMeshPtrs.push_back(m_Meshes[0].get());
			
			m_pRoot = std::make_unique<MeshNode>(curMeshPtrs, transform, "My Model");
		}
		fclose(fp);
#endif
		return true;
	}

#if defined (IE_PLATFORM_BUILD_WIN32)

	unique_ptr<MeshNode> Model::ParseNode_r(::aiNode* pNode)
	{
		ieTransform transform;
		if (pNode->mParent) {
			XMMATRIX mat = XMMatrixMultiply(XMMATRIX(&pNode->mTransformation.a1), XMMATRIX(&pNode->mParent->mTransformation.a1));
			transform.SetWorldMatrix(mat);
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

	unique_ptr<Mesh> Model::ProcessMesh(::aiMesh* pMesh, const ::aiScene* pScene)
	{
		std::vector<Vertex3D> Verticies; Verticies.reserve(pMesh->mNumVertices);
		std::vector<DWORD> Indices;
		
		// Load Verticies
		for (uint32_t i = 0; i < pMesh->mNumVertices; i++) 
		{
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
			if (pMesh->mTextureCoords[0]) 
			{
				Vertex.TexCoords.x = (float)pMesh->mTextureCoords[0][i].x;
				Vertex.TexCoords.y = (float)pMesh->mTextureCoords[0][i].y;

				Vertex.Tangent.x = pMesh->mTangents[i].x;
				Vertex.Tangent.y = pMesh->mTangents[i].y;
				Vertex.Tangent.z = pMesh->mTangents[i].z;

				Vertex.BiTangent.x = pMesh->mBitangents[i].x;
				Vertex.BiTangent.y = pMesh->mBitangents[i].y;
				Vertex.BiTangent.z = pMesh->mBitangents[i].z;
			} 
			else 
			{
				Vertex.TexCoords	= ieFloat2(0.0f, 0.0f);
				Vertex.Tangent		= ieFloat3(0.0f, 0.0f, 0.0f);
				Vertex.BiTangent	= ieFloat3(0.0f, 0.0f, 0.0f);
			}

			Verticies.push_back(Vertex);
		}

		// Load Indices
		for (uint32_t i = 0; i < pMesh->mNumFaces; i++) 
		{
			aiFace Face = pMesh->mFaces[i];

			for (uint32_t j = 0; j < Face.mNumIndices; j++) 
			{
				Indices.push_back(Face.mIndices[j]);
			}
		}

		return std::make_unique<Mesh>(Verticies, Indices);
	}
#endif
}
