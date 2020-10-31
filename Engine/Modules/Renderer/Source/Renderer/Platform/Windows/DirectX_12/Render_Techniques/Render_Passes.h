#pragma once

#include "Insight/Core.h"

#include "Renderer/Platform/Windows/DirectX_12/Wrappers/Descriptor_Heap_Wrapper.h"
#include "Renderer/Platform/Windows/DirectX_12/Wrappers/D3D12_Constant_Buffer_Wrapper.h"

namespace Insight {

	using Microsoft::WRL::ComPtr;

	class Direct3D12Context;
	class FrameResources;

	class RenderPass
	{
	public:
		friend class RenderPassStack;
	public:
		bool Create(Direct3D12Context* pRenderContext, CDescriptorHeapWrapper* pCBVSRVHeapRef, ID3D12GraphicsCommandList* pCommandList, ID3D12RootSignature* pRootSignature)
		{
			m_pRenderContextRef = pRenderContext;
			m_pCommandListRef = pCommandList;
			m_pRootSignatureRef = pRootSignature;
			m_pCBVSRVHeapRef = pCBVSRVHeapRef;

			return InternalCreate();
		}

		inline void Render(FrameResources* pFrameResources)
		{
			this->Set(pFrameResources);
			this->UnSet();
		}

		virtual void Reload()
		{
			throw std::runtime_error("Not Implemented!");
		}

		virtual void Destroy()
		{
			throw std::runtime_error("Not Implemented");
		}

	protected:
		RenderPass()			= default;
		virtual ~RenderPass()	= default;

		virtual bool Set(FrameResources* pFrameResources) = 0;
		virtual void UnSet() = 0;

		virtual bool InternalCreate() = 0;
		virtual void CreateResources() = 0;

		virtual void OnStackAttach() {}
		virtual void OnStackDetach() {}


	protected:
		Direct3D12Context*					m_pRenderContextRef;
		ComPtr<ID3D12GraphicsCommandList>	m_pCommandListRef;
		ComPtr<ID3D12RootSignature>			m_pRootSignatureRef;
		CDescriptorHeapWrapper*				m_pCBVSRVHeapRef;

		ComPtr<ID3D12PipelineState> m_pPipelineState;
		const float					m_ScreenClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		static const D3D12_RESOURCE_STATES IE_D3D12_DEFAULT_RESOURCE_STATE = D3D12_RESOURCE_STATE_COMMON;
	};


	/*===========================*/
	/*		Geometry Pass		 */
	/*===========================*/

	class DeferredGeometryPass : public RenderPass
	{
	public:
		DeferredGeometryPass()			= default;
		virtual ~DeferredGeometryPass() = default;

		// Returns the Scene Depth buffer.
		inline ComPtr<ID3D12Resource> GetSceneDepthTexture() const { return m_pSceneDepthStencilTexture; }
		
		// Returns a texture from the G-Buffer for a given index.
		inline ComPtr<ID3D12Resource> GetGBufferRenderTargetTexture(uint8_t Index) const { return m_pRenderTargetTextures[Index]; }
		
		// Returns the format for a G-Buffer texture at a given index.
		inline DXGI_FORMAT GetGBufferRenderTargetFormatAtIndex(uint8_t Index) const { return m_GBufferRTVFormats[Index]; }

		// Returns the number of G-Buffers the Geometry pass will write to.
		inline uint8_t GetNumGBuffers() const { return m_NumRenderTargets; }

	protected:
		virtual bool InternalCreate()	override;
		virtual void CreateResources()	override;

		virtual bool Set(FrameResources* pFrameResources) override;
		virtual void UnSet() override;
	private:
		static const uint8_t m_NumRenderTargets = 4u;

		CDescriptorHeapWrapper		m_RTVHeap;
		CDescriptorHeapWrapper		m_DSVHeap;
		ComPtr<ID3D12Resource>		m_pRenderTargetTextures[m_NumRenderTargets];

		DXGI_FORMAT	m_GBufferRTVFormats[m_NumRenderTargets] = {
						DXGI_FORMAT_R11G11B10_FLOAT,	// Albedo
						DXGI_FORMAT_R16G16B16A16_SNORM,	// Normal
						DXGI_FORMAT_R11G11B10_FLOAT,	// (R)Roughness/(G)Metallic/(B)AO
						DXGI_FORMAT_R32G32B32A32_FLOAT, // Position
		};

		// Depth
		DXGI_FORMAT	m_DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		DXGI_FORMAT	m_DSVSRVFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		FLOAT		m_DepthClearValue = 1.0f;
		ComPtr<ID3D12Resource>	m_pSceneDepthStencilTexture;
	};


	/*==============================*/
	/*			Light Pass			*/
	/*==============================*/

	class DeferredLightPass : public RenderPass
	{
	public:
		DeferredLightPass()				= default;
		virtual ~DeferredLightPass()	= default;

		// Set the reference to the G-Buffer.
		inline void SetRenderTargetTextureRef(ComPtr<ID3D12Resource> pRenderTarget) { m_GBufferRefs.push_back(pRenderTarget); }

		// Set the reference to the Scene Depth Buffer.
		inline void SetSceneDepthTextureRef(ComPtr<ID3D12Resource> pDepthTexture) { m_pSceneDepthTextureRef = pDepthTexture; }

		// Returns the number of render targets this pass writes to.
		inline uint8_t GetNumRenderTargets() const { return m_NumRenderTargets; }


	protected:
		virtual bool InternalCreate()	override;
		virtual void CreateResources()	override;

		virtual bool Set(FrameResources* pFrameResources) override;
		virtual void UnSet() override;
	private:
		static const uint8_t	m_NumRenderTargets = 2u;
		ComPtr<ID3D12Resource>	m_pRenderTargetTextures[m_NumRenderTargets];

		DXGI_FORMAT	m_RTVFormats[m_NumRenderTargets] = {
						DXGI_FORMAT_R32G32B32A32_FLOAT,	// Light Pass result
						DXGI_FORMAT_R11G11B10_FLOAT,	// Bloom threshold buffer
		};
		// GPU memory for the Light pass and bloom threshold render targets.
		CDescriptorHeapWrapper	m_RTVHeap;

		// G-Buffer references from the geometry pass.
		std::vector<ComPtr<ID3D12Resource>> m_GBufferRefs;
		// Scene depth texture referenced from the geometry pass.
		ComPtr<ID3D12Resource> m_pSceneDepthTextureRef;
	};


	/*=======================================*/
	/*		Post-Process Composite Pass		 */
	/*=======================================*/

	class PostProcessCompositePass : public RenderPass
	{
	public:
		PostProcessCompositePass()	= default;
		~PostProcessCompositePass() = default;
		
		// Set the reference to the Scene Depth Buffer.
		inline void SetSceneDepthTextureRef(ComPtr<ID3D12Resource> pDepthTexture) { m_pSceneDepthTextureRef = pDepthTexture; }


		virtual void OnStackAttach() {}
		virtual void OnStackDetach() {}

	protected:
		virtual bool InternalCreate()	override;
		virtual void CreateResources()	override;

		virtual bool Set(FrameResources* pFrameResources) override;
		virtual void UnSet() override;
	private:
		ComPtr<ID3D12Resource> m_pSceneDepthTextureRef;

	};

}
