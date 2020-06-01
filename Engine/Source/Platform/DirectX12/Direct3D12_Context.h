#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Rendering_Context.h"
#include "Platform/DirectX12/D3D12_Helper.h"
#include "Platform/Windows/Error/COM_Exception.h"

// TODO: implement shader system that uses this
#include "Platform/DirectX_Shared/Constant_Buffer_Types.h"
#include "Insight/Rendering/Texture.h"
#include "Insight/Rendering/Geometry/Mesh.h"
#include "Platform/DirectX12/Descriptor_Heap_Wrapper.h"
#include "Insight/Rendering/Lighting/APoint_Light.h"
#include "Insight/Rendering/Lighting/ADirectional_Light.h"

using Microsoft::WRL::ComPtr;

namespace Insight {

	class WindowsWindow;

	class ScreenQuad
	{
	public:
		void Init();
		void Render(ComPtr<ID3D12GraphicsCommandList> commandList);

	private:
		ComPtr<ID3D12Resource> m_VertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
		UINT m_NumVerticies = 0u;
	};

	class INSIGHT_API Direct3D12Context : public RenderingContext
	{
	public:
		Direct3D12Context(WindowsWindow* windowHandle);
		virtual ~Direct3D12Context();

		virtual bool Init() override;
		virtual bool PostInit() override;
		virtual void OnUpdate(const float& deltaTime) override;
		virtual void OnPreFrameRender() override;
		virtual void OnRender() override;
		virtual void OnMidFrameRender() override;
		virtual void ExecuteDraw() override;
		virtual void SwapBuffers() override;
		virtual void OnWindowResize() override;
		virtual void OnWindowFullScreen() override;

		inline static Direct3D12Context& Get() { return *s_Instance; }
		inline ID3D12Device& GetDeviceContext() const { return *m_pLogicalDevice.Get(); }
		inline ID3D12GraphicsCommandList& GetCommandList() const { return *m_pCommandList.Get(); }

		inline CDescriptorHeapWrapper& GetCBVSRVDescriptorHeap() { return m_cbvsrvHeap; }

		inline ID3D12Resource& GetConstantBufferPerObjectUploadHeap() const { return *m_PerObjectCBV[m_FrameIndex].Get(); }
		inline UINT8& GetConstantBufferViewGPUHeapAddress() { return *m_cbvPerObjectGPUAddress[m_FrameIndex]; }

		ID3D12Resource* GetRenderTarget() const { return m_pRenderTargets[m_FrameIndex].Get(); }

		const unsigned int GetNumRTVs() const { return m_NumRTV; }

		inline D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle;
			handle.ptr = m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + m_rtvDescriptorSize * m_FrameIndex;
			return handle;
		}

		void AddPointLight(APointLight* pointLight) { m_PointLights.push_back(pointLight); }
		void AddDirectionalLight(ADirectionalLight* pointLight) { m_DirectionalLights.push_back(pointLight); }

	private:
		void CloseCommandListAndSignalCommandQueue();
		// Per-Frame
		void PopulateCommandLists();
		void MoveToNextFrame();
		void BindGeometryPass(bool setPSO = false);
		void BindLightingPass();
		void BindSkyPass();

		// D3D12 Initialize
		void CreateDXGIFactory();
		void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter);
		void CreateDevice();
		void CreateCommandQueue();
		void CreateSwapChain();
		void CreateRenderTargetViewDescriptorHeap();

		void CreateDSV();
		void CreateRTVs();
		void CreateConstantBufferViews();
		void CreateRootSignature();
		void CreateGeometryPassPSO();
		void CreateSkyPassPSO();
		void CreateLightPassPSO();

		void CreateCommandAllocators();
		void CreateFenceEvent();
		void CreateConstantBuffers();
		void CreateViewport();
		void CreateScissorRect();
		void CreateScreenQuad();
		
		void Cleanup();
		void WaitForGPU();
		void UpdateSizeDependentResources();
		void UpdateViewAndScissor();

		void LoadAssets();

	private:
		static Direct3D12Context* s_Instance;
	private:
		HWND* m_pWindowHandle = nullptr;
		WindowsWindow* m_pWindow = nullptr;

		// CPU/GPU Syncronization
		int						m_FrameIndex = 0;
		UINT64					m_FenceValues[m_FrameBufferCount] = {};
		HANDLE					m_FenceEvent = {};
		ComPtr<ID3D12Fence>		m_pFence;

		bool		m_WindowResizeComplete = true;
		bool		m_RayTraceEnabled = false;
		bool		m_UseWarpDevice = false;
		int			m_RtvDescriptorIncrementSize = 0;

		// D3D 12 Usings
		ComPtr<IDXGIAdapter1>				m_pPhysicalDevice;
		ComPtr<ID3D12Device>				m_pLogicalDevice;
		ComPtr<IDXGIFactory4>				m_pDxgiFactory;
		ComPtr<IDXGISwapChain3>				m_pSwapChain;

		ComPtr<ID3D12CommandQueue>			m_pCommandQueue;
		ComPtr<ID3D12GraphicsCommandList>	m_pCommandList;
		ComPtr<ID3D12CommandAllocator>		m_pCommandAllocators[m_FrameBufferCount];

		ComPtr<ID3D12Resource>				m_pRenderTargetTextures[m_FrameBufferCount];
		ComPtr<ID3D12Resource>				m_pRenderTargets[m_FrameBufferCount];
		CDescriptorHeapWrapper				m_rtvHeap;
		ComPtr<ID3D12DescriptorHeap>		m_rtvDescriptorHeap;
		UINT								m_rtvDescriptorSize;

		ComPtr<ID3D12Resource>				m_pDepthStencilTexture;
		CDescriptorHeapWrapper				m_dsvHeap;

		ComPtr<ID3D12RootSignature>			m_pRootSignature;

		ComPtr<ID3D12PipelineState>			m_pPipelineStateObject_GeometryPass;
		ComPtr<ID3D12PipelineState>			m_pPipelineStateObject_LightingPass;
		ComPtr<ID3D12PipelineState>			m_pPipelineStateObject_Sky;

		//0: SRV-Albedo(RTV->SRV)
		//1: SRV-Normal(RTV->SRV)
		//2: SRV-(R)Roughness/(G)Metallic/(B)AO(RTV->SRV)
		//3: SRV-Position(RTV->SRV)
		//4: SRV-Depth(DSV->SRV)
		//-----PerObject-----
		//5: SRV-Albedo(SRV)
		//6: SRV-Normal(SRV)
		//7: SRV-Roughness(SRV)
		//8: SRV-Metallic(SRV)
		//9: SRV-AO(SRV)
		CDescriptorHeapWrapper				m_cbvsrvHeap;
		

		ScreenQuad							m_ScreenQuad;
		D3D12_VIEWPORT						m_ViewPort = {};
		D3D12_RECT							m_ScissorRect = {};
		DXGI_SAMPLE_DESC					m_SampleDesc = {};
		D3D12_DEPTH_STENCIL_VIEW_DESC		m_dsvDesc = {};

		float								m_ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
		static const unsigned int			m_NumRTV = 4;
		DXGI_FORMAT							m_DsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		DXGI_FORMAT							m_RtvFormat[4] = { 
			DXGI_FORMAT_R11G11B10_FLOAT,	// Albedo buffer
			DXGI_FORMAT_R8G8B8A8_SNORM,		// Normal
			DXGI_FORMAT_R11G11B10_FLOAT,	// (R)Roughness/(G)Metallic/(B)AO
			DXGI_FORMAT_R32G32B32A32_FLOAT, // Position
		};
		float								m_ClearDepth = 1.0f;

		// Utils
		struct Resolution
		{
			UINT Width;
			UINT Height;
		};
		static const Resolution m_ResolutionOptions[];
		static const UINT m_ResolutionOptionsCount;
		static UINT m_ResolutionIndex;

		ComPtr<ID3D12Resource> m_LightCBV[m_FrameBufferCount];
		UINT8* m_cbvLightBufferGPUAddress[m_FrameBufferCount];

		ComPtr<ID3D12Resource> m_PerObjectCBV[m_FrameBufferCount];
		UINT8* m_cbvPerObjectGPUAddress[m_FrameBufferCount];

		ComPtr<ID3D12Resource> m_PerFrameCBV[m_FrameBufferCount];
		UINT8* m_cbvPerFrameGPUAddress[m_FrameBufferCount];
		CB_PS_VS_PerFrame m_PerFrameData;

#define MAX_POINT_LIGHTS_SUPPORTED 16
		std::vector<APointLight*> m_PointLights;
		int ConstantBufferPointLightAlignedSize = (sizeof(CB_PS_PointLight) + 255) & ~255;

#define MAX_DIRECTIONAL_LIGHTS_SUPPORTED 4
		std::vector<ADirectionalLight*> m_DirectionalLights;
		int ConstantBufferDirectionalLightAlignedSize = (sizeof(CB_PS_DirectionalLight) + 255) & ~255;

#define MAX_SPOT_LIGHTS_SUPPORTED 16


		Texture m_AlbedoTexture;
		Texture m_NormalTexture;
		Texture m_RoughnessTexture;
		Texture m_MetallicTexture;
		Texture m_AOTexture;


		const UINT PIX_EVENT_UNICODE_VERSION = 0;

		inline void PIXBeginEvent(ID3D12CommandQueue* pCommandQueue, UINT64 /*metadata*/, PCWSTR pFormat)
		{
			pCommandQueue->BeginEvent(PIX_EVENT_UNICODE_VERSION, pFormat, (wcslen(pFormat) + 1) * sizeof(pFormat[0]));
		}

		inline void PIXBeginEvent(ID3D12GraphicsCommandList* pCommandList, UINT64 /*metadata*/, PCWSTR pFormat)
		{
			pCommandList->BeginEvent(PIX_EVENT_UNICODE_VERSION, pFormat, (wcslen(pFormat) + 1) * sizeof(pFormat[0]));
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
