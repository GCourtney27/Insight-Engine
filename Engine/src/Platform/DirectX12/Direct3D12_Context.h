#pragma once

#include "Insight/Core.h"

#include "Insight/Renderer/RenderingContext.h"
#include "Platform/DirectX12/D3D_Api.h"

// TODO: implement shader system that uses this
#include "ConstantBuffersPerObject_TEMP.h"
#include <DirectXMath.h>


namespace Insight {

	class WindowsWindow;

	class Direct3D12Context : public RenderingContext, public D3DApi
	{
	public:
		Direct3D12Context(Insight::WindowsWindow* windowHandle);
		virtual ~Direct3D12Context();
		
		virtual void OnUpdate() override;

		virtual bool Init() override;
		virtual void RenderFrame() override;
		virtual void SwapBuffers() override;
		virtual void OnWindowResize(UINT width, UINT height) override;

		inline ID3D12Device& GetDevice() const { return *m_pDevice.Get(); }
		inline ID3D12DescriptorHeap& GetImGuiDescriptorHeap() const { return *m_pImGuiDescriptorHeap.Get(); }
		inline ID3D12GraphicsCommandList& GetCommandList() const { return *m_pCommandList.Get(); }

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
		void CreateDepthStencilBuffer();
		void CreateCommandAllocators();
		void CreateFenceEvent();
		void CreatePipelineStateObjects();
		void CreateShaderDescriptorHeaps();
		void CreateViewport();
		void CreateScissorRect();
		void CreateImGuiDescriptorHeap();

		void InitShaders();// TEMP! Move this!
		//ConstantBuffer<ConstantBufferPerObject> cb_vertexShader;// TEMP! Move this!

		void Cleanup();
		void WaitForGPU();
		void ToggleFullscreen(IDXGISwapChain* pSwapChain);
	private:
		HWND* m_pWindowHandle = nullptr;
		WindowsWindow* m_pWindow = nullptr;

		// Sync Values
		int m_FrameIndex = 0;
		int m_RtvDescriptorSize = 0;
		UINT64 m_FenceValue[m_FrameBufferCount] = {};
		HANDLE m_FenceEvent = {};

		bool m_RayTraceEnabled = false;
		bool m_UseWarpDevice = false;

		WRL::ComPtr<ID3D12Device5> m_pDevice;
		WRL::ComPtr<IDXGIFactory4> m_pDxgiFactory;
		WRL::ComPtr<IDXGISwapChain3> m_pSwapChain;

		WRL::ComPtr<ID3D12CommandQueue> m_pCommandQueue;
		WRL::ComPtr<ID3D12DescriptorHeap> m_pRtvDescriptorHeap;
		WRL::ComPtr<ID3D12Resource> m_pRenderTargets[m_FrameBufferCount];
		WRL::ComPtr<ID3D12CommandAllocator> m_pCommandAllocators[m_FrameBufferCount];

		WRL::ComPtr<ID3D12Fence> m_pFence[m_FrameBufferCount];
		WRL::ComPtr<ID3D12GraphicsCommandList> m_pCommandList;

		WRL::ComPtr<ID3D12Resource> m_pDepthStencilBuffer;
		WRL::ComPtr<ID3D12DescriptorHeap> m_pDepthStencilDescriptorHeap;

		WRL::ComPtr<ID3D12PipelineState> m_pPipelineStateObject_Default;
		WRL::ComPtr<ID3D12RootSignature> m_pRootSignature_Default;

		D3D12_VIEWPORT m_ViewPort = {};
		D3D12_RECT m_ScissorRect = {};
		DXGI_SAMPLE_DESC m_SampleDesc = {};

		// IMGUI this should move
		WRL::ComPtr <ID3D12DescriptorHeap> m_pImGuiDescriptorHeap;


		//=== TEMPORARY! ===//
		//TODO: Move this to a model/vertex class
		WRL::ComPtr<ID3D12Resource> m_pVertexBuffer;
		WRL::ComPtr<ID3D12Resource> m_pVBufferUploadHeap;

		WRL::ComPtr<ID3D12Resource> m_pIndexBuffer;
		WRL::ComPtr<ID3D12Resource> m_pIndexBufferUploadHeap;
		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView = {};

		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView = {};
		struct Vertex {
			Vertex(float x, float y, float z, float r, float g, float b, float a)
				: pos(x, y, z), color(r, g, b, a) {}
			DirectX::XMFLOAT3 pos = {};
			DirectX::XMFLOAT4 color = {};
		};

		// TODO: Move this to constant buffers class
		struct ConstantBuffer {
			DirectX::XMFLOAT4 colorMultiplier;
		};
		WRL::ComPtr<ID3D12DescriptorHeap> m_pMainDescriptorHeap[m_FrameBufferCount];
		WRL::ComPtr<ID3D12Resource> m_pConstantBufferUploadHeap[m_FrameBufferCount];
		ConstantBuffer m_cbColorMultiplierData;
		UINT8* m_cbColorMultiplierGPUAddress[m_FrameBufferCount];

		//=== TEMPORARY! ===// END

	};

}
