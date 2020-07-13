#pragma once

#include <Insight/Core.h>

#include "Platform/Windows/DirectX12/Descriptor_Heap_Wrapper.h"


namespace Insight {

	using Microsoft::WRL::ComPtr;

	class Texture
	{
	public:

		typedef UINT32 ID;
		typedef UINT32 TextureHandle;

		enum eTextureType
		{
			INVALID = -1,
			// Per Object
			ALBEDO = 0,
			NORMAL = 1,
			ROUGHNESS = 2,
			METALLIC = 3,
			AO = 4,
			// Sky Sphere
			SKY_IRRADIENCE = 5,
			SKY_ENVIRONMENT_MAP = 6,
			SKY_BRDF_LUT = 7,
			SKY_DIFFUSE = 8,
		};

		struct IE_TEXTURE_INFO
		{
			eTextureType Type = eTextureType::INVALID;
			bool GenerateMipMaps = true;
			bool IsCubeMap = false;
			std::wstring Filepath;
			std::string AssetDirectoryRelPath;
			std::string DisplayName;
			ID Id;
		};

	public:
		Texture(IE_TEXTURE_INFO createInfo, CDescriptorHeapWrapper& srvHeapHandle);
		Texture(Texture&& texture) noexcept;
		Texture() {}
		~Texture();
	
		bool Init(IE_TEXTURE_INFO createInfo, CDescriptorHeapWrapper& srvHeapHandle);
		void Destroy();

		void Bind();

		inline const TextureHandle GetSrvHeapHandle() { return m_GPUHeapIndex; }
		inline const IE_TEXTURE_INFO& GetTextureInfo() const { return m_TextureInfo; }
		inline const std::string& GetDisplayName() const { return m_TextureInfo.DisplayName; }
		inline const std::wstring& GetFilepath() const { return m_TextureInfo.Filepath; }
		inline const std::string& GetAssetDirectoryRelPath() const { return m_TextureInfo.AssetDirectoryRelPath; }

		inline const D3D12_RESOURCE_DESC& GetD3D12ResourceDescription() { return m_TextureDesc; }
		inline const UINT64& GetWidth() const { return m_TextureDesc.Width; }
		inline const UINT64& GetHeight() const { return m_TextureDesc.Height; }
		inline const UINT16& GetMipLevels() const { return m_TextureDesc.MipLevels; }
		inline const DXGI_FORMAT& GetFormat() const { return m_TextureDesc.Format; }
		
	private:
		void InitDDSTexture(CDescriptorHeapWrapper& srvHeapHandle);
		bool InitTextureFromFile(CDescriptorHeapWrapper& srvHeapHandle);

	private:
		ID3D12GraphicsCommandList*	m_pCommandList = nullptr;

		ComPtr<ID3D12Resource>		m_pTexture;
		D3D12_RESOURCE_DESC			m_TextureDesc = {};
		TextureHandle				m_GPUHeapIndex = 0u;

		IE_TEXTURE_INFO				m_TextureInfo = {};

	private:
		static UINT32 s_NumSceneTextures;
	};

}