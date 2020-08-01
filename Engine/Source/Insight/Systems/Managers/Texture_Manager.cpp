#include <ie_pch.h>

#include "Texture_Manager.h"
#include "Insight/Systems/File_System.h"
#include "Insight/Utilities/String_Helper.h"
#include "Insight/Rendering/Renderer.h"

#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"
#include "Platform/Windows/DirectX_12/ie_D3D12_Texture.h"
#include "Platform/Windows/DirectX_11/ie_D3D11_Texture.h"

namespace Insight {

	TextureManager::TextureManager()
	{
	}

	TextureManager::~TextureManager()
	{
		Destroy();
	}

	void TextureManager::Destroy()
	{
	}

	void TextureManager::FlushTextureCache()
	{
		for (StrongTexturePtr& tex : m_AlbedoTextures) {
			tex.reset();
		}
		for (StrongTexturePtr& tex : m_NormalTextures) {
			tex.reset();
		}
		for (StrongTexturePtr& tex : m_MetallicTextures) {
			tex.reset();
		}
		for (StrongTexturePtr& tex : m_RoughnessTextures) {
			tex.reset();
		}
		for (StrongTexturePtr& tex : m_AOTextures) {
			tex.reset();
		}
		for (StrongTexturePtr& tex : m_OpacityTextures) {
			tex.reset();
		}
		for (StrongTexturePtr& tex : m_TranslucencyTextures) {
			tex.reset();
		}

	}

	bool TextureManager::Init()
	{
		LoadDefaultTextures();
		return true;
	}

	bool TextureManager::LoadResourcesFromJson(const rapidjson::Value& JsonTextures)
	{
		for (rapidjson::SizeType i = 0; i < JsonTextures.Size(); i++) {
			std::string Name, Filepath;
			int Type, ID;
			bool GenMipMaps;
			json::get_int(JsonTextures[i], "ID", ID);
			json::get_int(JsonTextures[i], "Type", Type);
			json::get_string(JsonTextures[i], "Name", Name);
			json::get_string(JsonTextures[i], "Filepath", Filepath);
			json::get_bool(JsonTextures[i], "GenerateMipMaps", GenMipMaps);

			Texture::IE_TEXTURE_INFO TexInfo = {};
			TexInfo.DisplayName = Name;
			TexInfo.Id = ID;
			TexInfo.Filepath = StringHelper::StringToWide(FileSystem::GetProjectRelativeAssetDirectory(Filepath));
			TexInfo.GenerateMipMaps = GenMipMaps;
			TexInfo.Type = (Texture::eTextureType)Type;

			RegisterTextureByType(TexInfo);

			m_HighestTextureId = ((int)m_HighestTextureId < ID) ? ID : m_HighestTextureId;
		}

		return true;
	}

	StrongTexturePtr TextureManager::GetTextureByID(Texture::ID textureID, Texture::eTextureType textreType)
	{
		switch (textreType) {
		case Texture::eTextureType::eTextureType_Albedo:
		{
			for (UINT i = 0; i < m_AlbedoTextures.size(); i++) {
				if (textureID == m_AlbedoTextures[i]->GetTextureInfo().Id) {
					return m_AlbedoTextures[i];
				}
			}
			break;
		}
		case Texture::eTextureType::eTextureType_Normal:
		{
			for (UINT i = 0; i < m_NormalTextures.size(); i++) {

				if (textureID == m_NormalTextures[i]->GetTextureInfo().Id) {
					return m_NormalTextures[i];
				}
			}
			break;
		}
		case Texture::eTextureType::eTextureType_Roughness:
		{
			for (UINT i = 0; i < m_RoughnessTextures.size(); i++) {

				if (textureID == m_RoughnessTextures[i]->GetTextureInfo().Id) {
					return m_RoughnessTextures[i];
				}
			}
			break;
		}
		case Texture::eTextureType::eTextureType_Metallic:
		{
			for (UINT i = 0; i < m_MetallicTextures.size(); i++) {

				if (textureID == m_MetallicTextures[i]->GetTextureInfo().Id) {
					return m_MetallicTextures[i];
				}
			}
			break;
		}
		case Texture::eTextureType::eTextureType_AmbientOcclusion:
		{
			for (UINT i = 0; i < m_AOTextures.size(); i++) {

				if (textureID == m_AOTextures[i]->GetTextureInfo().Id) {
					return m_AOTextures[i];
				}
			}
			break;
		}
		case Texture::eTextureType::eTextureType_Opacity:
		{
			for (UINT i = 0; i < m_OpacityTextures.size(); i++) {

				if (textureID == m_OpacityTextures[i]->GetTextureInfo().Id) {
					return m_OpacityTextures[i];
				}
			}
			break;
		}
		case Texture::eTextureType::eTextureType_Translucency:
		{
			for (UINT i = 0; i < m_TranslucencyTextures.size(); i++) {

				if (textureID == m_TranslucencyTextures[i]->GetTextureInfo().Id) {
					return m_TranslucencyTextures[i];
				}
			}
			break;
		}
		default:
		{
			IE_CORE_WARN("Failed to get texture handle for texture with ID: {0}", textureID);
			break;
		}
		}

		return nullptr;
	}

	bool TextureManager::LoadDefaultTextures()
	{
		return true;
		Texture::IE_TEXTURE_INFO TexInfo = {};
		TexInfo.Id = -1;
		TexInfo.GenerateMipMaps = true;

#ifndef IE_IS_STANDALONE
		const char* DirExtension = "../../../Engine/";
#else
		const char* DirExtension = "";
#endif
		// TODO: Fix this not working in release and debug build without modification

		// Albedo
		TexInfo.DisplayName = "Default_Albedo";
		TexInfo.Type = Texture::eTextureType::eTextureType_Albedo;
		TexInfo.Filepath = StringHelper::StringToWide("../../../Engine/Assets/Textures/Default_Object/Default_Albedo.png");
		// Normal
		TexInfo.DisplayName = "Default_Normal";
		TexInfo.Type = Texture::eTextureType::eTextureType_Normal;
		TexInfo.Filepath = StringHelper::StringToWide("../../../Engine/Assets/Textures/Default_Object/Default_Normal.png");
		// Metallic
		TexInfo.DisplayName = "Default_Metallic";
		TexInfo.Type = Texture::eTextureType::eTextureType_Metallic;
		TexInfo.Filepath = StringHelper::StringToWide("../../../Engine/Assets/Textures/Default_Object/Default_Metallic.png");
		// Roughness
		TexInfo.DisplayName = "Default_Roughness";
		TexInfo.Type = Texture::eTextureType::eTextureType_Roughness;
		TexInfo.Filepath = StringHelper::StringToWide("../../../Engine/Assets/Textures/Default_Object/Default_RoughAO.png");
		// AO
		TexInfo.DisplayName = "Default_AO";
		TexInfo.Type = Texture::eTextureType::eTextureType_AmbientOcclusion;
		TexInfo.Filepath = StringHelper::StringToWide("../../../Engine/Assets/Textures/Default_Object/Default_RoughAO.png");

		switch (Renderer::GetAPI())
		{
		case Renderer::eTargetRenderAPI::D3D_11:
		{
			m_DefaultAlbedoTexture = make_shared<ieD3D11Texture>(TexInfo);
			m_DefaultNormalTexture = make_shared<ieD3D11Texture>(TexInfo);
			m_DefaultMetallicTexture = make_shared<ieD3D11Texture>(TexInfo);
			m_DefaultRoughnessTexture = make_shared<ieD3D11Texture>(TexInfo);
			m_DefaultAOTexture = make_shared<ieD3D11Texture>(TexInfo);
			break;
		}
		case Renderer::eTargetRenderAPI::D3D_12:
		{
			Direct3D12Context* graphicsContext = reinterpret_cast<Direct3D12Context*>(&Renderer::Get());
			CDescriptorHeapWrapper& cbvSrvHeapStart = graphicsContext->GetCBVSRVDescriptorHeap();

			m_DefaultAlbedoTexture = make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart);
			m_DefaultNormalTexture = make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart);
			m_DefaultMetallicTexture = make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart);
			m_DefaultRoughnessTexture = make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart);
			m_DefaultAOTexture = make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart);
			break;
		}
		default:
		{
			IE_CORE_ERROR("Failed to load default textures for api: {0}", Renderer::GetAPI());
			break;
		}
		}
		return true;
	}

	void TextureManager::RegisterTextureByType(const Texture::IE_TEXTURE_INFO& TexInfo)
	{

		switch (Renderer::GetAPI())
		{
			case Renderer::eTargetRenderAPI::D3D_11:
			{
				switch (TexInfo.Type) {
				case Texture::eTextureType::eTextureType_Albedo:
				{
					m_AlbedoTextures.push_back(make_shared<ieD3D11Texture>(TexInfo));
					break;
				}
				case Texture::eTextureType::eTextureType_Normal:
				{
					m_NormalTextures.push_back(make_shared<ieD3D11Texture>(TexInfo));
					break;
				}
				case Texture::eTextureType::eTextureType_Roughness:
				{
					m_RoughnessTextures.push_back(make_shared<ieD3D11Texture>(TexInfo));
					break;
				}
				case Texture::eTextureType::eTextureType_Metallic:
				{
					m_MetallicTextures.push_back(make_shared<ieD3D11Texture>(TexInfo));
					break;
				}
				case Texture::eTextureType::eTextureType_AmbientOcclusion:
				{
					m_AOTextures.push_back(make_shared<ieD3D11Texture>(TexInfo));
					break;
				}
				case Texture::eTextureType::eTextureType_Opacity:
				{
					m_OpacityTextures.push_back(make_shared<ieD3D11Texture>(TexInfo));
					break;
				}
				case Texture::eTextureType::eTextureType_Translucency:
				{
					m_TranslucencyTextures.push_back(make_shared<ieD3D11Texture>(TexInfo));
					break;
				}
				default:
				{
					IE_CORE_WARN("Failed to identify texture to create with name of {0} - ID({1})", TexInfo.DisplayName, TexInfo.Id);
					break;
				}
				}
				break;
			}
			case Renderer::eTargetRenderAPI::D3D_12:
			{
				Direct3D12Context* GraphicsContext = reinterpret_cast<Direct3D12Context*>(&Renderer::Get());
				CDescriptorHeapWrapper& cbvSrvHeapStart = GraphicsContext->GetCBVSRVDescriptorHeap();

				switch (TexInfo.Type) {
				case Texture::eTextureType::eTextureType_Albedo:
				{
					m_AlbedoTextures.push_back(make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart));
					break;
				}
				case Texture::eTextureType::eTextureType_Normal:
				{
					m_NormalTextures.push_back(make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart));
					break;
				}
				case Texture::eTextureType::eTextureType_Roughness:
				{
					m_RoughnessTextures.push_back(make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart));
					break;
				}
				case Texture::eTextureType::eTextureType_Metallic:
				{
					m_MetallicTextures.push_back(make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart));
					break;
				}
				case Texture::eTextureType::eTextureType_AmbientOcclusion:
				{
					m_AOTextures.push_back(make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart));
					break;
				}
				case Texture::eTextureType::eTextureType_Opacity:
				{
					m_OpacityTextures.push_back(make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart));
					break;
				}
				case Texture::eTextureType::eTextureType_Translucency:
				{
					m_TranslucencyTextures.push_back(make_shared<ieD3D12Texture>(TexInfo, cbvSrvHeapStart));
					break;
				}
				default:
				{
					IE_CORE_WARN("Failed to identify texture to create with name of {0} - ID({1})", TexInfo.DisplayName, TexInfo.Id);
					break;
				}
				}
				break;
			}
			default:
			{
				IE_CORE_ERROR("Failed to determine graphics api to initialize texture. The renderer may not have been initialized yet.");
				break;
			}
		}


	}
}
