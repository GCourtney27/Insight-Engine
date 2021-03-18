#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Renderer.h"

#include "Platform/DirectX12/D3D12DeviceResources.h"
#include "Platform/DirectX12/Wrappers/D3D12ScreenQuad.h"
#include "Platform/DirectX12/RenderPasses/RenderPassStack.h"


/*
	Render context for Windows Direct3D 12 API. 
*/

#define IE_D3D12_FrameIndex m_DeviceResources.GetFrameIndex()


namespace Insight {


	class FrameResources
	{
	public:
		FrameResources()	= default;
		~FrameResources()	= default;

		void Init(Microsoft::WRL::ComPtr<ID3D12Device> pDevice)
		{
			// Constant Buffer containing lights for the world.
			m_CBLights.Init(pDevice.Get(), L"Lights Constant Buffer");

			// Per-Frame information.
			m_CBPerFrame.Init(pDevice.Get(), L"Per-Frame Constant Buffer");

			// Per-Object Constant Buffer.
			m_CBPerObject.Init(pDevice.Get(), L"Per-Object Constant Buffer");

			// Per-Object Material Constant Buffer.
			m_CBPerObjectMaterial.Init(pDevice.Get(), L"Per-Object Material Overrides Constant Buffer");

			// Down Sample Params for Bloom Pass
			m_CBDownSampleParams.Init(pDevice.Get(), L"Down Sample Params");

			// Gaussian Blur Params
			m_CBBlurParams.Init(pDevice.Get(), L"Bloom Gaussian Blur Params");
			// Compute Blur Params
			{
				m_CBBlurParams.Data.Radius = GAUSSIAN_RADIUS;
				m_CBBlurParams.Data.Direction = 0;

				// compute Gaussian kernel
				float sigma = 10.0f;
				float sigmaRcp = 1.0f / sigma;
				float twoSigmaSq = 2.0f * sigma * sigma;

				float sum = 0.0f;
				for (size_t i = 0; i <= GAUSSIAN_RADIUS; ++i)
				{
					// we omit the normalization factor here for the discrete version and normalize using the sum afterwards
					m_CBBlurParams.Data.Coefficients[i] = (1.f / sigma) * std::expf(-static_cast<float>(i * i) / twoSigmaSq);
					// we use each entry twice since we only compute one half of the curve
					sum += 2 * m_CBBlurParams.Data.Coefficients[i];
				}
				// the center (index 0) has been counted twice, so we subtract it once
				sum -= m_CBBlurParams.Data.Coefficients[0];

				// we normalize all entries using the sum so that the entire kernel gives us a sum of coefficients = 0
				float normalizationFactor = 1.f / sum;
				for (size_t i = 0; i <= GAUSSIAN_RADIUS; ++i)
				{
					m_CBBlurParams.Data.Coefficients[i] *= normalizationFactor;
				}
			}

			// Post-Processing Constant Buffer.
			m_CBPostProcessParams.Init(pDevice.Get(), L"Post-Process Constant Buffer");
		}

		D3D12ConstantBuffer<CB_PS_Lights>							m_CBLights;
		D3D12ConstantBuffer<CB_PS_VS_PerFrame>						m_CBPerFrame;
		D3D12ConstantBuffer<CB_CS_BlurParams>						m_CBBlurParams;
		D3D12ConstantBuffer<CB_CS_DownSampleParams>					m_CBDownSampleParams;
		D3D12ConstantBuffer<CB_PS_PostFx>							m_CBPostProcessParams;
		D3D12ConstantBuffer<CB_VS_PerObject>						m_CBPerObject;
		D3D12ConstantBuffer<CB_PS_VS_PerObjectMaterialAdditives>	m_CBPerObjectMaterial;
	};


	class Win32Window;
	class GeometryManager;
	class ieD3D12SphereRenderer;

	class INSIGHT_API Direct3D12Context : public Renderer
	{
	public:
		friend class Renderer;
		friend class D3D12DeviceResources;

		friend class RayTraceHelpers;

		friend class SkyPass;
		friend class ShadowMapPass;
		friend class DeferredLightPass;
		friend class DeferredGeometryPass;
		friend class RayTracedShadowsPass;
		friend class PostProcessCompositePass;
		friend class RayTracedShadowsPass;
		friend class BloomPass;
		friend class D3D12ImGuiLayer;

	public:
		virtual bool Init_Impl() override;
		virtual void Destroy_Impl() override;
		virtual bool PostInit_Impl() override;
		virtual void OnUpdate_Impl(const float DeltaMs) override;
		virtual void OnPreFrameRender_Impl() override;
		virtual void OnRender_Impl() override;
		virtual void OnMidFrameRender_Impl() override;
		virtual void OnEditorRender_Impl() override;
		virtual void ExecuteDraw_Impl() override;
		virtual void SwapBuffers_Impl() override;
		virtual void OnWindowResize_Impl() override;
		virtual void OnWindowFullScreen_Impl() override;
		virtual void OnShaderReload_Impl() override;

		virtual void SetVertexBuffers_Impl(uint32_t StartSlot, uint32_t NumBuffers, ieVertexBuffer* pBuffers) override;
		virtual void SetIndexBuffer_Impl(ieIndexBuffer* pBuffer) override;
		virtual void DrawIndexedInstanced_Impl(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation) override;
		virtual void DrawText_Impl(const wchar_t* Text) override;

		virtual void RenderSkySphere_Impl() override;
		virtual bool CreateSkybox_Impl() override;
		virtual void DestroySkybox_Impl() override;

		inline ID3D12Device& GetDeviceContext() const { return m_DeviceResources.GetD3D12Device(); }
		inline DXGI_FORMAT GetSwapChainBackBufferFormat() const { return m_DeviceResources.GetSwapChainBackBufferFormat(); }
		inline D3D12DeviceResources& GetDeviceResources() { return m_DeviceResources; }

		inline ID3D12GraphicsCommandList& GetScenePassCommandList() const { return *m_pScenePass_CommandList.Get(); }
		inline ID3D12GraphicsCommandList& GetPostProcessPassCommandList() const { return *m_pPostEffectsPass_CommandList.Get(); }
		inline ID3D12GraphicsCommandList& GetShadowPassCommandList() const { return *m_pShadowPass_CommandList.Get(); }
		inline ID3D12GraphicsCommandList& GetTransparencyPassCommandList() const { return *m_pTransparencyPass_CommandList.Get(); }

		inline D3D12_RECT GetClientScissorRect() const { return m_DeviceResources.GetClientScissorRect(); }
		inline D3D12_VIEWPORT GetClientViewPort() const { return m_DeviceResources.GetClientViewPort(); }
		inline ID3D12CommandQueue& GetCommandQueue() const { return m_DeviceResources.GetGraphicsCommandQueue(); }
		inline CDescriptorHeapWrapper& GetCBVSRVDescriptorHeap() { return m_cbvsrvHeap; }
		
		inline ID3D12Resource& GetConstantBufferPerObjectUploadHeap() const { return *m_FrameResources.m_CBPerObject.GetResource(); }
		inline UINT8* GetPerObjectCBVGPUHeapAddress() { return m_FrameResources.m_CBPerObject.GetGPUAddress(); }
		inline ID3D12Resource& GetConstantBufferPerObjectMaterialUploadHeap() const { return *m_FrameResources.m_CBPerObjectMaterial.GetResource(); }
		inline UINT8* GetPerObjectMaterialAdditiveCBVGPUHeapAddress() { return m_FrameResources.m_CBPerObjectMaterial.GetGPUAddress(); }

		const CB_PS_VS_PerFrame& GetPerFrameCB() const { return m_FrameResources.m_CBPerFrame.Data; }

		inline void SetActiveCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> pCommandList) { m_pActiveCommandList = pCommandList; }

		// Ray Tracing
		// -----------
		ID3D12Resource* GetRayTracingSRV() const { return m_RayTraceOutput_SRV.Get(); }
		NO_DISCARD uint32_t RegisterGeometryWithRTAccelerationStucture(Microsoft::WRL::ComPtr<ID3D12Resource> pVertexBuffer, Microsoft::WRL::ComPtr<ID3D12Resource> pIndexBuffer, uint32_t NumVerticies, uint32_t NumIndices, DirectX::XMMATRIX MeshWorldMat);
		void UpdateRTAccelerationStructureMatrix(uint32_t InstanceArrIndex, DirectX::XMMATRIX NewWorldMat) { m_RayTracedShadowPass.GetRTHelper()->UpdateInstanceTransformByIndex(InstanceArrIndex, NewWorldMat); }

		inline void ResetBloomFirstPass()
		{
			ThrowIfFailed(m_pBloomFirstPass_CommandList->Reset(m_pBloomFirstPass_CommandAllocators[IE_D3D12_FrameIndex].Get(), m_pThresholdDownSample_PSO.Get()), TEXT(""));
		}
		

		void TryCompiledShaders(bool bForceRecompile = false);
		

	private:
		Direct3D12Context();
		virtual ~Direct3D12Context();

		void CloseCommandListAndSignalCommandQueue();

		// Per-Frame
		
		void BindShadowPass();
		void BindTransparencyPass();
		void DrawDebugScreenQuad();

		// D3D12 Initialize
		
		// Create app resources
		
		void CreateDSVs();
		void CreateDeferredShadingRS();
		void CreateForwardShadingRS();
		// Texture down-sample and Gaussian Blur pipelines share the same shader inputs for the Bloom Pass.
		void CreateBloomPassRS();
		void CreateDebugScreenQuadRS();

		void LoadPipelines();

		// Create window resources
		
		void CreateCommandAllocators();
		void CreateViewport();
		void CreateScissorRect();
		void CreateScreenQuad();

		/*
			Close GPU handle and release resources for the D3D 12 context.
		*/
		void InternalCleanup();
		
		/*
			Resize render targets and depth stencil. Usually called from 'OnWindowResize'.
		*/
		void UpdateSizeDependentResources();

		/*
			Batches multiple resoures into a single transition. MSDN recommends batching transitions for performance reasons. So, this method should 
			be prefered over regular 'CommandList::ResourceBarrier' calls. Note: Can only batch 8 resources at a time.
			@param pCommandList - Command list to execute the transitions on.
			@param pResources - A pointer to the first element in an array of resources to transition.
			@param StateBefore - The current state of the resources.
			@param StateAfter - The state to transition the resources to.
			@param NumBarriers - Number of resoures present in pResources array to batch together.
		*/
		void ResourceBarrier(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource** pResources, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter, uint32_t NumBarriers = 1u);
		
		void LoadAssets();

	private:
		D3D12DeviceResources		m_DeviceResources;

		FrameResources				m_FrameResources;

		RenderPassStack				m_RenderPassStack;
		DeferredGeometryPass		m_GeometryPass;
		DeferredLightPass			m_LightPass;
		RayTracedShadowsPass		m_RayTracedShadowPass;
		SkyPass						m_SkyPass;
		BloomPass					m_BloomPass;
		PostProcessCompositePass	m_PostProcessCompositePass;

		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_textBrush;
		Microsoft::WRL::ComPtr<IDWriteTextFormat> m_textFormat;

		std::wstring m_Text;

		D3D12ScreenQuad		m_DebugScreenQuad;
		D3D12_VIEWPORT		m_ShadowPass_ViewPort = {};
		D3D12_RECT			m_ShadowPass_ScissorRect = {};

		ieD3D12SphereRenderer*	m_pSkySphere_Geometry;

		bool				m_WindowResizeComplete = true;
		bool				m_UseWarpDevice = false;

		// D3D 12 Usings

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_pActiveCommandList;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>	m_pRayTracePass_CommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		m_pRayTracePass_CommandAllocators[m_FrameBufferCount];
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_pShadowPass_CommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		m_pShadowPass_CommandAllocators[m_FrameBufferCount];
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_pScenePass_CommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		m_pScenePass_CommandAllocators[m_FrameBufferCount];
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_pTransparencyPass_CommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		m_pTransparencyPass_CommandAllocators[m_FrameBufferCount];
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_pPostEffectsPass_CommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		m_pPostEffectsPass_CommandAllocators[m_FrameBufferCount];
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_pBloomFirstPass_CommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		m_pBloomFirstPass_CommandAllocators[m_FrameBufferCount];
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_pBloomSecondPass_CommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		m_pBloomSecondPass_CommandAllocators[m_FrameBufferCount];



		//-----Light Pass-----
		// 0: Albedo
		// 1: Normal
		// 2: (R)Roughness/(G)Metallic/(B)AO/(A)Specular
		// 3: World Position
		// -----Post-Fx Pass-----
		// 4: Light Pass result
		// 5: Bloom Buffer
		CDescriptorHeapWrapper				m_rtvHeap;

		//0:  SceneDepth
		//1:  ShadowDepth
		CDescriptorHeapWrapper				m_dsvHeap;

		Microsoft::WRL::ComPtr<ID3D12Resource>				m_pShadowDepthTexture;
		Microsoft::WRL::ComPtr<ID3D12Resource>				m_RayTraceOutput_SRV;
		
		Microsoft::WRL::ComPtr<ID3D12Resource>				m_pBloomBlurResult_UAV;
		Microsoft::WRL::ComPtr<ID3D12Resource>				m_pBloomBlurResult_SRV;
		Microsoft::WRL::ComPtr<ID3D12Resource>				m_pBloomBlurIntermediateBuffer_UAV;
		Microsoft::WRL::ComPtr<ID3D12Resource>				m_pBloomBlurIntermediateBuffer_SRV;


		Microsoft::WRL::ComPtr<ID3D12RootSignature>			m_pDeferredShadingPass_RS;
		Microsoft::WRL::ComPtr<ID3D12RootSignature>			m_pForwardShadingPass_RS;
		Microsoft::WRL::ComPtr<ID3D12RootSignature>			m_pBloomPass_RS;
		Microsoft::WRL::ComPtr<ID3D12RootSignature>			m_pDebugScreenQuad_RS;

		Microsoft::WRL::ComPtr<ID3D12PipelineState>			m_pShadowPass_PSO;
		Microsoft::WRL::ComPtr<ID3D12PipelineState>			m_pSkyPass_PSO;
		Microsoft::WRL::ComPtr<ID3D12PipelineState>			m_pTransparency_PSO;
		Microsoft::WRL::ComPtr<ID3D12PipelineState>			m_pDebugScreenQuad_PSO;
		Microsoft::WRL::ComPtr<ID3D12PipelineState>			m_pThresholdDownSample_PSO;
		Microsoft::WRL::ComPtr<ID3D12PipelineState>			m_pGaussianBlur_PSO;

		//-----Pipeline-----
		//0:   SRV-Albedo(RTV->SRV)
		//1:   SRV-Normal(RTV->SRV)
		//2:   SRV-(R)Roughness/(G)Metallic/(B)AO(RTV->SRV)
		//3:   SRV-Position(RTV->SRV)
		//4:   SRV-Scene Depth(DSV->SRV)
		//5:   SRV-Light Pass Result(RTV->SRV)
		//6:   UAV-Ray Trace Output(RTHelper UAV(COPY)->SRV)

		//7:   SRV-Shadow Depth(DSV->SRV)
		//8:   SRV-Raw Bloom Buffer(RTV->SRV)
		//9:   UAV-Bloom Down Sampled(UAV)
		//10:  SRV-Bloom Down Sampled(UAV(COPY)->SRV)
		//11:  UAV-Bloom Down Sampled Intermediate Buffer(UAV)
		//12:  SRV-Bloom Down Sampled Intermediate Buffer(UAV(COPY)->SRV)
		//13:  SRV-Debug Font
		//-----PerObject-----
		//14:  SRV-PerObject Texture Begin Slot
		CDescriptorHeapWrapper				m_cbvsrvHeap;


		DXGI_FORMAT							m_ShadowMapFormat = DXGI_FORMAT_D32_FLOAT;
		const UINT m_ShadowMapWidth = 2048;
		const UINT m_ShadowMapHeight = 2048;


	};

}
