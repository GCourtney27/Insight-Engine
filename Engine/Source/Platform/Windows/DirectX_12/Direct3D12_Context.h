#pragma once

#include <Insight/Core.h>

#include "Platform/Windows/DirectX_12/D3D12_Helper.h"
#include "Insight/Rendering/Renderer.h"
#include "Platform/Windows/Error/COM_Exception.h"

#include "Platform/Windows/DirectX_12/Descriptor_Heap_Wrapper.h"
#include "Platform/Windows/DirectX_Shared/Constant_Buffer_Types.h"

/*
	Render context for Windows DirectX 12 API. Currently Deferred shading is the only pipeline supported.
	TODO: Add Transparency forward pass.
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
		virtual bool InitImpl() override;
		virtual void DestroyImpl();
		virtual bool PostInitImpl() override;
		virtual void OnUpdateImpl(const float DeltaMs) override;
		virtual void OnPreFrameRenderImpl() override;
		virtual void OnRenderImpl() override;
		virtual void OnMidFrameRenderImpl() override;
		virtual void ExecuteDrawImpl() override;
		virtual void SwapBuffersImpl() override;
		virtual void OnWindowResizeImpl() override;
		virtual void OnWindowFullScreenImpl() override;

		virtual void SetVertexBuffersImpl(uint32_t StartSlot, uint32_t NumBuffers, ieVertexBuffer* pBuffers) override;
		virtual void SetIndexBufferImpl(ieIndexBuffer* pBuffer) override;
		virtual void DrawIndexedInstancedImpl(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation) override;

		virtual void RenderSkySphereImpl() override;
		virtual bool CreateSkyboxImpl() override;
		virtual void DestroySkyboxImpl() override;

		inline ID3D12Device& GetDeviceContext() const { return *m_pDevice.Get(); }

		inline ID3D12GraphicsCommandList& GetScenePassCommandList() const { return *m_pScenePassCommandList.Get(); }
		inline ID3D12GraphicsCommandList& GetShadowPassCommandList() const { return *m_pShadowPassCommandList.Get(); }
		inline ID3D12CommandQueue& GetCommandQueue() const { return *m_pCommandQueue.Get(); }
		inline CDescriptorHeapWrapper& GetCBVSRVDescriptorHeap() { return m_cbvsrvHeap; }
		inline ID3D12Resource& GetConstantBufferPerObjectUploadHeap() const { return *m_PerObjectCBV[m_FrameIndex].Get(); }
		inline UINT8& GetPerObjectCBVGPUHeapAddress() { return *m_cbvPerObjectGPUAddress[m_FrameIndex]; }

		inline ID3D12Resource& GetConstantBufferPerObjectMaterialUploadHeap() const { return *m_PerObjectMaterialAdditivesCBV[m_FrameIndex].Get(); }
		inline UINT8& GetPerObjectMaterialAdditiveCBVGPUHeapAddress() { return *m_cbvPerObjectMaterialOverridesGPUAddress[m_FrameIndex]; }

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
		void CreateConstantBufferViews();
		void CreateDeferredShadingRootSignature();
		void CreateForwardShadingRootSignature();
		void CreateShadowPassPSO();
		void CreateGeometryPassPSO();
		void CreateSkyPassPSO();
		void CreateTransparencyPassPSO();
		void CreateLightPassPSO();
		void CreatePostFxPassPSO();

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
		// CPU/GPU Syncronization
		int						m_FrameIndex = 0;
		UINT64					m_FenceValues[m_FrameBufferCount] = {};
		HANDLE					m_FenceEvent = {};
		ComPtr<ID3D12Fence>		m_pFence;
		static const UINT		m_NumRTV = 5;

		bool		m_WindowResizeComplete = true;
		bool		m_RayTraceEnabled = false;
		bool		m_UseWarpDevice = false;
		int			m_RtvDescriptorIncrementSize = 0;

		// D3D 12 Usings
		ComPtr<IDXGIAdapter1>				m_pAdapter;
		ComPtr<ID3D12Device>				m_pDevice;
		ComPtr<IDXGIFactory4>				m_pDxgiFactory;
		ComPtr<IDXGISwapChain3>				m_pSwapChain;

		ComPtr<ID3D12GraphicsCommandList>	m_pActiveCommandList;
		ComPtr<ID3D12CommandQueue>			m_pCommandQueue;
		ComPtr<ID3D12GraphicsCommandList>	m_pScenePassCommandList;
		ComPtr<ID3D12CommandAllocator>		m_pScenePassCommandAllocators[m_FrameBufferCount];
		ComPtr<ID3D12GraphicsCommandList>	m_pShadowPassCommandList;
		ComPtr<ID3D12CommandAllocator>		m_pShadowPassCommandAllocators[m_FrameBufferCount];

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

		ComPtr<ID3D12RootSignature>			m_pRootSignature_DeferredShadingPass;
		ComPtr<ID3D12RootSignature>			m_pRootSignature_ForwardShadingPass;

		ComPtr<ID3D12PipelineState>			m_pPipelineStateObject_ShadowPass;
		ComPtr<ID3D12PipelineState>			m_pPipelineStateObject_GeometryPass;
		ComPtr<ID3D12PipelineState>			m_pPipelineStateObject_LightingPass;
		ComPtr<ID3D12PipelineState>			m_pPipelineStateObject_SkyPass;
		ComPtr<ID3D12PipelineState>			m_pPipelineStateObject_Transparency;
		ComPtr<ID3D12PipelineState>			m_pPipelineStateObject_PostFxPass;

		//-----Pipeline-----
		//0:  SRV-Albedo(RTV->SRV)
		//1:  SRV-Normal(RTV->SRV)
		//2:  SRV-(R)Roughness/(G)Metallic/(B)AO(RTV->SRV)
		//3:  SRV-Position(RTV->SRV)
		//4:  SRV-Scene Depth(DSV->SRV)
		//5:  SRV-Light Pass Result(RTV->SRV)
		//6:  SRV-Shadow Depth(DSV->SRV)
		//-----PerObject-----
		//7:  SRV-Albedo(SRV)
		//8:  SRV-Normal(SRV)
		//9:  SRV-Roughness(SRV)
		//10: SRV-Metallic(SRV)
		//11: SRV-AO(SRV)
		//12: SRV-Sky Irradiance(SRV)
		//13: SRV-Sky Environment(SRV)
		//14: SRV-Sky BRDF LUT(SRV)
		//15: SRV-Sky Diffuse(SRV)
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




		const UINT PIX_EVENT_UNICODE_VERSION = 0;

		inline void PIXBeginEvent(ID3D12CommandQueue* pCommandQueue, UINT64 /*metadata*/, PCWSTR pFormat)
		{
			pCommandQueue->BeginEvent(PIX_EVENT_UNICODE_VERSION, pFormat, (UINT)((wcslen(pFormat) + 1) * sizeof(pFormat[0])));
		}

		inline void PIXBeginEvent(ID3D12GraphicsCommandList* pCommandList, UINT64 /*metadata*/, PCWSTR pFormat)
		{
			pCommandList->BeginEvent(PIX_EVENT_UNICODE_VERSION, pFormat, (UINT)((wcslen(pFormat) + 1) * sizeof(pFormat[0])));
		}
		inline void PIXEndEvent(ID3D12CommandQueue* pCommandQueue)
		{
			pCommandQueue->EndEvent();
		}
		inline void PIXEndEvent(ID3D12GraphicsCommandList* pCommandList)
		{
			pCommandList->EndEvent();
		}
	};

}
