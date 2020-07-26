#pragma once

#include <Insight/Core.h>

#include "Platform/Windows/Error/COM_Exception.h"


using Microsoft::WRL::ComPtr;

namespace Insight {

	class INSIGHT_API D3D12Helper
	{
	public:
		D3D12Helper();
		~D3D12Helper();

		bool Init(HWND windowHandle, UINT windowWidth, UINT windowHeight);

		inline ID3D12Device& GetDeviceContext() const { return *m_pDeviceContext.Get(); }
		inline IDXGIAdapter1& GetAdapter() const { return *m_pAdapter.Get(); }
		inline IDXGIFactory4& GetDXGIFactor() const { return *m_pDxgiFactory.Get(); }

		inline IDXGISwapChain3& GetSwapChain() const { return *m_pSwapChain.Get(); }
		inline ID3D12Resource* GetRenderTarget() const { return m_pRenderTargets[m_FrameIndex].Get(); }
		inline ID3D12CommandQueue& GetCommandQueue() const { return *m_pCommandQueue.Get(); }

	private:

		void CreateDXGIFactory();
		void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter);
		void CreateDevice();
		void CreateSwapChain(HWND& WindowHandle, bool AllowTearing);
		void CreateCommandQueue();
		void CreateCommandAllocators();
		void CreateFenceEvent();
		void CreateViewport();
		void CreateScissorRect();
	private:
		static const uint8_t m_FrameBufferCount = 3;
		UINT				 m_WindowWidth;
		UINT				 m_WindowHeight;
		// Sync Values
		int									m_FrameIndex = 0;
		int									m_RtvDescriptorIncrementSize = 0;
		UINT64								m_FenceValues[m_FrameBufferCount] = {};
		HANDLE								m_FenceEvent = {};
		ComPtr<ID3D12Fence>					m_pFences[m_FrameBufferCount];

		ComPtr<IDXGIAdapter1>				m_pAdapter;
		ComPtr<ID3D12Device>				m_pDeviceContext;
		ComPtr<IDXGIFactory4>				m_pDxgiFactory;
		ComPtr<IDXGISwapChain3>				m_pSwapChain;

		ComPtr<ID3D12CommandQueue>			m_pCommandQueue;
		ComPtr<ID3D12CommandAllocator>		m_pCommandAllocators[m_FrameBufferCount];

		ComPtr<ID3D12Resource>				m_pRenderTargets[m_FrameBufferCount];
		ComPtr<ID3D12DescriptorHeap>		m_pRtvHeap;
		
		UINT								m_RtvDescriptorSize;
		D3D12_VIEWPORT						m_ViewPort = {};
		D3D12_RECT							m_ScissorRect = {};
		DXGI_SAMPLE_DESC					m_SampleDesc = {};
	};

}