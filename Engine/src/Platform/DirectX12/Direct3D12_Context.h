#pragma once

#include "Insight/Core.h"

#include "Insight/Renderer/RenderingContext.h"
#include "Platform/DirectX12/DX12_App.h"
#include "ConstantBuffersPerObject_TEMP.h"

namespace Insight {

	using namespace Microsoft;

	class Direct3D12Context : public RenderingContext, public DXApp
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
		void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter);
		void CreateCommandQueue();
		void CreateSwapChain();
		void CreateRTVDescriptorHeap();
		void CreateCommandAllocators();
		void CreateFenceEvent();
		void CreateRootSignature();

		void InitShaders();// TEMP! Move this!
		//ConstantBuffer<ConstantBufferPerObject> cb_vertexShader;// TEMP! Move this!

	private:
		HWND* m_WindowHandle;
		
		// Sync Values
		int m_FrameIndex = 0;
		int m_RtvDescriptorSize = 0;
		UINT64 m_FenceValue[m_FrameBufferCount];
		HANDLE m_FenceEvent;

		bool m_RayTraceEnabled = false;
		bool m_UseWarpDevice = false;

		WRL::ComPtr<ID3D12Device5> m_Device;
		WRL::ComPtr<IDXGIFactory4> m_DxgiFactory;
		WRL::ComPtr<IDXGISwapChain3> m_SwapChain;

		WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
		WRL::ComPtr<ID3D12DescriptorHeap> m_RtvDescriptorHeap;
		WRL::ComPtr<ID3D12Resource> m_RenderTargets[m_FrameBufferCount];
		WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocators[m_FrameBufferCount];

		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence[m_FrameBufferCount];
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;

	};

}
