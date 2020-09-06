#pragma once

#include <Insight/Core.h>

#include "Platform/Windows/Error/COM_Exception.h"


using Microsoft::WRL::ComPtr;

namespace Insight {

	class Direct3D12Context;

	class INSIGHT_API D3D12Helper
	{
	public:
		D3D12Helper();
		~D3D12Helper();

		bool Init(Direct3D12Context* pRendererContext);
		void ResizeResources();
		void CleanUp();

		inline ID3D12Device& GetDeviceContext() const { return *m_pDevice.Get(); }
		inline IDXGIAdapter1& GetAdapter() const { return *m_pAdapter.Get(); }
		inline IDXGIFactory4& GetDXGIFactor() const { return *m_pDxgiFactory.Get(); }

		inline IDXGISwapChain3& GetSwapChain() const { return *m_pSwapChain.Get(); }
		inline ID3D12Resource* GetSwapChainRenderTarget() const { return m_pRenderTargets[m_FrameIndex].Get(); }
		inline ID3D12CommandQueue& GetCommandQueue() const { return *m_pCommandQueue.Get(); }

		void MoveToNextFrame();
		void WaitForGPU();
		inline int GetFrameIndex() const { return m_FrameIndex; }
		inline void SetFrameIndex(int FrameIndex) { m_FrameIndex = FrameIndex; }
		inline void IncrementAndSignalFence()
		{
			m_FenceValues[m_FrameIndex]++;
			ThrowIfFailed(m_pCommandQueue->Signal(m_pFence.Get(), m_FenceValues[m_FrameIndex]),
				"Failed to signal command queue while incremenitign fence values for D3D 12 device resources.");
		}

		const D3D12_VIEWPORT& GetClientViewPort() const { return m_Client_ViewPort; }
		const D3D12_RECT& GetClientScissorRect() const { return m_Client_ScissorRect; }

	private:
		void CreateDXGIFactory();
		void CreateDevice();
		void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter);
		void CreateCommandQueue();
		void CreateSwapChain();
		void CreateViewport();
		void CreateScissorRect();
		void CreateFenceEvent();

	private:
		Direct3D12Context* m_pRenderContextRef;

		static const uint8_t m_FrameBufferCount = 3;

		// CPU/GPU Syncronization
		int						m_FrameIndex = 0;
		UINT64					m_FenceValues[m_FrameBufferCount] = {};
		HANDLE					m_FenceEvent = {};
		ComPtr<ID3D12Fence>		m_pFence;

		ComPtr<IDXGIAdapter1>				m_pAdapter;
		ComPtr<ID3D12Device>				m_pDevice;
		ComPtr<IDXGIFactory4>				m_pDxgiFactory;
		ComPtr<IDXGISwapChain3>				m_pSwapChain;

		ComPtr<ID3D12CommandQueue>			m_pCommandQueue;
		ComPtr<ID3D12CommandAllocator>		m_pCommandAllocators[m_FrameBufferCount];

		ComPtr<ID3D12Resource>				m_pRenderTargets[m_FrameBufferCount];
		ComPtr<ID3D12DescriptorHeap>		m_pRtvHeap;
		
		UINT								m_RtvDescriptorSize;
		D3D12_VIEWPORT						m_Client_ViewPort = {};
		D3D12_RECT							m_Client_ScissorRect = {};
		DXGI_SAMPLE_DESC					m_SampleDesc = {};
	};

}