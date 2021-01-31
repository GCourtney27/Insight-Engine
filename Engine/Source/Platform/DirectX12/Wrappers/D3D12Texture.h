#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Texture.h"
#include "Platform/DirectX12/Wrappers/DescriptorHeapWrapper.h"

namespace Insight {
	
	

	class INSIGHT_API ieD3D12Texture : public Texture
	{
	public:
		typedef UINT32 SRVHeapIndex;

	public:
		ieD3D12Texture(IE_TEXTURE_INFO createInfo, CDescriptorHeapWrapper& srvHeapHandle);
		virtual ~ieD3D12Texture();

		// Destroy and release texture resources.
		virtual void Destroy() override;
		// Binds the texture to the pipeline to be used in the deferred render pass.
		virtual void BindForDeferredPass() override;
		// Binds the texture to the pipeline to be used in the forward render pass.
		virtual void BindForForwardPass() override;

		// Get the heap handle associated with the CBV/SRV heap bound to the pipeline.
		inline const SRVHeapIndex GetSrvHeapHandle() { return m_GPUHeapIndex; }
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
		// Load a HDR file from disk.
		void InitHDRTexture(CDescriptorHeapWrapper& srvHeapHandle);
		// Load  generic texture file from disk.
		bool InitTextureFromFile(CDescriptorHeapWrapper& srvHeapHandle);
		// Get the Root Parameter Index this texture belongs too.
		// This should only be called once during initialization of the texture.
		UINT GetRootParameterIndexForTextureType(eTextureType TextureType);
	private:
		ID3D12GraphicsCommandList*	m_pScenePass_CommandList;
		ID3D12GraphicsCommandList*	m_pTranslucencyPass_CommandList;
		CDescriptorHeapWrapper*		m_pCbvSrvHeapStart;

		Microsoft::WRL::ComPtr<ID3D12Resource>		m_pTexture;
		D3D12_RESOURCE_DESC			m_D3DTextureDesc = {};
		SRVHeapIndex				m_GPUHeapIndex;

		uint32_t					m_RootParamIndex;
	private:
		static uint32_t s_NumSceneTextures;

	};

}
