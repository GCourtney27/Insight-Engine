#pragma once

#include "EngineDefines.h"

#include "Platform/Public/Utility/COMException.h"

#include "Platform/DirectX12/Wrappers/DescriptorHeapWrapper.h"



namespace Insight {

	class Direct3D12Context;

	class INSIGHT_API D3D12DeviceResources
	{
	public:
		D3D12DeviceResources();
		~D3D12DeviceResources();

		bool Init(Direct3D12Context* pRendererContext);
		void ResizeResources();
		void CleanUp();

		inline ID3D12Device& GetD3D12Device() const { return *m_pD3D12Device.Get(); }
		inline IDXGIAdapter1& GetAdapter() const { return *m_pAdapter.Get(); }
		inline IDXGIFactory4& GetDXGIFactory() const { return *m_pDxgiFactory.Get(); }

		inline ID3D12Resource& GetSwapChainRenderTarget() const { return *m_pSwapChainRenderTargets[m_FrameIndex].Get(); }
		inline D3D12_CPU_DESCRIPTOR_HANDLE GetSwapChainRTV() const
		{
			D3D12_CPU_DESCRIPTOR_HANDLE Handle;
			Handle.ptr = m_SwapChainRTVHeap.hCPUHeapStart.ptr + m_SwapChainRTVHeap.HandleIncrementSize * m_FrameIndex;
			return Handle;
		}
		inline ID2D1Bitmap1& GetD2DRenderTarget() const { return *m_d2dRenderTargets[m_FrameIndex].Get(); }
		inline ID3D11Resource& GetWrappedD2DBackBuffer() const { return *m_WrappedBackBuffers[m_FrameIndex].Get(); }


		inline ID3D11On12Device& GetD3D11On12Device() const { return *m_D3D11On12Device.Get(); }
		inline ID2D1DeviceContext2& GetD2DDeviceContext() const { return *m_d2dDeviceContext.Get(); }
		inline IDWriteFactory& GetWriteFactory() const { return *m_dWriteFactory.Get(); }
		inline ID3D11DeviceContext& GetD3D11DeviceContext() const { return *m_pD3D11DeviceContext.Get(); }

		inline IDXGISwapChain3& GetSwapChain() const { return *m_pSwapChain.Get(); }
		inline ID3D12CommandQueue& GetGraphicsCommandQueue() const { return *m_pGraphicsCommandQueue.Get(); }
		inline ID3D12CommandQueue& GetComputeCommandQueue() const { return *m_pComputeCommandQueue.Get(); }
		inline DXGI_FORMAT GetSwapChainBackBufferFormat() const { return m_SwapChainBackBufferFormat; }

		inline int GetFrameIndex() const { return m_FrameIndex; }
		inline void SetFrameIndex(int FrameIndex) { m_FrameIndex = FrameIndex; }
		inline void ResetFenceValue(uint32_t Index) { m_FenceValues[Index] = m_FrameIndex; }
		void MoveToNextFrame();
		void WaitForGPU();
		FORCEINLINE void IncrementAndSignalFence()
		{
			m_FenceValues[m_FrameIndex]++;
			ThrowIfFailed(m_pGraphicsCommandQueue->Signal(m_pFence.Get(), m_FenceValues[m_FrameIndex]), TEXT(""));
		}

		inline const D3D12_VIEWPORT GetClientViewPort() const { return m_Client_ViewPort; }
		inline const D3D12_RECT GetClientScissorRect() const { return m_Client_ScissorRect; }

	private:
		void CreateDXGIFactory();
		void CreateDevice();
		void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter);
		void CreateCommandQueues();
		void CreateSwapChain();
		void CreateViewport();
		void CreateScissorRect();
		void CreateFenceEvent();

		void CreateD3D11On12Resources();

		void CreateSwapchainRTVDescriptors();

	private:
		Direct3D12Context*		m_pRenderContextRef;

		static const uint8_t	m_FrameBufferCount = 3;
		bool m_ForceUseWarpAdapter;
		

		// CPU/GPU Syncronization
		int									m_FrameIndex = 0;
		UINT64								m_FenceValues[m_FrameBufferCount] = {};
		HANDLE								m_FenceEvent = {};
		HANDLE								m_ComputeFenceEvent = {};
		Microsoft::WRL::ComPtr<ID3D12Fence>	m_pFence;

		Microsoft::WRL::ComPtr<IDXGIAdapter1>				m_pAdapter;
		Microsoft::WRL::ComPtr<ID3D12Device>				m_pD3D12Device;
		Microsoft::WRL::ComPtr<IDXGIFactory4>				m_pDxgiFactory;
		Microsoft::WRL::ComPtr<IDXGISwapChain3>				m_pSwapChain;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue>			m_pComputeCommandQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>			m_pGraphicsCommandQueue;

		// Number of decriptors depends on frame buffer count. Start slot is 0.
		CDescriptorHeapWrapper				m_SwapChainRTVHeap;

		Microsoft::WRL::ComPtr<ID3D12Resource>				m_pSwapChainRenderTargets[m_FrameBufferCount];
		Microsoft::WRL::ComPtr<ID3D11Resource>				m_WrappedBackBuffers[m_FrameBufferCount];
		Microsoft::WRL::ComPtr<ID2D1Bitmap1>				m_d2dRenderTargets[m_FrameBufferCount];

		DXGI_FORMAT							m_SwapChainBackBufferFormat;
		UINT								m_RtvDescriptorSize;
		D3D12_VIEWPORT						m_Client_ViewPort = {};
		D3D12_RECT							m_Client_ScissorRect = {};
		DXGI_SAMPLE_DESC					m_SampleDesc = {};

		/* D3D11On12 Resources */

		Microsoft::WRL::ComPtr<ID3D11On12Device>		m_D3D11On12Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>		m_pD3D11DeviceContext;
		Microsoft::WRL::ComPtr<ID2D1Factory3>			m_d2dFactory;
		Microsoft::WRL::ComPtr<ID2D1Device2>			m_d2dDevice;
		Microsoft::WRL::ComPtr<ID2D1DeviceContext2>		m_d2dDeviceContext;
		Microsoft::WRL::ComPtr<IDWriteFactory>			m_dWriteFactory;
	};

}