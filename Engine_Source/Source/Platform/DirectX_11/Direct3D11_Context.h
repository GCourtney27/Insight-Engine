#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Renderer.h"
#include "Platform/Win32/Error/COM_Exception.h"

#include "Platform/DirectX_11/D3D11_Helper.h"
#include "Platform/DirectX_11/Wrappers/Constant_Buffer_Wrapper.h"

#include "Platform/DirectX_11/D3D11_Deferred_Tech.h"




namespace Insight {

	class Win32Window;
	class GeometryManager;

	class ieD3D11SphereRenderer;

	class INSIGHT_API Direct3D11Context : public Renderer
	{
		friend class Renderer;
	public:
		ID3D11Device& GetDevice() { return *m_pDevice.Get(); }
		ID3D11DeviceContext& GetDeviceContext() { return *m_pDeviceContext.Get(); }

		// Initilize Direc3D 12 library.
		virtual bool Init_Impl() override;
		// Destroy the current graphics context
		virtual void Destroy_Impl() override;
		// Submit initilize commands to the GPU.
		virtual bool PostInit_Impl() override;
		// Upload per-frame constants to the GPU as well as lighting information.
		virtual void OnUpdate_Impl(const float deltaTime) override;
		// Flush the command allocators and clear render targets.
		virtual void OnPreFrameRender_Impl() override;
		// Draws shadow pass first then binds geometry pass for future draw commands.
		virtual void OnRender_Impl() override;
		// Binds light pass.
		virtual void OnMidFrameRender_Impl() override;
		// executes the command queue on the GPU. Waits for the GPU to finish before proceeding.
		virtual void ExecuteDraw_Impl() override;
		// Swap buffers with the new frame.
		virtual void SwapBuffers_Impl() override;
		// Resize render target, depth stencil and sreen rects when window size is changed.
		virtual void OnWindowResize_Impl() override;
		// Tells the swapchain to enable full screen rendering.
		virtual void OnWindowFullScreen_Impl() override;
		// Reloads all shaders
		virtual void OnShaderReload_Impl() override;
		virtual void OnEditorRender_Impl() override;

		virtual void SetVertexBuffers_Impl(uint32_t StartSlot, uint32_t NumBuffers, ieVertexBuffer* pBuffers) override;
		virtual void SetIndexBuffer_Impl(ieIndexBuffer* pBuffer) override;
		virtual void DrawIndexedInstanced_Impl(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation) override;
		virtual void DrawText_Impl(const wchar_t* Text) override;

		virtual void RenderSkySphere_Impl() override;
		virtual bool CreateSkybox_Impl() override;
		virtual void DestroySkybox_Impl() override;

	private:
		Direct3D11Context();
		virtual ~Direct3D11Context();

		void CreateDXGIFactory();
		void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter** ppAdapter);
		void CreateDeviceAndSwapChain();
		void CreateRTV();
		void CreateConstantBufferViews();
		void CreateViewports();
		void CreateScissorRect();
		void CreateSamplers();
		void UpdateSizeDependentResources();
		void UpdateViewAndScissor();

		void LoadAssets();

	private:
		GeometryManager*	m_pModelManager = nullptr;
		bool				m_WindowResizeComplete = true;
		float				m_ClearColor[4] = { 0.1f, 0.1f, 0.3f, 1.0f };

		ConstantBuffer<CB_PS_VS_PerFrame>	m_PerFrameData;
		ConstantBuffer<CB_PS_Lights>	m_LightData;
		ConstantBuffer<CB_PS_PostFx>		m_PostFxData;
		D3D_FEATURE_LEVEL					m_DeviceMaxSupportedFeatureLevel;
		DXGI_SAMPLE_DESC					m_SampleDesc = {};
		D3D11_VIEWPORT						m_ScenePassViewPort = {};
		D3D12_RECT							m_ScenePassScissorRect = {};
		D3D11DeferredShadingTech			m_DeferredShadingTech = {};

		//D3D11Helper m_DeviceResources;

		Microsoft::WRL::ComPtr<IDXGIFactory1> m_pDxgiFactory;
		Microsoft::WRL::ComPtr<IDXGIAdapter> m_pAdapter;
		
		Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pDeviceContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pBackBuffer;
		
		
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pLinearWrap_SamplerState;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pPointClamp_SamplerState;

		ieD3D11SphereRenderer* m_SkySphere;


	};

}