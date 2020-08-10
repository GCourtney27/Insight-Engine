#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Renderer.h"

#include "Platform/Windows/Error/COM_Exception.h"
#include "Platform/Windows/DirectX_12/D3D12_Helper.h"
#include "Platform/Windows/DirectX_12/Descriptor_Heap_Wrapper.h"
#include "Platform/Windows/DirectX_Shared/Constant_Buffer_Types.h"
#include "Platform/Windows/DirectX_12/Ray_Tracing/Ray_Trace_Helpers.h"

/*
	Render context for Windows DirectX 12 API. Currently Deferred shading is the only pipeline supported.
	TODO: Add forward rendering as optional shading technique
*/

using Microsoft::WRL::ComPtr;

namespace Insight {

	class WindowsWindow;
	class GeometryManager;
	class ieD3D12SphereRenderer;

	class ScreenQuad
	{
	public:
		void Init();
		void OnRender(ComPtr<ID3D12GraphicsCommandList> commandList);

	private:
		ComPtr<ID3D12Resource> m_VertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
		UINT m_NumVerticies = 0u;
	};

	class INSIGHT_API Direct3D12Context : public Renderer
	{
		friend class Renderer;
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

		inline ID3D12Device& GetDeviceContext() const { return *m_pDevice.Get(); }

		inline ID3D12GraphicsCommandList& GetScenePassCommandList() const { return *m_pScenePass_CommandList.Get(); }
		inline ID3D12GraphicsCommandList& GetPostProcessPassCommandList() const { return *m_pPostEffectsPass_CommandList.Get(); }
		inline ID3D12GraphicsCommandList& GetShadowPassCommandList() const { return *m_pShadowPass_CommandList.Get(); }
		inline ID3D12GraphicsCommandList& GetTransparencyPassCommandList() const { return *m_pTransparencyPass_CommandList.Get(); }

		inline ID3D12CommandQueue& GetCommandQueue() const { return *m_pCommandQueue.Get(); }
		inline CDescriptorHeapWrapper& GetCBVSRVDescriptorHeap() { return m_cbvsrvHeap; }
		inline ID3D12Resource& GetConstantBufferPerObjectUploadHeap() const { return *m_PerObjectCBV[m_FrameIndex].Get(); }
		inline UINT8& GetPerObjectCBVGPUHeapAddress() { return *m_cbvPerObjectGPUAddress[m_FrameIndex]; }

		inline ID3D12Resource& GetConstantBufferPerObjectMaterialUploadHeap() const { return *m_PerObjectMaterialAdditivesCBV[m_FrameIndex].Get(); }
		inline UINT8& GetPerObjectMaterialAdditiveCBVGPUHeapAddress() { return *m_cbvPerObjectMaterialOverridesGPUAddress[m_FrameIndex]; }

		const CB_PS_VS_PerFrame& GetPerFrameCB() const { return m_PerFrameData; }

		// Ray Tracing
		// -----------
		ID3D12Resource* GetRayTracingSRV() const { return m_RayTraceOutput_SRV.Get(); }
		uint32_t RegisterGeometryWithRTAccelerationStucture(ComPtr<ID3D12Resource> pVertexBuffer, ComPtr<ID3D12Resource> pIndexBuffer, uint32_t NumVerticies, uint32_t NumIndices, DirectX::XMMATRIX MeshWorldMat);
		void UpdateRTAccelerationStructureMatrix(uint32_t InstanceArrIndex, DirectX::XMMATRIX NewWorldMat) { m_RTHelper.UpdateInstanceTransformByIndex(InstanceArrIndex, NewWorldMat); }

		ID3D12Resource* GetRenderTarget() const { return m_pRenderTargets[m_FrameIndex].Get(); }
		const unsigned int GetNumRTVs() const { return m_NumRTV; }
		inline D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle;
			handle.ptr = m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + m_RTVDescriptorSize * m_FrameIndex;
			return handle;
		}

		
	private:
		Direct3D12Context(WindowsWindow* windowHandle);
		virtual ~Direct3D12Context();

		void CloseCommandListAndSignalCommandQueue();
		// Per-Frame
		
		void MoveToNextFrame();
		void BindShadowPass();
		void BindGeometryPass(bool setPSO = false);
		void BindLightingPass();
		void BindSkyPass();
		void BindTransparencyPass();
		void BindRayTracePass();
		void BindPostFxPass();

		// D3D12 Initialize
		
		void CreateDXGIFactory();
		void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter);
		void CreateDevice();
		void CreateCommandQueue();
		void CreateSwapChain();
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
		void CreateFenceEvent();
		void CreateConstantBuffers();
		void CreateViewport();
		void CreateScissorRect();
		void CreateScreenQuad();
		
		// Close GPU handle and release resources for the graphics context.
		void Cleanup();
		// Once called CPU will wait for the GPU to finish executing any pending work.
		void WaitForGPU();
		// Resize render targets and depth stencil. Usually called from 'OnWindowResize'.
		void UpdateSizeDependentResources();
		// Update view and scissor rects to new window width and height.
		void UpdateViewAndScissor();

		// Load any demo assets for debugging. Usually can be left empty
		void LoadDemoAssets();

	private:
		HWND*				m_pWindowHandle = nullptr;
		WindowsWindow*		m_pWindow = nullptr;
		D3D12Helper			m_d3dDeviceResources;
		ieD3D12SphereRenderer*				m_SkySphere;
		RayTraceHelpers						m_RTHelper;

		// CPU/GPU Syncronization
		int						m_FrameIndex = 0;
		UINT64					m_FenceValues[m_FrameBufferCount] = {};
		HANDLE					m_FenceEvent = {};
		ComPtr<ID3D12Fence>		m_pFence;

		static const UINT		m_NumRTV = 5;
		bool		m_WindowResizeComplete = true;
		bool		m_IsRayTraceEnabled = true;
		bool		m_IsRayTraceSupported = false; // Assume ray tracing is not supported on the GPU
		bool		m_UseWarpDevice = false;
		int			m_RtvDescriptorIncrementSize = 0;

		// D3D 12 Usings
		ComPtr<IDXGIAdapter1>				m_pAdapter;
		ComPtr<ID3D12Device>				m_pDevice;
		ComPtr<IDXGIFactory4>				m_pDxgiFactory;
		ComPtr<IDXGISwapChain3>				m_pSwapChain;

		ComPtr<ID3D12GraphicsCommandList>	m_pActiveCommandList;
		ComPtr<ID3D12CommandQueue>			m_pCommandQueue;

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

		ComPtr<ID3D12Resource>				m_pRenderTargetTextures[m_NumRTV];
		ComPtr<ID3D12Resource>				m_pRenderTargetTextures_PostFxPass[m_FrameBufferCount];
		ComPtr<ID3D12Resource>				m_pRenderTargets[m_FrameBufferCount];
		
		//-----Light Pass-----
		// 0: Albedo
		// 1: Normal
		// 2: Roughness/Metallic/AO
		// 3: Position
		// -----Post-Fx Pass-----
		// 4: Light Pass result
		CDescriptorHeapWrapper				m_rtvHeap;
		ComPtr<ID3D12DescriptorHeap>		m_RTVDescriptorHeap;
		UINT								m_RTVDescriptorSize;

		ComPtr<ID3D12Resource>				m_pDepthStencilTexture;
		ComPtr<ID3D12Resource>				m_pShadowDepthTexture;

		//0:  SceneDepth
		//1:  ShadowDepth
		CDescriptorHeapWrapper				m_dsvHeap;

		ComPtr<ID3D12RootSignature>			m_pDeferredShadingPass_RootSignature;
		ComPtr<ID3D12RootSignature>			m_pForwardShadingPass_RootSignature;

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
		//9:  SRV-Normal(SRV)
		//10:  SRV-Roughness(SRV)
		//11:  SRV-Metallic(SRV)
		//12:  SRV-AO(SRV)
		//13:  SRV-Sky Irradiance(SRV)
		//14:  SRV-Sky Environment(SRV)
		//15:  SRV-Sky BRDF LUT(SRV)
		//15:  SRV-Sky Diffuse(SRV)
		CDescriptorHeapWrapper				m_cbvsrvHeap;

		ScreenQuad							m_ScreenQuad;
		D3D12_VIEWPORT						m_ScenePassViewPort = {};
		D3D12_VIEWPORT						m_ShadowPassViewPort = {};
		D3D12_RECT							m_ScenePassScissorRect = {};
		D3D12_RECT							m_ShadowPassScissorRect = {};

		DXGI_SAMPLE_DESC					m_SampleDesc = {};
		D3D12_DEPTH_STENCIL_VIEW_DESC		m_ScenePassDsvDesc = {};
		float								m_ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
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

		ComPtr<ID3D12Resource>	m_RayTraceOutput_SRV;

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
