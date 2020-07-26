#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Renderer.h"
#include "Platform/Windows/Error/COM_Exception.h"

#include "Platform/Windows/DirectX_11/D3D11_Helper.h"
#include "Platform/Windows/DirectX_11/Constant_Buffer_Wrapper.h"

#include "Platform/Windows/DirectX_11/D3D11_Deferred_Tech.h"


using Microsoft::WRL::ComPtr;

namespace Insight {

	class WindowsWindow;
	class GeometryManager;

	class ieD3D11SphereRenderer;

	class INSIGHT_API Direct3D11Context : public Renderer
	{
		friend class Renderer;
	public:
		ID3D11Device& GetDevice() { return *m_pDevice.Get(); }
		ID3D11DeviceContext& GetDeviceContext() { return *m_pDeviceContext.Get(); }

		// Initilize Direc3D 12 library.
		virtual bool InitImpl() override;
		// Destroy the current graphics context
		virtual void DestroyImpl() override;
		// Submit initilize commands to the GPU.
		virtual bool PostInitImpl() override;
		// Upload per-frame constants to the GPU as well as lighting information.
		virtual void OnUpdateImpl(const float deltaTime) override;
		// Flush the command allocators and clear render targets.
		virtual void OnPreFrameRenderImpl() override;
		// Draws shadow pass first then binds geometry pass for future draw commands.
		virtual void OnRenderImpl() override;
		// Binds light pass.
		virtual void OnMidFrameRenderImpl() override;
		// executes the command queue on the GPU. Waits for the GPU to finish before proceeding.
		virtual void ExecuteDrawImpl() override;
		// Swap buffers with the new frame.
		virtual void SwapBuffersImpl() override;
		// Resize render target, depth stencil and sreen rects when window size is changed.
		virtual void OnWindowResizeImpl() override;
		// Tells the swapchain to enable full screen rendering.
		virtual void OnWindowFullScreenImpl() override;

		virtual void SetVertexBuffersImpl(uint32_t StartSlot, uint32_t NumBuffers, ieVertexBuffer* pBuffers) override;
		virtual void SetIndexBufferImpl(ieIndexBuffer* pBuffer) override;
		virtual void DrawIndexedInstancedImpl(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation) override;

		virtual void RenderSkySphereImpl() override;
		virtual bool CreateSkyboxImpl() override;
		virtual void DestroySkyboxImpl() override;

	private:
		Direct3D11Context(WindowsWindow* windowHandle);
		virtual ~Direct3D11Context();

		void CreateDXGIFactory();
		void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter** ppAdapter);
		void CreateDeviceAndSwapChain();
		void CreateRTV();
		void CreateConstantBufferViews();
		void CreateViewports();
		void CreateSamplers();

		void LoadAssets();

	private:
		HWND*				m_pWindowHandle = nullptr;
		WindowsWindow*		m_pWindow = nullptr;
		GeometryManager*	m_pModelManager = nullptr;
		ACamera*			m_pWorldCamera = nullptr;
		bool				m_WindowResizeComplete = true;
		float				m_ClearColor[4] = { 0.1f, 0.1f, 0.3f, 1.0f };

		ConstantBuffer<CB_PS_VS_PerFrame>	m_PerFrameData;
		ConstantBuffer<D3D11_CB_PS_Lights>	m_LightData;
		ConstantBuffer<CB_PS_PostFx>	m_PostFxData;
		D3D_FEATURE_LEVEL					m_DeviceMaxSupportedFeatureLevel;
		DXGI_SAMPLE_DESC					m_SampleDesc = {};
		D3D11_VIEWPORT						m_ScenePassViewport;
		D3D11DeferredShadingTech			m_DeferredShadingTech;

		//D3D11Helper m_DeviceResources;

		ComPtr<IDXGIFactory1> m_pDxgiFactory;
		ComPtr<IDXGIAdapter> m_pAdapter;

		ComPtr<ID3D11Device> m_pDevice;
		ComPtr<ID3D11DeviceContext> m_pDeviceContext;
		ComPtr<IDXGISwapChain> m_pSwapChain;
		ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
		ComPtr<ID3D11Texture2D> m_pBackBuffer;


		ComPtr<ID3D11SamplerState> m_pLinearWrap_SamplerState;
		ComPtr<ID3D11SamplerState> m_pPointClamp_SamplerState;

		ieD3D11SphereRenderer* m_SkySphere;

	};

}