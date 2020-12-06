#include <Engine_pch.h>

#include "Model.h"
#include "Insight/Utilities/String_Helper.h"
#include "Insight/Rendering/Material.h"

#include "Insight/UI/UI_Lib.h"

#if defined (IE_PLATFORM_BUILD_UWP)
#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>
#endif
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
			m_Meshes.push_back(
				std::move(AssimpProcessMesh(pScene->mMeshes[i], pScene))
			);
		}

		m_pRoot = AssimpParseNode_r(pScene->mRootNode);

#elif defined (IE_PLATFORM_BUILD_UWP)

		std::string FileExtension = StringHelper::GetFileExtension(path);
		if (FileExtension == "FBX" || FileExtension == "fbx")
		{
			size_t FileSize;
			char* FileContents = FileSystem::ReadRawData(path.c_str(), FileSize);
			if (!FileContents) 
				return false;
			ofbx::IScene* pScene = ofbx::load(
				(const ofbx::u8*)FileContents, 
				(int)FileSize, 
				(ofbx::u64)ofbx::LoadFlags::TRIANGULATE
			);
			const ofbx::GlobalSettings* s = pScene->getGlobalSettings();
			if (!pScene)
			{
				IE_DEBUG_LOG(LogSeverity::Warning, "ofbx import error: {0}", ofbx::getError());
				return false;
			}
			else
			{
				int MeshCount = pScene->getMeshCount();
				for (int i = 0; i < MeshCount; ++i)
				{
					m_Meshes.push_back(
						std::move(OFBXProcessMesh(*(pScene->getMesh(i))))
					);
				}

				//m_pRoot = OFBXParseNode_r(pScene->getRoot());
				ieTransform Transform;
				Transform.SetWorldMatrix(XMMATRIX((float*)&pScene->getRoot()->getGlobalTransform().m[0]));
				std::vector<Mesh*> CurMeshPtrs;
				CurMeshPtrs.push_back(m_Meshes[0].get());
				m_pRoot = std::make_unique<MeshNode>(CurMeshPtrs, Transform, pScene->getRoot()->name);
			}
			delete[] FileContents;
		}
		else if (FileExtension == "OBJ" || FileExtension == "obj")
		{
			std::string inputfile = "cornell_box.obj";
			tinyobj::attrib_t attrib;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;
			std::filebuf fb;
			fb.open(path.c_str(), std::ios::in);
			std::istream inStream(&fb);
			std::string warn;
			std::string err;
			bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, &inStream, nullptr, true);
			fb.close();
			
			// Loop over shapes
			for (size_t s = 0; s < shapes.size(); s++) 
			{
				const tinyobj::mesh_t& mesh = shapes[s].mesh;

				// Loop over faces(polygon)
				size_t index_offset = 0;
				std::vector<Vertex3D> Verticies; Verticies.reserve(mesh.num_face_vertices.size());
				std::vector<DWORD> Indices;
				for (size_t f = 0; f < mesh.num_face_vertices.size(); f++)
				{
					int NumVerticies = mesh.num_face_vertices[f];

					// Loop over vertices in the face.
					for (size_t v = 0; v < NumVerticies; v++)
					{
						Vertex3D Vertex;
						// access to vertex
						tinyobj::index_t idx = mesh.indices[index_offset + v];
						Vertex.Position.x = attrib.vertices[3 * idx.vertex_index + 0];
						Vertex.Position.y = attrib.vertices[3 * idx.vertex_index + 1];
						Vertex.Position.z = attrib.vertices[3 * idx.vertex_index + 2];
						Vertex.Normal.x = attrib.normals[3 * idx.normal_index + 0];
						Vertex.Normal.y = attrib.normals[3 * idx.normal_index + 1];
						Vertex.Normal.z = attrib.normals[3 * idx.normal_index + 2];
						Vertex.TexCoords.x = attrib.texcoords[2 * idx.texcoord_index + 0];
						Vertex.TexCoords.y = attrib.texcoords[2 * idx.texcoord_index + 1];

						Verticies.push_back(Vertex);
					} // verticies

					index_offset += NumVerticies;
				} // faces

				size_t NumIndices = mesh.indices.size();
				for (size_t i = 0; i < NumIndices; i++)
				{
					Indices.push_back(mesh.indices[i].vertex_index);
				}

				m_Meshes.push_back(std::make_unique<Mesh>(Verticies, Indices));
			} // shapes

			ieTransform transform;
			transform.SetWorldMatrix(XMMatrixIdentity());
			//shapes[0].mesh.tags[0].name;
			std::vector<Mesh*> curMeshPtrs;
			curMeshPtrs.push_back(m_Meshes[0].get());
			m_pRoot = std::make_unique<MeshNode>(curMeshPtrs, transform, "Test");

		}
#endif
		return true;
	}

#if defined (IE_PLATFORM_BUILD_WIN32)

	unique_ptr<MeshNode> Model::AssimpParseNode_r(::aiNode* pNode)
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
			pMeshNode->AddChild(AssimpParseNode_r(pNode->mChildren[i]));
		}

		return pMeshNode;
	}

	unique_ptr<Mesh> Model::AssimpProcessMesh(::aiMesh* pMesh, const ::aiScene* pScene)
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
				Vertex.TexCoords = ieFloat2(0.0f, 0.0f);
				Vertex.Tangent = ieFloat3(0.0f, 0.0f, 0.0f);
				Vertex.BiTangent = ieFloat3(0.0f, 0.0f, 0.0f);
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

#elif defined (IE_PLATFORM_BUILD_UWP)

	std::unique_ptr<Mesh> Model::OFBXProcessMesh(const ofbx::Mesh& FBXMesh)
	{
		const ofbx::Geometry& Geometry = *(FBXMesh.getGeometry());

		const ofbx::Vec3* RawVerticies = Geometry.getVertices();
		int VertexCount = Geometry.getVertexCount();
		std::vector<Vertex3D> Verticies; Verticies.reserve(VertexCount);
		std::vector<DWORD> Indices;

		for (int i = 0; i < VertexCount; ++i)
		{
			Vertex3D Vertex;

			Vertex.Position.x = (float)RawVerticies[i].x;
			Vertex.Position.y = (float)RawVerticies[i].y;
			Vertex.Position.z = (float)RawVerticies[i].z;

			if (Geometry.getNormals() != nullptr)
			{
				const ofbx::Vec3* normals = Geometry.getNormals();

				Vertex.Normal.x = (float)normals[i].x;
				Vertex.Normal.y = (float)normals[i].y;
				Vertex.Normal.z = (float)normals[i].z;
			}

			if (Geometry.getUVs() != nullptr)
			{
				const ofbx::Vec2* uvs = Geometry.getUVs();
				int count = Geometry.getIndexCount();

				Vertex.TexCoords.x = (float)uvs[i].x;
				Vertex.TexCoords.y = (float)uvs[i].y;

				const ofbx::Vec3* tans = Geometry.getTangents();
				if (tans)
				{
					Vertex.Tangent.x = (float)tans[i].x;
					Vertex.Tangent.y = (float)tans[i].y;
					Vertex.Tangent.z = (float)tans[i].z;
				
					ieVector3 TempTangent((float)tans[i].x, (float)tans[i].y, (float)tans[i].z);
					ieVector3 Normal(Vertex.Normal.x, Vertex.Normal.y, Vertex.Normal.z);
					ieVector3 BiTangent = TempTangent.Cross(Normal);
					Vertex.BiTangent.x = BiTangent.x;
					Vertex.BiTangent.y = BiTangent.y;
					Vertex.BiTangent.z = BiTangent.z;
				}
				else
				{
					Vertex.TexCoords = ieFloat2(0.0f, 0.0f);
					Vertex.Tangent = ieFloat3(0.0f, 0.0f, 0.0f);
					Vertex.BiTangent = ieFloat3(0.0f, 0.0f, 0.0f);
				}

			}

			Verticies.push_back(Vertex);
		}

		const int* RawFaceIndicies = Geometry.getFaceIndices();
		int IndexCount = Geometry.getIndexCount();
		for (int i = 0; i < IndexCount; ++i)
		{
			int idx = RawFaceIndicies[i];

			// If the index is negative in fbx that means it is the last index of that polygon.
			// So, make it positive and subtrct one.
			if (idx < 0)
				idx = -(idx + 1);

			Indices.push_back(idx);
		}

		return std::make_unique<Mesh>(Verticies, Indices);
	}

	/*std::unique_ptr<Mesh> Model::TinyOBJProcessMesh()
	{

	}*/

#endif
}
