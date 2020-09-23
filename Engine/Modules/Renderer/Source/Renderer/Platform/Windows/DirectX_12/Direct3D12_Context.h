#pragma once

#include <Insight/Core.h>

#include "Renderer/Renderer.h"

#include "Platform/Windows/Error/COM_Exception.h"
#include "Renderer/Platform/Windows/DirectX_12/D3D12_Helper.h"
#include "Renderer/Platform/Windows/DirectX_12/Wrappers/Descriptor_Heap_Wrapper.h"
#include "Renderer/Platform/Windows/DirectX_Shared/Constant_Buffer_Types.h"
#include "Renderer/Platform/Windows/DirectX_12/Ray_Tracing/Ray_Trace_Helpers.h"
#include "Renderer/Platform/Windows/DirectX_12/Wrappers/ie_D3D12_Screen_Quad.h"

#include "Insight/Rendering/Lighting/ADirectional_Light.h"
#include "Renderer/Platform/Windows/DirectX_12/Wrappers/D3D12_Constant_Buffer_Wrapper.h"

/*
	Render context for Windows Direct3D 12 API. 
*/

using Microsoft::WRL::ComPtr;

#define IE_D3D12_FrameIndex m_d3dDeviceResources.GetFrameIndex()

namespace Insight {

	class WindowsWindow;
	class GeometryManager;
	class ieD3D12SphereRenderer;

	typedef ID3D12Resource D3D12Texture2D;
	typedef ID3D12Resource D3D12RenderTargetView;
	typedef ID3D12Resource D3D12ShaderResourceView;
	typedef ID3D12Resource D3D12UnoreredAccessView;

	class INSIGHT_API Direct3D12Context : public Renderer
	{
	private:
		
		/*struct RenderTargetTexture
		{
			void Create(CDescriptorHeapWrapper &rtHeap)
			{

			}
			RenderTarget(){}
			D3D12Texture2D* RenderTargetTexture;
			D3D12RenderTargetView* RenderTargetView;
			D3D12RenderTargetView* ShaderResourceView;
			D3D12UnoreredAccessView* UnorderedAccessView;
		};
		const static UINT NumDummyRTs = 6;
		RenderTargetTexture RenderTargets[NumDummyRTs];
		void DummyCreateRenderTargets()
		{
			CDescriptorHeapWrapper rtHeap;
			rtHeap.Create(&m_d3dDeviceResources.GetDeviceContext(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, NumDummyRTs, false);

			for (size_t i = 0; i < NumDummyRTs; i++)
			{
				RenderTargets[i].Create(rtHeap);
			}
		}*/
	public:
		friend class Renderer;
		friend class D3D12Helper;
	public:
		virtual bool Init_Impl() override;
		virtual void Destroy_Impl() override;
		virtual bool PostInit_Impl() override;
		virtual void OnUpdate_Impl(const float DeltaMs) override;
		virtual void OnPreFrameRender_Impl() override;
		virtual void OnRender_Impl() override;
		virtual void OnMidFrameRender_Impl() override;
		virtual void ExecuteDraw_Impl() override;
		virtual void SwapBuffers_Impl() override;
		virtual void OnWindowResize_Impl() override;
		virtual void OnWindowFullScreen_Impl() override;
		virtual void OnShaderReload_Impl() override;

		virtual void SetVertexBuffers_Impl(uint32_t StartSlot, uint32_t NumBuffers, ieVertexBuffer* pBuffers) override;
		virtual void SetIndexBuffer_Impl(ieIndexBuffer* pBuffer) override;
		virtual void DrawIndexedInstanced_Impl(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation) override;

		virtual void RenderSkySphere_Impl() override;
		virtual bool CreateSkybox_Impl() override;
		virtual void DestroySkybox_Impl() override;

		WindowsWindow& GetWindowRef() const { return *m_pWindowRef; }

		inline ID3D12Device& GetDeviceContext() const { return m_d3dDeviceResources.GetDeviceContext(); }

		inline ID3D12GraphicsCommandList& GetScenePassCommandList() const { return *m_pScenePass_CommandList.Get(); }
		inline ID3D12GraphicsCommandList& GetPostProcessPassCommandList() const { return *m_pPostEffectsPass_CommandList.Get(); }
		inline ID3D12GraphicsCommandList& GetShadowPassCommandList() const { return *m_pShadowPass_CommandList.Get(); }
		inline ID3D12GraphicsCommandList& GetTransparencyPassCommandList() const { return *m_pTransparencyPass_CommandList.Get(); }

		inline ID3D12CommandQueue& GetCommandQueue() const { return m_d3dDeviceResources.GetGraphicsCommandQueue(); }
		inline CDescriptorHeapWrapper& GetCBVSRVDescriptorHeap() { return m_cbvsrvHeap; }
		
		inline ID3D12Resource& GetConstantBufferPerObjectUploadHeap() const { return *m_CBPerObject[IE_D3D12_FrameIndex].GetResource(); }
		inline UINT8* GetPerObjectCBVGPUHeapAddress() { return m_CBPerObject[IE_D3D12_FrameIndex].GetGPUAddress(); }
		inline ID3D12Resource& GetConstantBufferPerObjectMaterialUploadHeap() const { return *m_CBPerObjectMaterial[IE_D3D12_FrameIndex].GetResource(); }
		inline UINT8* GetPerObjectMaterialAdditiveCBVGPUHeapAddress() { return m_CBPerObjectMaterial[IE_D3D12_FrameIndex].GetGPUAddress(); }

		const CB_PS_VS_PerFrame& GetPerFrameCB() const { return m_CBPerFrame.Data; }

		// Ray Tracing
		// -----------
		inline ID3D12GraphicsCommandList4& GetRayTracePassCommandList() const { return *m_pRayTracePass_CommandList.Get(); }
		ID3D12Resource* GetRayTracingSRV() const { return m_RayTraceOutput_SRV.Get(); }
		[[nodiscard]] uint32_t RegisterGeometryWithRTAccelerationStucture(ComPtr<ID3D12Resource> pVertexBuffer, ComPtr<ID3D12Resource> pIndexBuffer, uint32_t NumVerticies, uint32_t NumIndices, DirectX::XMMATRIX MeshWorldMat);
		void UpdateRTAccelerationStructureMatrix(uint32_t InstanceArrIndex, DirectX::XMMATRIX NewWorldMat) { m_RTHelper.UpdateInstanceTransformByIndex(InstanceArrIndex, NewWorldMat); }

		ID3D12Resource* GetSwapChainRenderTarget() const { return m_pSwapChainRenderTargets[IE_D3D12_FrameIndex].Get(); }
		const UINT GetNumLightPassRTVs() const { return m_NumRTVs; }
		inline D3D12_CPU_DESCRIPTOR_HANDLE GetSwapChainRTV() const
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle;
			handle.ptr = m_SwapChainRTVHeap.hCPUHeapStart.ptr + m_SwapChainRTVHeap.HandleIncrementSize * IE_D3D12_FrameIndex;
			return handle;
		}

		
	private:
		Direct3D12Context(WindowsWindow* windowHandle);
		virtual ~Direct3D12Context();

		void CloseCommandListAndSignalCommandQueue();

		// Per-Frame
		
		void BindShadowPass();
		void BindGeometryPass();
		void BindLightingPass();
		void BindSkyPass();
		void BindTransparencyPass();
		void BindRayTracePass();
		void BindPostFxPass();
		void BlurBloomBuffer();

		// D3D12 Initialize
		
		void CreateSwapChainRTVDescriptorHeap();

		// Create app resources
		
		void CreateDSVs();
		void CreateRTVs();
		void CreateCBVs();
		void CreateSRVs();
		void CreateDeferredShadingRS();
		void CreateForwardShadingRS();
		// Texture down-sample and Gaussian Blur pipelines share the same shader inputs for the Bloom Pass.
		void CreateBloomPassRS();

		void CreateShadowPassPSO();
		void CreateGeometryPassPSO();
		void CreateSkyPassPSO();
		void CreateTransparencyPassPSO();
		void CreateLightPassPSO();
		void CreatePostEffectsPassPSO();
		void CreateDownSamplePSO();
		void CreateGaussianBlurPSO();

		// Create window resources
		
		void CreateCommandAllocators();
		void CreateViewport();
		void CreateScissorRect();
		void CreateScreenQuad();
		
		// Close GPU handle and release resources for the D3D 12 context.
		void InternalCleanup();
		// Resize render targets and depth stencil. Usually called from 'OnWindowResize'.
		void UpdateSizeDependentResources();

		void ResourceBarrier(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pResource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);
		
	private:
		WindowsWindow*		m_pWindowRef = nullptr;
		D3D12Helper			m_d3dDeviceResources;
		RayTraceHelpers		m_RTHelper;

		ieD3D12ScreenQuad	m_ScreenQuad;
		D3D12_VIEWPORT		m_ShadowPass_ViewPort = {};
		D3D12_RECT			m_ShadowPass_ScissorRect = {};

		ieD3D12SphereRenderer*	m_pSkySphere_Geometry;

		static const UINT	m_NumRTVs = 6;
		bool				m_WindowResizeComplete = true;
		bool				m_UseWarpDevice = false;

		// D3D 12 Usings
		ComPtr<ID3D12GraphicsCommandList>	m_pActiveCommandList;

		ComPtr<ID3D12GraphicsCommandList4>	m_pRayTracePass_CommandList;
		ComPtr<ID3D12CommandAllocator>		m_pRayTracePass_CommandAllocators[m_FrameBufferCount];
		ComPtr<ID3D12GraphicsCommandList>	m_pShadowPass_CommandList;
		ComPtr<ID3D12CommandAllocator>		m_pShadowPass_CommandAllocators[m_FrameBufferCount];
		ComPtr<ID3D12GraphicsCommandList>	m_pScenePass_CommandList;
		ComPtr<ID3D12CommandAllocator>		m_pScenePass_CommandAllocators[m_FrameBufferCount];
		ComPtr<ID3D12GraphicsCommandList>	m_pTransparencyPass_CommandList;
		ComPtr<ID3D12CommandAllocator>		m_pTransparencyPass_CommandAllocators[m_FrameBufferCount];
		ComPtr<ID3D12GraphicsCommandList>	m_pPostEffectsPass_CommandList;
		ComPtr<ID3D12CommandAllocator>		m_pPostEffectsPass_CommandAllocators[m_FrameBufferCount];
		ComPtr<ID3D12GraphicsCommandList>	m_pDownSample_CommandList;
		ComPtr<ID3D12CommandAllocator>		m_pDownSample_CommandAllocators[m_FrameBufferCount];

		ComPtr<ID3D12Resource>				m_pRenderTargetTextures[m_NumRTVs];
		ComPtr<ID3D12Resource>				m_pSwapChainRenderTargets[m_FrameBufferCount];
		
		//-----Light Pass-----
		// 0: Albedo
		// 1: Normal
		// 2: (R)Roughness/(G)Metallic/(B)AO
		// 3: World Position
		// -----Post-Fx Pass-----
		// 4: Light Pass result
		// 5: Bloom Buffer
		CDescriptorHeapWrapper				m_rtvHeap;
		// Number of decriptors depends on frame buffer count. Start slot is 0.
		CDescriptorHeapWrapper				m_SwapChainRTVHeap;
		//0:  SceneDepth
		//1:  ShadowDepth
		CDescriptorHeapWrapper				m_dsvHeap;

		ComPtr<ID3D12Resource>				m_pSceneDepthStencilTexture;
		ComPtr<ID3D12Resource>				m_pShadowDepthTexture;
		ComPtr<ID3D12Resource>				m_RayTraceOutput_SRV;

		ComPtr<ID3D12Resource>				m_pBloomBlurResult_UAV;
		ComPtr<ID3D12Resource>				m_pBloomBlurResult_SRV;
		ComPtr<ID3D12Resource>				m_pBloomBlurIntermediateBuffer_UAV;
		ComPtr<ID3D12Resource>				m_pBloomBlurIntermediateBuffer_SRV;


		ComPtr<ID3D12RootSignature>			m_pDeferredShadingPass_RS;
		ComPtr<ID3D12RootSignature>			m_pForwardShadingPass_RS;
		ComPtr<ID3D12RootSignature>			m_pBloomPass_RS;

		ComPtr<ID3D12PipelineState>			m_pShadowPass_PSO;
		ComPtr<ID3D12PipelineState>			m_pGeometryPass_PSO;
		ComPtr<ID3D12PipelineState>			m_pLightingPass_PSO;
		ComPtr<ID3D12PipelineState>			m_pSkyPass_PSO;
		ComPtr<ID3D12PipelineState>			m_pTransparency_PSO;
		ComPtr<ID3D12PipelineState>			m_pPostFxPass_PSO;
		ComPtr<ID3D12PipelineState>			m_pThresholdDownSample_PSO;
		ComPtr<ID3D12PipelineState>			m_pGaussianBlur_PSO;

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
		//-----PerObject-----
		//13:  SRV-Albedo(SRV)
		//14:  SRV-Normal(SRV)
		//15:  SRV-Roughness(SRV)
		//16:  SRV-Metallic(SRV)
		//17:  SRV-AO(SRV)
		//18:  SRV-Sky Irradiance(SRV)
		//19:  SRV-Sky Environment(SRV)
		//20:  SRV-Sky BRDF LUT(SRV)
		//21:  SRV-Sky Diffuse(SRV)
		CDescriptorHeapWrapper				m_cbvsrvHeap;


		DXGI_SAMPLE_DESC					m_SampleDesc = {};
		D3D12_DEPTH_STENCIL_VIEW_DESC		m_ScenePass_DsvDesc = {};
		float								m_ScreenClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		DXGI_FORMAT							m_DsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		DXGI_FORMAT							m_RtvFormat[6] = { 
												DXGI_FORMAT_R11G11B10_FLOAT,	// Albedo buffer
												DXGI_FORMAT_R16G16B16A16_SNORM,	// Normal
												DXGI_FORMAT_R11G11B10_FLOAT,	// (R)Roughness/(G)Metallic/(B)AO
												DXGI_FORMAT_R32G32B32A32_FLOAT, // Position
												DXGI_FORMAT_R11G11B10_FLOAT,	// Light Pass result
												DXGI_FORMAT_R11G11B10_FLOAT,	// Bloom buffer
											};
		float								m_DepthClearValue = 1.0f;
		DXGI_FORMAT							m_ShadowMapFormat = DXGI_FORMAT_D32_FLOAT;

		const UINT m_ShadowMapWidth = 1024U;
		const UINT m_ShadowMapHeight = 1024U;

		// Constant Buffers
		
		ieD3D12ConstantBuffer<CB_CS_DownSampleParams>	m_CBDownSampleParams;
		ieD3D12ConstantBuffer<CB_CS_BlurParams>			m_CBBlurParams;
		ieD3D12ConstantBuffer<CB_PS_Lights>				m_CBLights;
		ieD3D12ConstantBuffer<CB_PS_PostFx>				m_CBPostFx;
		ieD3D12ConstantBuffer<CB_PS_VS_PerFrame>		m_CBPerFrame;
		ieD3D12ConstantBuffer<CB_VS_PerObject>			m_CBPerObject[m_FrameBufferCount];
		ieD3D12ConstantBuffer<CB_PS_VS_PerObjectMaterialAdditives> m_CBPerObjectMaterial[m_FrameBufferCount];

	};

}
