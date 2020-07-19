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
	
		// Load  the texture from disk and initialize.
		bool Init(IE_TEXTURE_INFO createInfo, CDescriptorHeapWrapper& srvHeapHandle);
		// Destroy and release texture resources.
		void Destroy();
		// Binds the texture to the pipeline to be drawn in the scene pass.
		void Bind();

		// Get the heap handle associated with the CBV/SRV heap bound to the pipeline.
		inline const TextureHandle GetSrvHeapHandle() { return m_GPUHeapIndex; }
		// Get the general information about this texture.
		inline const IE_TEXTURE_INFO& GetTextureInfo() const { return m_TextureInfo; }
		// Get the filename for this texture.
		inline const std::string& GetDisplayName() const { return m_TextureInfo.DisplayName; }
		// Get the full file path to this texture on disk.
		inline const std::wstring& GetFilepath() const { return m_TextureInfo.Filepath; }
		// Get the Asset directory relative path for the texture for this project.
		inline const std::string& GetAssetDirectoryRelPath() const { return m_TextureInfo.AssetDirectoryRelPath; }

		// Get the Direc3D 12 resource description for this texture.
		inline D3D12_RESOURCE_DESC GetD3D12ResourceDescription() { return m_TextureDesc; }
		// Get the width of the texture in texels.
		inline UINT64 GetWidth() const { return m_TextureDesc.Width; }
		// Get the height of the texture in texels.
		inline UINT64 GetHeight() const { return m_TextureDesc.Height; }
		// Get the number of mip levels this texture has access too.
		inline UINT16 GetMipLevels() const { return m_TextureDesc.MipLevels; }
		// Get the Direc3D pixel format of this texture.
		inline DXGI_FORMAT GetFormat() const { return m_TextureDesc.Format; }
		
	private:
		// Load a DDS texture from disk.
		void InitDDSTexture(CDescriptorHeapWrapper& srvHeapHandle);
		// Load  generic texture file from disk.
		bool InitTextureFromFile(CDescriptorHeapWrapper& srvHeapHandle);
		// Get the Root Parameter Index this texture belongs too.
		// This should only be called once during initialization of the texture.
		UINT GetRootParameterIndexForTextureType(eTextureType TextureType);
	private:
		ID3D12GraphicsCommandList*	m_pCommandList = nullptr;
		CDescriptorHeapWrapper*		m_pCbvSrvHeapStart;

		ComPtr<ID3D12Resource>		m_pTexture;
		D3D12_RESOURCE_DESC			m_TextureDesc = {};
		TextureHandle				m_GPUHeapIndex = 0U;

		IE_TEXTURE_INFO				m_TextureInfo = {};

		UINT						m_RootParamIndex = 0U;
	private:
		static UINT32 s_NumSceneTextures;
	};

}