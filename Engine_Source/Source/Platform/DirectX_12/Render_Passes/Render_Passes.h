#pragma once

#include "Insight/Core.h"

#include "Platform/DirectX_12/Ray_Tracing/Ray_Trace_Helpers.h"
#include "Platform/DirectX_12/Render_Passes/Helpers/Pass_Helpers.h"
#include "Platform/DirectX_12/Wrappers/D3D12_Constant_Buffer_Wrapper.h"



namespace Insight {

	

	class Direct3D12Context;
	class FrameResources;

	class INSIGHT_API RenderPass
	{
	public:
		friend class RenderPassStack;
	public:

		// Assign variables and initialize the components of the render pass.
		bool Create(Direct3D12Context* pRenderContext, CDescriptorHeapWrapper* pCBVSRVHeapRef, ID3D12GraphicsCommandList* pCommandList, ID3D12RootSignature* pRootSignature)
		{
			m_pRenderContextRef = pRenderContext;
			m_pCommandListRef = pCommandList;
			m_pRootSignatureRef = pRootSignature;
			m_pCBVSRVHeapRef = pCBVSRVHeapRef;

			return InternalCreate();
		}

		// Render the pass.
		inline void Render(FrameResources* pFrameResources)
		{
			this->Set(pFrameResources);
			this->UnSet(pFrameResources);
		}

		// Reload the shaders and pipeline associated with this pass.
		inline void ReloadShaders() { this->LoadPipeline(); }

		// Resize and/or recreate any resoures this pass contains.
		inline void ResizeBuffers() { this->CreateResources(); }
		
	protected:
		RenderPass()			= default;
		virtual ~RenderPass()	= default;

		// Bind the pass for rendering.
		virtual bool Set(FrameResources* pFrameResources) = 0;
		// Unbind the pass for rendering and resotore any resoures to their generic state for other passes.
		virtual void UnSet(FrameResources* pFrameResources) = 0;

		// Hlepr function fo initializing the pass.
		virtual bool InternalCreate() = 0;
		// Load the pipeline state for the pas.
		virtual void LoadPipeline() = 0;
		// Create any resoures the pass needs.
		virtual void CreateResources() = 0;

		// Called when the pass is attached to the render pass stack.
		virtual void OnStackAttach() {}
		// Called when the pass is detached from the render pass stack.
		virtual void OnStackDetach() {}

	protected:
		// Reference to the render context.
		Direct3D12Context*					m_pRenderContextRef = nullptr;
		// Reference to the GPU memory for the resources in the application.
		CDescriptorHeapWrapper*				m_pCBVSRVHeapRef = nullptr;
		// Reference Command list used to execure commands assocaiated with this pass.
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_pCommandListRef;
		// Reference to the shader root siganture this pass will exeute with.
		Microsoft::WRL::ComPtr<ID3D12RootSignature>			m_pRootSignatureRef;

		// The state of the pipeline including which shaders get exected.
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pPipelineState;
		// Clear color for all buffers the render passes will clear to.
		static constexpr float					s_ScreenClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		// The genreric state all resources will be bind to/from when they are finished to resources.
		// Execution of a resource could happen from any pass, "ResourceBarrier'ing to this state will
		// insure there are no state conflics.
		static const D3D12_RESOURCE_STATES IE_D3D12_DEFAULT_RESOURCE_STATE = D3D12_RESOURCE_STATE_COMMON;
	};


	/*===========================*/
	/*		Geometry Pass		 */
	/*===========================*/

	class INSIGHT_API DeferredGeometryPass : public RenderPass
	{
	public:
		DeferredGeometryPass()			= default;
		virtual ~DeferredGeometryPass() = default;

		// Returns the Scene Depth buffer.
		inline Microsoft::WRL::ComPtr<ID3D12Resource> GetSceneDepthTexture() const { return m_pSceneDepthStencilTexture; }
		
		// Returns a texture from the G-Buffer for a given index.
		inline Microsoft::WRL::ComPtr<ID3D12Resource> GetGBufferRenderTargetTexture(uint8_t Index) const { return m_pRenderTargetTextures[Index]; }
		
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
		Microsoft::WRL::ComPtr<ID3D12Resource>		m_pRenderTargetTextures[m_NumRenderTargets];

		DXGI_FORMAT	m_GBufferRTVFormats[m_NumRenderTargets] = {
						DXGI_FORMAT_R11G11B10_FLOAT,	// Albedo
						DXGI_FORMAT_R16G16B16A16_SNORM,	// Normal
						DXGI_FORMAT_R16G16B16A16_FLOAT,	// (R)Roughness/(G)Metallic/(B)AO/ (A)Specular
						DXGI_FORMAT_R32G32B32A32_FLOAT, // Position
		};

		// Depth
		DXGI_FORMAT	m_DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		DXGI_FORMAT	m_DSVSRVFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		FLOAT		m_DepthClearValue = 1.0f;
		Microsoft::WRL::ComPtr<ID3D12Resource>	m_pSceneDepthStencilTexture;
	};


	/*==============================*/
	/*			Light Pass			*/
	/*==============================*/

	class INSIGHT_API DeferredLightPass : public RenderPass
	{
	public:
		DeferredLightPass()				= default;
		virtual ~DeferredLightPass()	= default;

		// Set the reference to the G-Buffer.
		inline void SetRenderTargetTextureRef(Microsoft::WRL::ComPtr<ID3D12Resource> pRenderTarget) { m_GBufferRefs.push_back(pRenderTarget); }

		// Set the reference to the Scene Depth Buffer.
		inline void SetSceneDepthTextureRef(Microsoft::WRL::ComPtr<ID3D12Resource> pDepthTexture) { m_pSceneDepthTextureRef = pDepthTexture; }

		// Set the referance to the global skylight;
		inline void SetSkyLightRef(ASkyLight* pSkyLight) { m_pSkyLightRef = pSkyLight; }

		// Returns the number of render targets this pass writes to.
		inline uint8_t GetNumRenderTargets() const { return m_NumRenderTargets; }

		// Returns the result of the light pass.
		inline Microsoft::WRL::ComPtr<ID3D12Resource> GetLightPassResult() const { return m_pRenderTargetTextures[0]; }

		// Returns the CPU handle to the light pass result buffer.
		inline D3D12_CPU_DESCRIPTOR_HANDLE GetLightPassResultCPUHandle() { return m_RTVHeap.hCPU(0); }

		// Returns the prefilted bloom threshold buffer.
		inline Microsoft::WRL::ComPtr<ID3D12Resource> GetBloomThresholdResult() const { return m_pRenderTargetTextures[1]; }

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
		Microsoft::WRL::ComPtr<ID3D12Resource>	m_pRenderTargetTextures[m_NumRenderTargets];

		DXGI_FORMAT	m_RTVFormats[m_NumRenderTargets] = {
						DXGI_FORMAT_R32G32B32A32_FLOAT,	// Light Pass result
						DXGI_FORMAT_R11G11B10_FLOAT,	// Bloom threshold buffer
		};
		// GPU memory for the Light pass and bloom threshold render targets.
		CDescriptorHeapWrapper	m_RTVHeap;

		// G-Buffer references from the geometry pass.
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_GBufferRefs;
		// Scene depth texture referenced from the geometry pass.
		Microsoft::WRL::ComPtr<ID3D12Resource> m_pSceneDepthTextureRef;

		ASkyLight* m_pSkyLightRef = nullptr;
	};



	/*=======================*/
	/*		Sky Pass		 */
	/*=======================*/

	class INSIGHT_API SkyPass : public RenderPass
	{
	public:
		SkyPass() = default;
		~SkyPass() = default;
	
		// Set the reference to the Scene depth texture.
		inline void SetSceneDepthTextureRef(Microsoft::WRL::ComPtr<ID3D12Resource> pSceneDepthBuffer, D3D12_CPU_DESCRIPTOR_HANDLE pDSV) { m_pSceneDepthTextureRef = pSceneDepthBuffer; m_pDSVHandle = pDSV; }
		
		// Set the reference to the render target to draw the sky to.
		inline void SetRenderTargetRef(Microsoft::WRL::ComPtr<ID3D12Resource> pRenderTarget, D3D12_CPU_DESCRIPTOR_HANDLE pRTV) { m_pRenderTargetRef = pRenderTarget; m_pRTVHandle = pRTV;}
		
		// Set the sky that will be drawn.
		inline void SetSkySphereRef(ASkySphere* pSkySphere) { m_pSkyShereRef = pSkySphere; }
		
	protected:
		virtual bool Set(FrameResources* pFrameResources) override;
		virtual void UnSet(FrameResources* pFrameResources) override;

		virtual bool InternalCreate() override;
		virtual void LoadPipeline() override;
		virtual void CreateResources() override;

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_pSceneDepthTextureRef;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_pRenderTargetRef;
		ASkySphere* m_pSkyShereRef = nullptr;

		D3D12_CPU_DESCRIPTOR_HANDLE m_pRTVHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE m_pDSVHandle;
	};


	/*===========================================*/
	/*		Screen-Space Reflections Pass		 */
	/*===========================================*/
	class INSIGHT_API ScreenSpaceReflectionsPass : public RenderPass
	{

	};


	/*===============================================*/
	/*		Screen-Space Ambient Occlusion Pass		 */
	/*===============================================*/
	class INSIGHT_API ScreenSpaceAmbientOcclusionPass : public RenderPass
	{

	};


	/*===================================*/
	/*		Ray-Traced Shadows Pass		 */
	/*===================================*/

	class INSIGHT_API RayTracedShadowsPass : public RenderPass
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
		RayTraceHelpers							m_RTHelper;
		Microsoft::WRL::ComPtr<ID3D12Resource>	m_pRayTraceOutput_SRV;

	};


	/*===========================*/
	/*		Shadow Map Pass		 */
	/*===========================*/

	class INSIGHT_API ShadowMapPass : public RenderPass
	{
	public:
		ShadowMapPass() = default;
		~ShadowMapPass() = default;

	protected:
		virtual bool Set(FrameResources * pFrameResources) override;
		virtual void UnSet(FrameResources * pFrameResources) override;

		virtual bool InternalCreate() override;
		virtual void LoadPipeline() override;
		virtual void CreateResources() override;
	};


	/*=======================*/
	/*		Bloom Pass		 */
	/*=======================*/

	class INSIGHT_API BloomPass : public RenderPass
	{
	public:
		BloomPass() = default;
		~BloomPass() = default;

		/*
			Create the downsampler and assigned the resources it will depend on.
			@param pCommandList - The command list the downsampler will execute on.
		*/
		void InitHelpers(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> pFirstPassCommandList, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> pSecondPassCommandList);

		void ResizeHelperBuffers()
		{
			m_DownSampleHelper.SetSRVSourceHandle(m_pCBVSRVHeapRef->hGPU(8));
			m_DownSampleHelper.SetUAVDestinationHandle(m_pCBVSRVHeapRef->hGPU(9));
		
			m_GaussianBlurHelper.SetSourceUAV(m_pDownsampleResult_UAV, m_pCBVSRVHeapRef->hGPU(9));
			m_GaussianBlurHelper.SetSourceSRV(m_pDownsampleResult_SRV, m_pCBVSRVHeapRef->hGPU(10));
			m_GaussianBlurHelper.SetIntermediateUAV(m_pIntermediateBuffer_UAV, m_pCBVSRVHeapRef->hGPU(11));
			m_GaussianBlurHelper.SetIntermediateSRV(m_pIntermediateBuffer_SRV, m_pCBVSRVHeapRef->hGPU(12));
		}

	protected:
		virtual bool Set(FrameResources * pFrameResources) override;
		virtual void UnSet(FrameResources * pFrameResources) override;

		virtual bool InternalCreate() override;
		virtual void LoadPipeline() override;
		virtual void CreateResources() override;
		
	private:
		// The Unorder Access View that will be used to downsample and blur.
		Microsoft::WRL::ComPtr<ID3D12Resource>		m_pDownsampleResult_UAV;

		// The Shader Resource View alias for the UAV that was downsampled to in with "BloomPass::m_DownSampleHelper".
		// The constents are populated via a ID3D12GraphicsCommandList::ResourceCopy command, with 
		// "BloomPass::m_pDownsampleResult_UAV" being the source for the copy.
		Microsoft::WRL::ComPtr<ID3D12Resource>		m_pDownsampleResult_SRV;

		// Unordered Access View to the intermediate resource to blur to.
		Microsoft::WRL::ComPtr<ID3D12Resource>		m_pIntermediateBuffer_UAV;

		// The Sahder Resouce view alias of "BloomPass::m_pBloomBlurIntermediateBuffer_UAV". This is the final blurred image.
		// The contents are populated via a "ID3D12GraphicsCommandList::ResourceCopy" command, with 
		// "BloomPass::m_pIntermediateBuffer_UAV" being the source for the copy.
		Microsoft::WRL::ComPtr<ID3D12Resource>		m_pIntermediateBuffer_SRV;

		// Helper that will downsample the bloom render target.
		ThresholdDownSampleHelper	m_DownSampleHelper;

		// Helper that will blur the downsampled render target.
		GaussianBlurHelper			m_GaussianBlurHelper;

	};


	/*=======================================*/
	/*		Post-Process Composite Pass		 */
	/*=======================================*/

	class INSIGHT_API PostProcessCompositePass : public RenderPass
	{
	public:
		PostProcessCompositePass()	= default;
		~PostProcessCompositePass() = default;
		
		// Set the reference to the Scene Depth Buffer.
		inline void SetSceneDepthTextureRef(Microsoft::WRL::ComPtr<ID3D12Resource> pDepthTexture) { m_pSceneDepthTextureRef = pDepthTexture; }


		virtual void OnStackAttach() {}
		virtual void OnStackDetach() {}

	protected:
		virtual bool InternalCreate()	override;
		virtual void LoadPipeline() override;
		virtual void CreateResources()	override;

		virtual bool Set(FrameResources* pFrameResources) override;
		virtual void UnSet(FrameResources* pFrameResources) override;
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_pSceneDepthTextureRef;
		static const uint8_t m_NumRenderTargets = 1u;
	};

}
