#include "Model.h"

bool Model::Initialize(const std::string & filepath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_PerObject>& CB_VS_PerObject, Material * material)
{
	this->modelDirectory = filepath;
	this->device = device;
	this->deviceContext = deviceContext;
	this->cb_vs_PerObject = &CB_VS_PerObject; // <--Uncommented for skybox
	this->m_pMaterial = material;

	try
	{
		if (!this->LoadModel(filepath))
			return false;
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

void Model::Draw(const XMMATRIX & worldMatrix, const XMMATRIX & projectionMatrix, const XMMATRIX & viewMatrix)
{

	// THE REASON EVERYTHING NEEDS NULLPTR CHECKS IS BECASUE THEY SKYBOX IN GRAPHICS.CPP FIX IT!!
	if (m_pMaterial != nullptr)
	{
		this->m_pMaterial->PSSetShader();
		this->m_pMaterial->VSSetShader();
		this->m_pMaterial->IASetInputLayout();

		this->deviceContext->VSSetConstantBuffers(0, 1, this->m_pMaterial->m_cb_vs_PerObject.GetAddressOf());
		this->deviceContext->VSSetConstantBuffers(2, 1, this->m_pMaterial->m_cb_vs_PerObjectUtil.GetAddressOf());

		this->deviceContext->PSSetConstantBuffers(2, 1, this->m_pMaterial->m_cb_ps_PerObjectUtil.GetAddressOf());

	}
	else
	{
		// Gets the default vertex shader becassue the material for sky isnt implemented yet
		this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_PerObject->GetAddressOf());
	}

	for (size_t i = 0; i < meshes.size(); i++)
	{
		if (m_pMaterial != nullptr)
		{
			// Update Constant Buffer Matricies
			this->m_pMaterial->m_cb_vs_PerObject.data.worldMatrix = meshes[i].GetTransformMatrix() * worldMatrix; // Calculate World Matrix
			this->m_pMaterial->m_cb_vs_PerObject.data.viewMatrix = viewMatrix; // Calculate World-ViewProjection Matrix
			this->m_pMaterial->m_cb_vs_PerObject.data.projectionMatrix = projectionMatrix;
			this->m_pMaterial->m_cb_vs_PerObject.ApplyChanges();

			m_pMaterial->PSSetShaderResources();
		}
		else
		{
			this->cb_vs_PerObject->data.worldMatrix = meshes[i].GetTransformMatrix() * worldMatrix; // Calculate World Matrix
			this->cb_vs_PerObject->data.viewMatrix = viewMatrix;
			this->cb_vs_PerObject->data.projectionMatrix = projectionMatrix;
			this->cb_vs_PerObject->ApplyChanges();
		}
		
		meshes[i].Draw();
	}
}

bool Model::LoadModel(const std::string & filepath)
{
	this->directory = StringHelper::GetDirectoryFromPath(filepath);
	Assimp::Importer importer; 
	const aiScene* pScene = importer.ReadFile(filepath,
												aiProcess_Triangulate | 
												aiProcess_ConvertToLeftHanded |
												aiProcess_GenNormals |
												aiProcess_CalcTangentSpace);

	if (pScene == nullptr)
		return false;

	this->ProcessNode(pScene->mRootNode, pScene, DirectX::XMMatrixIdentity());
	return true;
}

void Model::ProcessNode(aiNode * node, const aiScene * scene, const XMMATRIX & parentTransformMatrix)
{
	XMMATRIX nodeTransformMatrix = XMMatrixTranspose(XMMATRIX(&node->mTransformation.a1)) * parentTransformMatrix;

	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(this->ProcessMesh(mesh, scene, nodeTransformMatrix));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], scene, nodeTransformMatrix);
	}
}

Mesh Model::ProcessMesh(aiMesh * mesh, const aiScene * scene, const XMMATRIX & transformMatrix)
{
	// Data to fill
	std::vector<Vertex3D> verticies;
	std::vector<DWORD> indicies;

	// Get verticies
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex3D vertex;

		vertex.pos.x = mesh->mVertices[i].x;
		vertex.pos.y = mesh->mVertices[i].y;
		vertex.pos.z = mesh->mVertices[i].z;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		vertex.Tangent.x = mesh->mTangents[i].x;
		vertex.Tangent.y = mesh->mTangents[i].y;
		vertex.Tangent.z = mesh->mTangents[i].z;

		vertex.BiTangent.x = mesh->mBitangents[i].x;
		vertex.BiTangent.y = mesh->mBitangents[i].y;
		vertex.BiTangent.z = mesh->mBitangents[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.texCoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		verticies.push_back(vertex);
	}

	// Get indices
	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indicies.push_back(face.mIndices[j]);
	}


	std::vector<Texture> textures;
	aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];
	std::vector<Texture> diffuseTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, scene);
	textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());

	return Mesh(this->device, this->deviceContext, verticies, indicies, textures, transformMatrix);
}

TextureStorageType Model::DetermineTextureStorageType(const aiScene * pScene, aiMaterial * pMat, unsigned int index, aiTextureType textureType)
{
	if (pMat->GetTextureCount(textureType) == 0)
		return TextureStorageType::None;

	aiString path;
	pMat->GetTexture(textureType, index, &path);
	std::string texturePath = path.C_Str();
	//Check if texture is an embedded indexed texture by seeing if the file path is an index #
	if (texturePath[0] == '*')
	{
		if (pScene->mTextures[0]->mHeight == 0)
		{
			return TextureStorageType::EmbeddedIndexCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR INDEXED NON COMPRESSED TEXTURES!" && 0);
			return TextureStorageType::EmbeddedIndexNonCompressed;
		}
	}
	//Check if texture is an embedded texture but not indexed (path will be the texture's name instead of #)
	if (auto pTex = pScene->GetEmbeddedTexture(texturePath.c_str()))
	{
		if (pTex->mHeight == 0)
		{
			return TextureStorageType::EmbeddedCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR EMBEDDED NON COMPRESSED TEXTURES!" && 0);
			return TextureStorageType::EmbeddedNonCompressed;
		}
	}
	//Lastly check if texture is a filepath by checking for period before extension name
	if (texturePath.find('.') != std::string::npos)
	{
		return TextureStorageType::Disk;
	}

	return TextureStorageType::None; // No texture exists
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial * pMaterial, aiTextureType textureType, const aiScene * pScene)
{
	std::vector<Texture> materialTextures;
	TextureStorageType storetype = TextureStorageType::Invalid;
	unsigned int textureCount = pMaterial->GetTextureCount(textureType);

	if (textureCount == 0) //If there are no textures
	{
		storetype = TextureStorageType::None;
		aiColor3D aiColor(0.0f, 0.0f, 0.0f);
		switch (textureType)
		{
		case aiTextureType_DIFFUSE:
			pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
			if (aiColor.IsBlack()) //If color = black, just use grey
			{
				materialTextures.push_back(Texture(this->device, Colors::UnloadedTextureColor, textureType));
				return materialTextures;
			}
			materialTextures.push_back(Texture(this->device, Color((BYTE)aiColor.r * 255, (BYTE)aiColor.g * 255, (BYTE)aiColor.b * 255), textureType));
			return materialTextures;
		}
	}
	else
	{
		for (UINT i = 0; i < textureCount; i++)
		{
			aiString path;
			pMaterial->GetTexture(textureType, i, &path);
			TextureStorageType storeType = DetermineTextureStorageType(pScene, pMaterial, i, textureType);
			switch (storeType)
			{
			case TextureStorageType::EmbeddedIndexCompressed:
			{
				int index = GetTextureIndex(&path);
				Texture embededIndexedTexture(this->device,
												reinterpret_cast<uint8_t*>(pScene->mTextures[index]->pcData),
												pScene->mTextures[index]->mWidth,
												textureType);
				materialTextures.push_back(embededIndexedTexture);
				break;
			}
			case TextureStorageType::EmbeddedCompressed:
			{
				const aiTexture * pTexture = pScene->GetEmbeddedTexture(path.C_Str());
				Texture embeddedTexture(this->device,
										reinterpret_cast<uint8_t*>(pTexture->pcData),
										pTexture->mWidth,
										textureType);
				materialTextures.push_back(embeddedTexture);
				break;
			}
			case TextureStorageType::Disk:
				{
					std::string filename = this->directory + '\\' + path.C_Str();
					Texture diskTexture(this->device, filename, textureType);
					materialTextures.push_back(diskTexture);
					break;
				}
			}
		}
	}

	if (materialTextures.size() == 0)
	{
		materialTextures.push_back(Texture(this->device, Colors::UnhandledTextureColor, aiTextureType::aiTextureType_DIFFUSE));
	}
	return materialTextures;
}

int Model::GetTextureIndex(aiString * pStr)
{
	assert(pStr->length >= 2);
	return atoi(&pStr->C_Str()[1]);
}
