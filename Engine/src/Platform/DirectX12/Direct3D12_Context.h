#pragma once

#include "Insight/Core.h"

#include "Insight/Renderer/RenderingContext.h"
#include "Platform/DirectX12/D3D_Api.h"

// TODO: implement shader system that uses this
#include "ConstantBuffersPerObject_TEMP.h"

namespace Insight {


	class Direct3D12Context : public RenderingContext, public D3DApi
	{
	public:
		Direct3D12Context(HWND* windowHandle, uint32_t windowWidth, uint32_t windowHeight);
		virtual ~Direct3D12Context();
		
		void OnUpdate();

		virtual void Init() override;
		virtual void RenderFrame() override;
		virtual void SwapBuffers() override;
	private:
		// Per-Frame
		void WaitForPreviousFrame();
		void UpdatePipeline();

		// D3D12 Initialize
		void CreateDXGIFactory();
		void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter);
		void CreateDevice();
		void CreateCommandQueue();
		void CreateSwapChain();
		void CreateRTVDescriptorHeap();
		void CreateCommandAllocators();
		void CreateFenceEvent();
		void CreateRootSignature();

		void InitShaders();// TEMP! Move this!
		//ConstantBuffer<ConstantBufferPerObject> cb_vertexShader;// TEMP! Move this!

		void Cleanup();
	private:
		HWND* m_WindowHandle;
		
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

		Microsoft::WRL::ComPtr<ID3D12Fence> m_pFence[m_FrameBufferCount];
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_pRootSignature;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pCommandList;

	};

}
