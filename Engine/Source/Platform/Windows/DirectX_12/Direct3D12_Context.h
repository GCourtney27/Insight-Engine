#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Renderer.h"

#include "Platform/Windows/Error/COM_Exception.h"
#include "Platform/Windows/DirectX_12/D3D12_Helper.h"
#include "Platform/Windows/DirectX_12/Descriptor_Heap_Wrapper.h"
#include "Platform/Windows/DirectX_Shared/Constant_Buffer_Types.h"
#include "Platform/Windows/DirectX_12/Ray_Tracing/Ray_Trace_Helpers.h"
#include "Platform/Windows/DirectX_12/ie_D3D12_Screen_Quad.h"

/*
	Render context for Windows DirectX 12 API. 
*/

using Microsoft::WRL::ComPtr;

#define IE_D3D12_FrameIndex m_d3dDeviceResources.GetFrameIndex()

namespace Insight {

	class WindowsWindow;
	class GeometryManager;
	class ieD3D12SphereRenderer;

	class INSIGHT_API Direct3D12Context : public Renderer
	{
	public:
		friend class Renderer;
		friend class D3D12Helper;
	public:
		virtual bool Init_Impl() override;
		virtual void Destroy_Impl();
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

		inline ID3D12CommandQueue& GetCommandQueue() const { return m_d3dDeviceResources.GetCommandQueue(); }
		inline CDescriptorHeapWrapper& GetCBVSRVDescriptorHeap() { return m_cbvsrvHeap; }
		inline ID3D12Resource& GetConstantBufferPerObjectUploadHeap() const { return *m_PerObjectCBV[IE_D3D12_FrameIndex].Get(); }
		inline UINT8& GetPerObjectCBVGPUHeapAddress() { return *m_cbvPerObjectGPUAddress[IE_D3D12_FrameIndex]; }

		inline ID3D12Resource& GetConstantBufferPerObjectMaterialUploadHeap() const { return *m_PerObjectMaterialAdditivesCBV[IE_D3D12_FrameIndex].Get(); }
		inline UINT8& GetPerObjectMaterialAdditiveCBVGPUHeapAddress() { return *m_cbvPerObjectMaterialOverridesGPUAddress[IE_D3D12_FrameIndex]; }

		const CB_PS_VS_PerFrame& GetPerFrameCB() const { return m_PerFrameData; }

		// Ray Tracing
		// -----------
		inline ID3D12GraphicsCommandList4& GetRayTracePassCommandList() const { return *m_pRayTracePass_CommandList.Get(); }
		ID3D12Resource* GetRayTracingSRV() const { return m_RayTraceOutput_SRV.Get(); }
		uint32_t RegisterGeometryWithRTAccelerationStucture(ComPtr<ID3D12Resource> pVertexBuffer, ComPtr<ID3D12Resource> pIndexBuffer, uint32_t NumVerticies, uint32_t NumIndices, DirectX::XMMATRIX MeshWorldMat);
		void UpdateRTAccelerationStructureMatrix(uint32_t InstanceArrIndex, DirectX::XMMATRIX NewWorldMat) { m_RTHelper.UpdateInstanceTransformByIndex(InstanceArrIndex, NewWorldMat); }

		ID3D12Resource* GetSwapChainRenderTarget() const { return m_pRenderTargets[IE_D3D12_FrameIndex].Get(); }
		const unsigned int GetNumLightPassRTVs() const { return m_NumRenderTargetViews; }
		inline D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle;
			handle.ptr = m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + m_RTVDescriptorSize * IE_D3D12_FrameIndex;
			return handle;
		}

		
	private:
		Direct3D12Context(WindowsWindow* windowHandle);
		virtual ~Direct3D12Context();

		void CloseCommandListAndSignalCommandQueue();
		
		// Threading

		void LoadContexts();
		void WorkerThread(uint8_t ThreadIndex);

		// Per-Frame
		
		void BindShadowPass();
		void BindGeometryPass();
		void BindLightingPass();
		void BindSkyPass();
		void BindTransparencyPass();
		void BindRayTracePass();
		void BindPostFxPass();

		// D3D12 Initialize
		
		void CreateRenderTargetViewDescriptorHeap();

		// Create app resources
		
		void CreateDSVs();
		void CreateRTVs();
		void CreateCBVs();
		void CreateSRVs();
		void CreateDeferredShadingRootSignature();
		void CreateForwardShadingRootSignature();

		void CreateShadowPassPSO();
		void CreateGeometryPassPSO();
		void CreateSkyPassPSO();
		void CreateTransparencyPassPSO();
		void CreateLightPassPSO();
		void CreatePostEffectsPassPSO();

		// Create window resources
		
		void CreateCommandAllocators();
		void CreateConstantBuffers();
		void CreateViewport();
		void CreateScissorRect();
		void CreateScreenQuad();
		
		// Close GPU handle and release resources for the D3D 12 context.
		void Cleanup();
		// Resize render targets and depth stencil. Usually called from 'OnWindowResize'.
		void UpdateSizeDependentResources();

		void ResourceBarrier(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pResource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);
	private:
		WindowsWindow*		m_pWindowRef = nullptr;
		D3D12Helper			m_d3dDeviceResources;
		RayTraceHelpers		m_RTHelper;

		ieD3D12SphereRenderer*	m_pSkySphere_Geometry;

		// Threading
		HANDLE m_WorkerThreadPreFrameRender[s_NumRenderContexts];
		HANDLE m_WorkerThreadRender[s_NumRenderContexts];
		HANDLE m_WorkerThreadExecuteDraw[s_NumRenderContexts];
		struct ThreadParameter
		{
			uint8_t ThreadIndex;
		};
		ThreadParameter m_ThreadParameters[s_NumRenderContexts];

		static const UINT	m_NumRenderTargetViews = 5;
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

		ComPtr<ID3D12Resource>				m_pRenderTargetTextures[m_NumRenderTargetViews];
		ComPtr<ID3D12Resource>				m_pRenderTargetTextures_PostFxPass[m_FrameBufferCount];
		ComPtr<ID3D12Resource>				m_pRenderTargets[m_FrameBufferCount];
		
		//-----Light Pass-----
		// 0: Albedo
		// 1: Normal
		// 2: (R)Roughness/(G)Metallic/(B)AO
		// 3: World Position
		// -----Post-Fx Pass-----
		// 4: Light Pass result
		CDescriptorHeapWrapper				m_rtvHeap;
		ComPtr<ID3D12DescriptorHeap>		m_RTVDescriptorHeap;
		UINT								m_RTVDescriptorSize;

		ComPtr<ID3D12Resource>				m_pSceneDepthStencilTexture;
		ComPtr<ID3D12Resource>				m_pShadowDepthTexture;
		ComPtr<ID3D12Resource>				m_RayTraceOutput_SRV;

		//0:  SceneDepth
		//1:  ShadowDepth
		CDescriptorHeapWrapper				m_dsvHeap;

		ComPtr<ID3D12RootSignature>			m_pDeferredShadingPass_RS;
		ComPtr<ID3D12RootSignature>			m_pForwardShadingPass_RS;

		ComPtr<ID3D12PipelineState>			m_pShadowPass_PSO;
		ComPtr<ID3D12PipelineState>			m_pGeometryPass_PSO;
		ComPtr<ID3D12PipelineState>			m_pLightingPass_PSO;
		ComPtr<ID3D12PipelineState>			m_pSkyPass_PSO;
		ComPtr<ID3D12PipelineState>			m_pTransparency_PSO;
		ComPtr<ID3D12PipelineState>			m_pPostFxPass_PSO;

		//-----Pipeline-----
		//0:   SRV-Albedo(RTV->SRV)
		//1:   SRV-Normal(RTV->SRV)
		//2:   SRV-(R)Roughness/(G)Metallic/(B)AO(RTV->SRV)
		//3:   SRV-Position(RTV->SRV)
		//4:   SRV-Scene Depth(DSV->SRV)
		//5:   SRV-Light Pass Result(RTV->SRV)
		//6:   SAV-Ray Trace Output(RTHelper UAV(COPY)->SRV)
		//7:   SRV-Shadow Depth(DSV->SRV)
		//-----PerObject-----
		//8:   SRV-Albedo(SRV)
		//9:   SRV-Normal(SRV)
		//10:  SRV-Roughness(SRV)
		//11:  SRV-Metallic(SRV)
		//12:  SRV-AO(SRV)
		//13:  SRV-Sky Irradiance(SRV)
		//14:  SRV-Sky Environment(SRV)
		//15:  SRV-Sky BRDF LUT(SRV)
		//15:  SRV-Sky Diffuse(SRV)
		CDescriptorHeapWrapper				m_cbvsrvHeap;

		ieD3D12ScreenQuad					m_ScreenQuad;
		D3D12_VIEWPORT						m_ShadowPass_ViewPort = {};
		D3D12_RECT							m_ShadowPass_ScissorRect = {};

		DXGI_SAMPLE_DESC					m_SampleDesc = {};
		D3D12_DEPTH_STENCIL_VIEW_DESC		m_ScenePass_DsvDesc = {};
		float								m_ScreenClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		DXGI_FORMAT							m_DsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		DXGI_FORMAT							m_RtvFormat[5] = { 
												DXGI_FORMAT_R11G11B10_FLOAT,	// Albedo buffer
												DXGI_FORMAT_R8G8B8A8_SNORM,		// Normal
												DXGI_FORMAT_R11G11B10_FLOAT,	// (R)Roughness/(G)Metallic/(B)AO
												DXGI_FORMAT_R32G32B32A32_FLOAT, // Position
												DXGI_FORMAT_R11G11B10_FLOAT,	// Light Pass result
											};
		float								m_DepthClearValue = 1.0f;
		DXGI_FORMAT							m_ShadowMapFormat = DXGI_FORMAT_D32_FLOAT;

		const UINT m_ShadowMapWidth = 1024U;
		const UINT m_ShadowMapHeight = 1024U;


		ComPtr<ID3D12Resource>	m_LightCBV;
		UINT8*					m_cbvLightBufferGPUAddress;

		ComPtr<ID3D12Resource>	m_PerObjectCBV[m_FrameBufferCount];
		UINT8*					m_cbvPerObjectGPUAddress[m_FrameBufferCount];

		ComPtr<ID3D12Resource>	m_PerObjectMaterialAdditivesCBV[m_FrameBufferCount];
		UINT8*					m_cbvPerObjectMaterialOverridesGPUAddress[m_FrameBufferCount];

		ComPtr<ID3D12Resource> m_PerFrameCBV;
		UINT8*				   m_cbvPerFrameGPUAddress;
		CB_PS_VS_PerFrame	   m_PerFrameData;

		ComPtr<ID3D12Resource> m_PostFxCBV;
		UINT8*				   m_cbvPostFxGPUAddress;
		CB_PS_VS_PerFrame	   m_PostFxData;
		int CBPerFrameAlignedSize = (sizeof(CB_PS_VS_PerFrame) + 255) & ~255;
		int CBPostFxAlignedSize = (sizeof(CB_PS_PostFx) + 255) & ~255;

		#define POINT_LIGHTS_CB_ALIGNED_OFFSET (0)
		int	CBPointLightsAlignedSize = (sizeof(CB_PS_PointLight) + 255) & ~255;

		#define DIRECTIONAL_LIGHTS_CB_ALIGNED_OFFSET (MAX_POINT_LIGHTS_SUPPORTED * sizeof(CB_PS_PointLight))
		int	CBDirectionalLightsAlignedSize = (sizeof(CB_PS_DirectionalLight) + 255) & ~255;

		#define SPOT_LIGHTS_CB_ALIGNED_OFFSET (MAX_POINT_LIGHTS_SUPPORTED * sizeof(CB_PS_PointLight) + MAX_DIRECTIONAL_LIGHTS_SUPPORTED * sizeof(CB_PS_DirectionalLight))
		int	CBSpotLightsAlignedSize = (sizeof(CB_PS_SpotLight) + 255) & ~255;

	};

}
