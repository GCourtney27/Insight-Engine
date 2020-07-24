#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Texture.h"
#include "Platform/Windows/DirectX_12/Descriptor_Heap_Wrapper.h"

namespace Insight {

	class INSIGHT_API ieD3D12Texture : public Texture
	{
	public:
		ieD3D12Texture(IE_TEXTURE_INFO createInfo, CDescriptorHeapWrapper& srvHeapHandle);
		virtual ~ieD3D12Texture();

		// Destroy and release texture resources.
		virtual void Destroy() override;
		// Binds the texture to the pipeline to be drawn in the scene pass.
		virtual void Bind() override;

		// Get the heap handle associated with the CBV/SRV heap bound to the pipeline.
		inline const TextureHandle GetSrvHeapHandle() { return m_GPUHeapIndex; }
		// Get the Direc3D 12 resource description for this texture.
		inline D3D12_RESOURCE_DESC GetD3D12ResourceDescription() { return m_D3DTextureDesc; }
		// Get the width of the texture in texels.
		inline uint64_t GetWidth() const { return m_D3DTextureDesc.Width; }
		// Get the height of the texture in texels.
		inline uint64_t GetHeight() const { return m_D3DTextureDesc.Height; }
		// Get the number of mip levels this texture has access too.
		inline uint64_t GetMipLevels() const { return m_D3DTextureDesc.MipLevels; }
		// Get the Direc3D pixel format of this texture.
		inline DXGI_FORMAT GetFormat() const { return m_D3DTextureDesc.Format; }

	private:
		// Load  the texture from disk and initialize.
		bool Init(IE_TEXTURE_INFO createInfo, CDescriptorHeapWrapper& srvHeapHandle);
		// Load a DDS texture from disk.
		void InitDDSTexture(CDescriptorHeapWrapper& srvHeapHandle);
		// Load  generic texture file from disk.
		bool InitTextureFromFile(CDescriptorHeapWrapper& srvHeapHandle);
		// Get the Root Parameter Index this texture belongs too.
		// This should only be called once during initialization of the texture.
		UINT GetRootParameterIndexForTextureType(eTextureType TextureType);
	private:
		ID3D12GraphicsCommandList* m_pCommandList = nullptr;
		CDescriptorHeapWrapper* m_pCbvSrvHeapStart;

		ComPtr<ID3D12Resource>		m_pTexture;
		D3D12_RESOURCE_DESC			m_D3DTextureDesc = {};
		TextureHandle				m_GPUHeapIndex = 0U;

		IE_TEXTURE_INFO				m_TextureInfo = {};

		uint32_t						m_RootParamIndex = 0U;
	private:
		static uint32_t s_NumSceneTextures;

	};

}
