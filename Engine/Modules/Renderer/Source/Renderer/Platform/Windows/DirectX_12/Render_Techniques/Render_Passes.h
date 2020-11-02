#pragma once

#include "Insight/Core.h"

#include "Renderer/Platform/Windows/DirectX_12/Wrappers/Descriptor_Heap_Wrapper.h"
#include "Renderer/Platform/Windows/DirectX_12/Wrappers/D3D12_Constant_Buffer_Wrapper.h"
#include "Renderer/Platform/Windows/DirectX_12/Ray_Tracing/Ray_Trace_Helpers.h"

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
			this->UnSet(pFrameResources);
		}

		void ReloadShaders() 
		{
			this->LoadPipeline();
		}
		void ResizeBuffers() 
		{
			this->CreateResources();
		}
		

	protected:
		RenderPass()			= default;
		virtual ~RenderPass()	= default;

		virtual bool Set(FrameResources* pFrameResources) = 0;
		virtual void UnSet(FrameResources* pFrameResources) = 0;

		virtual bool InternalCreate() = 0;
		virtual void LoadPipeline() = 0;
		virtual void CreateResources() = 0;

		virtual void OnStackAttach() {}
		virtual void OnStackDetach() {}

	protected:
		Direct3D12Context*					m_pRenderContextRef = nullptr;
		CDescriptorHeapWrapper*				m_pCBVSRVHeapRef = nullptr;
		ComPtr<ID3D12GraphicsCommandList>	m_pCommandListRef;
		ComPtr<ID3D12RootSignature>			m_pRootSignatureRef;

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

		// Returns the format of the scene depth buffer.
		DXGI_FORMAT GetSceneDepthBufferFormat() const { return m_DSVFormat; }

		// Returns the CPU handle to the Scene depth buffer.
		inline D3D12_CPU_DESCRIPTOR_HANDLE GetSceneDepthCPUHandle() { return m_DSVHeap.hCPU(0); }


	protected:
		virtual bool InternalCreate()	override;
		virtual void LoadPipeline() override;
		virtual void CreateResources()	override;


		virtual bool Set(FrameResources* pFrameResources) override;
		virtual void UnSet(FrameResources* pFrameResources) override;
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

		// Set the referance to the global skylight;
		inline void SetSkyLightRef(ASkyLight* pSkyLight) { m_pSkyLightRef = pSkyLight; }

		// Returns the number of render targets this pass writes to.
		inline uint8_t GetNumRenderTargets() const { return m_NumRenderTargets; }

		// Returns the result of the light pass.
		inline ComPtr<ID3D12Resource> GetLightPassResult() const { return m_pRenderTargetTextures[0]; }

		// Returns the CPU handle to the light pass result buffer.
		inline D3D12_CPU_DESCRIPTOR_HANDLE GetLightPassResultCPUHandle() { return m_RTVHeap.hCPU(0); }

		// Returns the prefilted bloom threshold buffer.
		inline ComPtr<ID3D12Resource> GetBloomThresholdResult() const { return m_pRenderTargetTextures[1]; }

		// Returns the CPU handle to the Bloom threshold buffer.
		inline D3D12_CPU_DESCRIPTOR_HANDLE GetBloomThresholdCPUHandle() { return m_RTVHeap.hCPU(1); }


	protected:
		virtual bool InternalCreate()	override;
		virtual void LoadPipeline() override;
		virtual void CreateResources()	override;

		virtual bool Set(FrameResources* pFrameResources) override;
		virtual void UnSet(FrameResources* pFrameResources) override;
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

		ASkyLight* m_pSkyLightRef;
	};



	/*=======================*/
	/*		Sky Pass		 */
	/*=======================*/

	class SkyPass : public RenderPass
	{
	public:
		SkyPass() = default;
		~SkyPass() = default;
	
		// Set the reference to the Scene depth texture.
		inline void SetSceneDepthTextureRef(ComPtr<ID3D12Resource> pSceneDepthBuffer, D3D12_CPU_DESCRIPTOR_HANDLE pDSV) { m_pSceneDepthTextureRef = pSceneDepthBuffer; m_pDSVHandle = pDSV; }
		
		// Set the reference to the render target to draw the sky to.
		inline void SetRenderTargetRef(ComPtr<ID3D12Resource> pRenderTarget, D3D12_CPU_DESCRIPTOR_HANDLE pRTV) { m_pRenderTargetRef = pRenderTarget; m_pRTVHandle = pRTV;}
		
		// Set the sky that will be drawn.
		inline void SetSkySphereRef(ASkySphere* pSkySphere) { m_pSkyShereRef = pSkySphere; }
		
	protected:
		virtual bool Set(FrameResources* pFrameResources) override;
		virtual void UnSet(FrameResources* pFrameResources) override;

		virtual bool InternalCreate() override;
		virtual void LoadPipeline() override;
		virtual void CreateResources() override;

	private:
		ComPtr<ID3D12Resource> m_pSceneDepthTextureRef;
		ComPtr<ID3D12Resource> m_pRenderTargetRef;
		ASkySphere* m_pSkyShereRef;

		D3D12_CPU_DESCRIPTOR_HANDLE m_pRTVHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE m_pDSVHandle;
	};


	/*===================================*/
	/*		Ray-Traced Shadows Pass		 */
	/*===================================*/

	class RayTracedShadowsPass : public RenderPass
	{
	public:
		RayTracedShadowsPass() = default;
		~RayTracedShadowsPass() = default;

		RayTraceHelpers* GetRTHelper() { return &m_RTHelper; }

	protected:
		virtual bool Set(FrameResources* pFrameResources) override;
		virtual void UnSet(FrameResources* pFrameResources) override;

		virtual bool InternalCreate() override;
		virtual void LoadPipeline() override;
		virtual void CreateResources() override;

	private:
		RayTraceHelpers		m_RTHelper;
		ComPtr<ID3D12Resource>	m_pRayTraceOutput_SRV;


	};


	/*===================================*/
	/*		Shadow Map Shadows Pass		 */
	/*===================================*/

	class ShadowMapPass : public RenderPass
	{

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
		virtual void LoadPipeline() override;
		virtual void CreateResources()	override;

		virtual bool Set(FrameResources* pFrameResources) override;
		virtual void UnSet(FrameResources* pFrameResources) override;
	private:
		ComPtr<ID3D12Resource> m_pSceneDepthTextureRef;
		static const uint8_t m_NumRenderTargets = 1u;
	};

}
