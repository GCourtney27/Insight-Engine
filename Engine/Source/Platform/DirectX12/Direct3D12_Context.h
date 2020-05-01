#pragma once

#include "Insight/Core.h"

#include "Insight/Rendering/RenderingContext.h"
#include "Platform/Windows/Error/COM_Exception.h"

#include "Insight/Runtime/ACamera.h"

// TODO: implement shader system that uses this
#include "Platform/DirectX_Shared/Constant_Buffer_Types.h"

//TEMP
#include "Insight/Systems/Mesh_Manager.h"
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
		virtual void OnUpdate(const float& deltaTime) override;
		virtual void OnPreFrameRender() override;
		virtual void OnRender() override;
		virtual void ExecuteDraw() override;
		virtual void SwapBuffers() override;
		virtual void OnWindowResize() override;
		virtual void OnWindowFullScreen() override;

		inline static Direct3D12Context& Get() { return *s_Instance; }

		inline ID3D12Device& GetDeviceContext() const { return *m_pLogicalDevice.Get(); }
		inline ID3D12GraphicsCommandList& GetCommandList() const { return *m_pCommandList.Get(); }
		inline ID3D12DescriptorHeap& GetShaderVisibleDescriptorHeap() const { return *m_pMainDescriptorHeap.Get(); }
		inline ID3D12Resource& GetConstantBufferUploadHeap() const { return *m_ConstantBufferUploadHeaps[m_FrameIndex].Get(); }
		inline UINT8& GetConstantBufferViewGPUHeapAddress() {return *m_cbvGPUAddress[m_FrameIndex];}

		inline const Camera& GetCamera() const { return camera; }
		
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
		void CreateRenderTargetViewDescriptorHeap();
		void CreateDepthStencilViewDescriptorHeap();
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
		void LoadAssets();
		void LoadModels();
		void LoadTextures();
		//----------------

		void Cleanup();
		void WaitForGPU();
		void UpdateSizeDependentResources();
		void UpdateViewAndScissor();
	private:
		static Direct3D12Context* s_Instance;
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

		const UINT AO_MAP_SHADER_REGISTER = Texture::eTextureType::AO;
		const UINT ALBEDO_MAP_SHADER_REGISTER = Texture::eTextureType::ALBEDO;
		const UINT NORMAL_MAP_SHADER_REGISTER = Texture::eTextureType::NORMAL;
		const UINT METALLIC_MAP_SHADER_REGISTER = Texture::eTextureType::METALLIC;
		const UINT SPECULAR_MAP_SHADER_REGISTER = Texture::eTextureType::SPECULAR;
		const UINT ROUGHNESS_MAP_SHADER_REGISTER = Texture::eTextureType::ROUGHNESS;

		D3D12_VIEWPORT						m_ViewPort = {};
		D3D12_RECT							m_ScissorRect = {};
		DXGI_SAMPLE_DESC					m_SampleDesc = {};
		D3D12_DEPTH_STENCIL_VIEW_DESC		m_dsvDesc = {};

		// Utils
		struct Resolution
		{
			UINT Width;
			UINT Height;
		};
		static const Resolution m_ResolutionOptions[];
		static const UINT m_ResolutionOptionsCount;
		static UINT m_ResolutionIndex;

		//=== TEMPORARY! ===//
		ModelManager m_ModelManager;

		Model model;
		Camera camera;

		ComPtr<ID3D12Resource> m_ConstantBufferUploadHeaps[m_FrameBufferCount];
		UINT8* m_cbvGPUAddress[m_FrameBufferCount]; 

		ComPtr<ID3D12Resource> m_ConstantBufferPerFrameUploadHeaps[m_FrameBufferCount];
		UINT8* m_cbvPerFrameGPUAddress[m_FrameBufferCount];
		CB_PS_VS_PerFrame m_PerFrameData;

		// TEMP Textures
		// TODO: create texture manager class
		Texture texture;
		Texture texture2;

		
	};

}
