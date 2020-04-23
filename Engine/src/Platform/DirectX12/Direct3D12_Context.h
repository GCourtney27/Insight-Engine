#pragma once

#include "Insight/Core.h"

#include "Insight/Rendering/RenderingContext.h"

#include "Platform/Windows/Error/COM_Exception.h"
#include "Insight/Game/Camera.h"

// TODO: implement shader system that uses this
#include "Platform/DirectX_Shared/ConstantBuffersPerObject_TEMP.h"

//TEMP
#include "Insight/Rendering/Geometry/Model.h"
#include "Insight/Rendering/Texture.h"

using Microsoft::WRL::ComPtr;

namespace Insight {

	class WindowsWindow;

	class Direct3D12Context : public RenderingContext
	{
	public:
		Direct3D12Context(WindowsWindow* windowHandle);
		virtual ~Direct3D12Context();
		
		virtual bool Init() override;
		virtual void OnUpdate() override;
		virtual void OnPreFrameRender() override;
		virtual void OnRender() override;
		virtual void ExecuteDraw() override;
		virtual void SwapBuffers() override;
		virtual void OnWindowResize() override;
		virtual void OnWindowFullScreen() override;

		inline ID3D12Device& GetDeviceContext() const { return *m_pLogicalDevice.Get(); }
		inline ID3D12GraphicsCommandList& GetCommandList() const { return *m_pCommandList.Get(); }
		inline ID3D12DescriptorHeap& GetShaderVisibleDescriptorHeap() const { return *m_pMainDescriptorHeap.Get(); }

	private:
		// Per-Frame
		void PopulateCommandLists();
		void WaitForPreviousFrame();

		// D3D12 Initialize
		void CreateDXGIFactory();
		void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter);
		void CreateDevice();
		void CreateCommandQueue();
		void CreateSwapChain();
		void CreateDescriptorHeaps();
		void CreateRTVDescriptorHeap();
		void CreateDSVDescriptorHeap();
		void CreateShaderVisibleResourceDescriptorHeap();
		void CreateDepthStencilBuffer();
		void CreateCommandAllocators();
		void CreateFenceEvent();
		void CreatePipelineStateObjects();
		void CreateConstantBufferResourceHeaps();
		void CreateViewport();
		void CreateScissorRect();
		void CloseCommandListAndSignalCommandQueue();

		// TEMP! Move this!
		void InitShaders();
		DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID);
		WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID);
		int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat);
		int LoadImageDataFromFile(BYTE** imageData, D3D12_RESOURCE_DESC& resourceDescription, LPCWSTR filename, int& bytesPerRow);
		void InitDemoScene();
		//ConstantBuffer<ConstantBufferPerObject> cb_vertexShader;// TEMP! Move this!
		void LoadAssets();
		void LoadModels();
		void LoadTextures();
		//----------------

		void Cleanup();
		void WaitForGPU();
		void UpdateSizeDependentResources();
		void UpdateViewAndScissor();
	private:
		HWND* m_pWindowHandle			= nullptr;
		WindowsWindow* m_pWindow		= nullptr;

		// Sync Values
		int			m_FrameIndex				= 0;
		int			m_RtvDescriptorSize			= 0;
		UINT64		m_FenceValues[m_FrameBufferCount] = {};
		HANDLE		m_FenceEvent = {};

		bool		m_WindowResizeComplete = true;
		bool		m_RayTraceEnabled = false;
		bool		m_UseWarpDevice = false;
		
		ComPtr<IDXGIAdapter1>				m_pPhysicalDevice;
		ComPtr<ID3D12Device>				m_pLogicalDevice;
		ComPtr<IDXGIFactory4>				m_pDxgiFactory;
		ComPtr<IDXGISwapChain3>				m_pSwapChain;

		ComPtr<ID3D12CommandQueue>			m_pCommandQueue;
		ComPtr<ID3D12DescriptorHeap>		m_pRtvDescriptorHeap;
		ComPtr<ID3D12Resource>				m_pRenderTargets[m_FrameBufferCount];
		ComPtr<ID3D12CommandAllocator>		m_pCommandAllocators[m_FrameBufferCount];

		ComPtr<ID3D12Fence>					m_pFences[m_FrameBufferCount];
		ComPtr<ID3D12GraphicsCommandList>	m_pCommandList;

		ComPtr<ID3D12Resource>				m_pDepthStencilBuffer;
		ComPtr<ID3D12DescriptorHeap>		m_pDepthStencilDescriptorHeap;

		ComPtr<ID3D12PipelineState>			m_pPipelineStateObject_ForwardPass;
		ComPtr<ID3D12RootSignature>			m_pRootSignature_ForwardPass;

		ComPtr<ID3D12DescriptorHeap>		m_pMainDescriptorHeap;


		D3D12_VIEWPORT						m_ViewPort = {};
		D3D12_RECT							m_ScissorRect = {};
		DXGI_SAMPLE_DESC					m_SampleDesc = {};
		D3D12_DEPTH_STENCIL_VIEW_DESC		m_dsvDesc = {};

		//=== TEMPORARY! ===//
		Model model;

		struct ConstantBufferPerObject {
			DirectX::XMFLOAT4X4 wvpMatrix;
		};
		int ConstantBufferPerObjectAlignedSize = (sizeof(ConstantBufferPerObject) + 255) & ~255;
		ConstantBufferPerObject cbPerObject;
		ComPtr<ID3D12Resource> constantBufferUploadHeaps[m_FrameBufferCount];

		UINT8* cbvGPUAddress[m_FrameBufferCount]; 
		Camera camera;

		DirectX::XMFLOAT4X4 cube1WorldMat;
		DirectX::XMFLOAT4X4 cube1RotMat;
		DirectX::XMFLOAT4 cube1Position;

		// TEMP Textures
		// TODO: create texture manager class
		Texture texture;
		Texture texture2;

		// Utils
		struct Resolution
		{
			UINT Width;
			UINT Height;
		};
		static const Resolution m_ResolutionOptions[];
		static const UINT m_ResolutionOptionsCount;
		static UINT m_ResolutionIndex; // Index of the current scene rendering resolution from m_resolutionOptions.

	};

}
