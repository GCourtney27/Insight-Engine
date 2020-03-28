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
		
		void OnUpdate();

		virtual bool Init() override;
		virtual void RenderFrame() override;
		virtual void SwapBuffers() override;

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
		void CreateRTVDescriptorHeap();
		void CreateCommandAllocators();
		void CreateFenceEvent();
		void CreatePipelineStateObjects();
		void CreateRootSignature();
		void CreateViewport();
		void CreateScissorRect();

		void InitShaders();// TEMP! Move this!
		//ConstantBuffer<ConstantBufferPerObject> cb_vertexShader;// TEMP! Move this!

		void Cleanup();
	private:
		HWND* m_pWindowHandle;
		WindowsWindow* m_pWindow;

		// Sync Values
		int m_FrameIndex = 0;
		int m_RtvDescriptorSize = 0;
		UINT64 m_FenceValue[m_FrameBufferCount];
		HANDLE m_FenceEvent;

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

		WRL::ComPtr<ID3D12PipelineState> m_pPipelineStateObject_Default;
		WRL::ComPtr<ID3D12RootSignature> m_pRootSignature_Default;
		D3D12_VIEWPORT m_ViewPort;
		D3D12_RECT m_ScissorRect;
		DXGI_SAMPLE_DESC m_SampleDesc;

		//=== TEMPORARY! ===//
		//TODO: Move this to a model/vertex class
		WRL::ComPtr<ID3D12Resource> m_pVertexBuffer;
		WRL::ComPtr<ID3D12Resource> m_pVBufferUploadHeap;

		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
		struct Vertex {
			DirectX::XMFLOAT3 pos;
		};

	};

}
