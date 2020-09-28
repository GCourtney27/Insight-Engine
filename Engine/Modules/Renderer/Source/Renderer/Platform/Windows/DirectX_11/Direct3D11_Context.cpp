#include <Renderer_pch.h>

#include "Direct3D11_Context.h"

#include "Retina/Core/Application.h"
#include "Platform/Windows/Windows_Window.h"
#include "Platform/Windows/DirectX_11/Geometry/D3D11_Index_Buffer.h"
#include "Platform/Windows/DirectX_11/Geometry/D3D11_Vertex_Buffer.h"
#include "Platform/Windows/DirectX_11/Geometry/D3D11_Sphere_Renderer.h"

#include "Retina/Runtime/APlayer_Character.h"
#include "Retina/Systems/Managers/Geometry_Manager.h"

#include "Retina/Rendering/APost_Fx.h"
#include "Retina/Rendering/ASky_Light.h"
#include "Retina/Rendering/ASky_Sphere.h"
#include "Retina/Rendering/Lighting/ASpot_Light.h"
#include "Retina/Rendering/Lighting/APoint_Light.h"
#include "Retina/Rendering/Lighting/ADirectional_Light.h"

namespace Retina {




	Direct3D11Context::Direct3D11Context(WindowsWindow* WindowHandle)
		: m_pWindowHandle(&WindowHandle->GetWindowHandleRef()),
		m_pWindow(WindowHandle),
		Renderer(WindowHandle->GetWidth(), WindowHandle->GetHeight(), false)
	{
	}

	Direct3D11Context::~Direct3D11Context()
	{
	}

	bool Direct3D11Context::Init_Impl()
	{
		RN_CORE_INFO("Renderer: D3D 11");

		CreateDXGIFactory();
		CreateDeviceAndSwapChain();
		CreateRTV();
		CreateConstantBufferViews();
		CreateViewports();
		CreateScissorRect();
		CreateSamplers();

		m_DeferredShadingTech.Init(m_pDevice.Get(), m_pDeviceContext.Get(), m_pWindow);
		LoadAssets();

		return true;
	}

	void Direct3D11Context::SetVertexBuffers_Impl(uint32_t StartSlot, uint32_t NumBuffers, ieVertexBuffer* pBuffers)
	{
		RN_CORE_ASSERT(dynamic_cast<D3D11VertexBuffer*>(pBuffers) != nullptr, "A vertex buffer passed to renderer with D3D 11 active must be a \"D3D11VertexBuffer\"");
		m_pDeviceContext->IASetVertexBuffers(StartSlot, NumBuffers, reinterpret_cast<D3D11VertexBuffer*>(pBuffers)->GetBufferPtr(), reinterpret_cast<D3D11VertexBuffer*>(pBuffers)->GetStridePtr(), reinterpret_cast<D3D11VertexBuffer*>(pBuffers)->GetBufferOffset());
	}

	void Direct3D11Context::SetIndexBuffer_Impl(ieIndexBuffer* pBuffer)
	{
		RN_CORE_ASSERT(dynamic_cast<D3D11IndexBuffer*>(pBuffer) != nullptr, "A index buffer passed to renderer with D3D 11 active must be a \"D3D11IndexBuffer\"");
		m_pDeviceContext->IASetIndexBuffer(reinterpret_cast<D3D11IndexBuffer*>(pBuffer)->GetBufferPtr(), reinterpret_cast<D3D11IndexBuffer*>(pBuffer)->GetFormat(), reinterpret_cast<D3D11IndexBuffer*>(pBuffer)->GetBufferOffset());
	}

	void Direct3D11Context::DrawIndexedInstanced_Impl(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation)
	{
		m_pDeviceContext->DrawIndexed(IndexCountPerInstance, StartIndexLocation, BaseVertexLoaction);
	}

	void Direct3D11Context::RenderSkySphere_Impl()
	{
		m_SkySphere->Render();
	}

	bool Direct3D11Context::CreateSkybox_Impl()
	{
		m_SkySphere = new ieD3D11SphereRenderer();
		m_SkySphere->Init(10, 20, 20, m_pDevice.Get(), m_pDeviceContext.Get());

		return true;
	}

	void Direct3D11Context::DestroySkybox_Impl()
	{
		if (m_SkySphere) {
			delete m_pSkySphere;
		}
	}

	void Direct3D11Context::Destroy_Impl()
	{
	}

	bool Direct3D11Context::PostInit_Impl()
	{
		m_pWorldCamera = &Runtime::ACamera::Get();

		return true;
	}

	void Direct3D11Context::OnUpdate_Impl(const float DeltaMs)
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		static float WorldTime;
		WorldTime += DeltaMs;

		// Send Per-Frame Data to GPU
		/*XMFLOAT4X4 viewFloat;
		XMStoreFloat4x4(&viewFloat, XMMatrixTranspose(m_pWorldCamera->GetViewMatrix()));
		XMFLOAT4X4 projectionFloat;
		XMStoreFloat4x4(&projectionFloat, XMMatrixTranspose(m_pWorldCamera->GetProjectionMatrix()));
		m_PerFrameData.Data.DeltaMs = DeltaMs;
		m_PerFrameData.Data.view = viewFloat;
		m_PerFrameData.Data.projection = projectionFloat;*/
		m_PerFrameData.Data.CameraPosition = m_pWorldCamera->GetPosition();
		m_PerFrameData.Data.DeltaMs = DeltaMs;
		m_PerFrameData.Data.WorldTime = WorldTime;
		m_PerFrameData.Data.RayTraceEnabled = 0.0f;
		m_PerFrameData.Data.CameraNearZ = m_pWorldCamera->GetNearZ();
		m_PerFrameData.Data.CameraFarZ = m_pWorldCamera->GetFarZ();
		m_PerFrameData.Data.CameraExposure = m_pWorldCamera->GetExposure();
		m_PerFrameData.Data.NumPointLights = (float)m_PointLights.size();
		m_PerFrameData.Data.NumDirectionalLights = (m_pWorldDirectionalLight != nullptr) ? 1.0f : 0.0f;
		m_PerFrameData.Data.NumSpotLights = (float)m_SpotLights.size();
		m_PerFrameData.Data.ScreenSize.x = (float)m_WindowWidth;
		m_PerFrameData.Data.ScreenSize.y = (float)m_WindowHeight;
		m_PerFrameData.SubmitToGPU();

		// Send Point Lights to GPU
		if (m_PointLights.size() == 0) {
			m_LightData.Data.PointLights[0] = CB_PS_PointLight{};
		}
		else {
			for (int i = 0; i < m_PointLights.size(); i++) {
				m_LightData.Data.PointLights[i] = m_PointLights[i]->GetConstantBuffer();
			}
		}

		// Send Directionl Light to GPU
		if (m_pWorldDirectionalLight == nullptr) {
			m_LightData.Data.DirectionalLight = CB_PS_DirectionalLight{};
		}
		else {
			m_LightData.Data.DirectionalLight = m_pWorldDirectionalLight->GetConstantBuffer();
		}

		// Send Spot Lights to GPU
		if (m_SpotLights.size() == 0) {
			m_LightData.Data.SpotLights[0] = CB_PS_SpotLight{};
		}
		else {
			for (int i = 0; i < m_SpotLights.size(); i++) {
				m_LightData.Data.SpotLights[i] = m_SpotLights[i]->GetConstantBuffer();
			}
		}
		m_LightData.SubmitToGPU();

		// Send Post-Fx data to GPU
		if (m_pPostFx) {
			m_PostFxData.Data = m_pPostFx->GetConstantBuffer();
		}
		else {
			m_PostFxData.Data = CB_PS_PostFx{};
		}
		m_PostFxData.SubmitToGPU();
	}

	void Direct3D11Context::OnPreFrameRender_Impl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView.Get(), m_ClearColor);

		// Set Persistant Pass Properties
		m_pDeviceContext->RSSetViewports(1, &m_ScenePassViewPort);
		m_pDeviceContext->RSSetScissorRects(1, &m_ScenePassScissorRect);
		m_pDeviceContext->PSSetSamplers(0, 1, m_pPointClamp_SamplerState.GetAddressOf());
		m_pDeviceContext->PSSetSamplers(1, 1, m_pLinearWrap_SamplerState.GetAddressOf());
		m_DeferredShadingTech.PrepPipelineForRenderPass();
	}

	void Direct3D11Context::OnRender_Impl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		// TODO Shadow Pass

		// Geometry Pass
		m_DeferredShadingTech.BindGeometryPass();
		m_pDeviceContext->VSSetConstantBuffers(1, 1, m_PerFrameData.GetAddressOf());
		m_pDeviceContext->PSSetConstantBuffers(1, 1, m_PerFrameData.GetAddressOf());
		GeometryManager::Render(eRenderPass::RenderPass_Scene);
	}

	void Direct3D11Context::OnMidFrameRender_Impl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		// Light Pass
		if (m_pSkyLight) {
			m_pSkyLight->BindCubeMaps(true);
		}
		m_pDeviceContext->PSSetConstantBuffers(2, 1, m_LightData.GetAddressOf());
		m_DeferredShadingTech.BindLightPass();

		// Sky Pass
		if (m_pSkySphere) {
			m_DeferredShadingTech.BindSkyPass();
			m_pSkySphere->RenderSky();
		}

		// Transparency Pass
		if (m_pSkyLight) {
			m_pSkyLight->BindCubeMaps(false);
		}
		m_pDeviceContext->VSSetConstantBuffers(1, 1, m_PerFrameData.GetAddressOf());
		m_pDeviceContext->PSSetConstantBuffers(1, 1, m_PerFrameData.GetAddressOf());
		m_pDeviceContext->PSSetConstantBuffers(2, 1, m_LightData.GetAddressOf());
		m_DeferredShadingTech.BindTransparencyPass();
		GeometryManager::Render(eRenderPass::RenderPass_Transparency);

		// PostFx Pass
		m_pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), nullptr);
		m_pDeviceContext->PSSetConstantBuffers(3, 1, m_PostFxData.GetAddressOf());
		m_pDeviceContext->PSSetConstantBuffers(1, 1, m_PerFrameData.GetAddressOf());
		m_DeferredShadingTech.BindPostFxPass();
	}

	void Direct3D11Context::ExecuteDraw_Impl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;


	}

	void Direct3D11Context::SwapBuffers_Impl()
	{
		RETURN_IF_WINDOW_NOT_VISIBLE;

		UINT PresentFlags = (m_AllowTearing && m_WindowedMode) ? DXGI_PRESENT_ALLOW_TEARING : 0;
		HRESULT hr = m_pSwapChain->Present(m_VSyncEnabled, PresentFlags);
		ThrowIfFailed(hr, "Failed to present frame for D3D 11 context.");
	}

	void Direct3D11Context::OnWindowResize_Impl()
	{
		if (!m_IsMinimized) {

			if (m_WindowResizeComplete) {

				m_WindowResizeComplete = false;
				HRESULT hr;

				m_pRenderTargetView.Reset();
				m_pBackBuffer.Reset();

				DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
				m_pSwapChain->GetDesc(&SwapChainDesc);
				hr = m_pSwapChain->ResizeBuffers(m_FrameBufferCount, m_WindowWidth, m_WindowHeight, SwapChainDesc.BufferDesc.Format, SwapChainDesc.Flags);
				ThrowIfFailed(hr, "Failed to resize swap chain buffers for D3D 11 context.");

				BOOL fullScreenState;
				m_pSwapChain->GetFullscreenState(&fullScreenState, nullptr);
				m_WindowedMode = !fullScreenState;

				UpdateSizeDependentResources();
			}
		}
		m_WindowVisible = !m_IsMinimized;
		m_WindowResizeComplete = true;
	}

	void Direct3D11Context::OnWindowFullScreen_Impl()
	{
		if (m_FullScreenMode)
		{
			SetWindowLong(*m_pWindowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW);

			SetWindowPos(
				*m_pWindowHandle,
				HWND_NOTOPMOST,
				m_pWindow->GetWindowRect().left,
				m_pWindow->GetWindowRect().top,
				m_pWindow->GetWindowRect().right - m_pWindow->GetWindowRect().left,
				m_pWindow->GetWindowRect().bottom - m_pWindow->GetWindowRect().top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE
			);
			ShowWindow(*m_pWindowHandle, SW_NORMAL);
		}
		else
		{
			GetWindowRect(*m_pWindowHandle, &m_pWindow->GetWindowRect());

			SetWindowLong(*m_pWindowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

			RECT FullscreenWindowRect;
			try
			{
				if (m_pSwapChain)
				{
					// Get the settings of the display on which the app's window is currently displayed
					ComPtr<IDXGIOutput> pOutput;
					ThrowIfFailed(m_pSwapChain->GetContainingOutput(&pOutput), "Failed to get containing output while switching to fullscreen mode in D3D 12 context.");
					DXGI_OUTPUT_DESC Desc;
					ThrowIfFailed(pOutput->GetDesc(&Desc), "Failed to get description from output while switching to fullscreen mode in D3D 12 context.");
					FullscreenWindowRect = Desc.DesktopCoordinates;
				}
				else
				{
					// Fallback to EnumDisplaySettings _Implementation
					throw COMException(NULL, "No Swap chain available", __FILE__, __FUNCTION__, __LINE__);
				}
			}
			catch (COMException& e)
			{
				UNREFERENCED_PARAMETER(e);

				// Get the settings of the primary display
				DEVMODE DevMode = {};
				DevMode.dmSize = sizeof(DEVMODE);
				EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &DevMode);

				FullscreenWindowRect = {
					DevMode.dmPosition.x,
					DevMode.dmPosition.y,
					DevMode.dmPosition.x + static_cast<LONG>(DevMode.dmPelsWidth),
					DevMode.dmPosition.y + static_cast<LONG>(DevMode.dmPelsHeight)
				};
			}

			SetWindowPos(
				*m_pWindowHandle,
				HWND_TOPMOST,
				FullscreenWindowRect.left,
				FullscreenWindowRect.top,
				FullscreenWindowRect.right,
				FullscreenWindowRect.bottom,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);


			ShowWindow(*m_pWindowHandle, SW_MAXIMIZE);
		}
		m_FullScreenMode = !m_FullScreenMode;
	}

	void Direct3D11Context::OnShaderReload_Impl()
	{
		m_DeferredShadingTech.ReloadShaders();
	}

	void Direct3D11Context::UpdateSizeDependentResources()
	{
		UpdateViewAndScissor();

		// Re-Create Render Target View
		{
			HRESULT hr;
			hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(m_pBackBuffer.GetAddressOf()));
			ThrowIfFailed(hr, "Failed to get the back buffer from the swapchain for D3D 11 context during window resize.");

			hr = m_pDevice->CreateRenderTargetView(m_pBackBuffer.Get(), NULL, m_pRenderTargetView.GetAddressOf());
			ThrowIfFailed(hr, "Failed to create render target view for D3D 11 context during window resize.");
		}

		// Re-Create GBuffer
		{
			m_DeferredShadingTech.Destroy();
			m_DeferredShadingTech.Init(m_pDevice.Get(), m_pDeviceContext.Get(), m_pWindow);
		}

		// Recreate Camera Projection Matrix
		{
			if (!m_pWorldCamera->GetIsOrthographic()) {
				m_pWorldCamera->SetPerspectiveProjectionValues(m_pWorldCamera->GetFOV(), static_cast<float>(m_WindowWidth) / static_cast<float>(m_WindowHeight), m_pWorldCamera->GetNearZ(), m_pWorldCamera->GetFarZ());
			}
		}
	}

	void Direct3D11Context::UpdateViewAndScissor()
	{
		m_ScenePassViewPort.TopLeftX = 0.0f;
		m_ScenePassViewPort.TopLeftY = 0.0f;
		m_ScenePassViewPort.Width = static_cast<FLOAT>(m_WindowWidth);
		m_ScenePassViewPort.Height = static_cast<FLOAT>(m_WindowHeight);

		m_ScenePassScissorRect.left = static_cast<LONG>(m_ScenePassViewPort.TopLeftX);
		m_ScenePassScissorRect.right = static_cast<LONG>(m_ScenePassViewPort.TopLeftX + m_ScenePassViewPort.Width);
		m_ScenePassScissorRect.top = static_cast<LONG>(m_ScenePassViewPort.TopLeftY);
		m_ScenePassScissorRect.bottom = static_cast<LONG>(m_ScenePassViewPort.TopLeftX + m_ScenePassViewPort.Height);
	}






	// Initializaion
	// --------------

	void Direct3D11Context::CreateDXGIFactory()
	{
		HRESULT hr = ::CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(m_pDxgiFactory.GetAddressOf()));
		ThrowIfFailed(hr, "Failed to create Dxgi Factor for DirectX 11.");
	}

	void Direct3D11Context::GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter** ppAdapter)
	{
		HRESULT hr;

		ComPtr<IDXGIAdapter> pAdapter;
		*ppAdapter = nullptr;
		uint32_t CurrentVideoCardMemory = 0;
		DXGI_ADAPTER_DESC Desc;

		for (uint32_t AdapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters(AdapterIndex, &pAdapter); AdapterIndex++)
		{
			Desc = {};
			pAdapter->GetDesc(&Desc);

			// Make sure we get the video card that is not a software adapter
			// and it has the most video memory. Likly a software adapter if 
			// the device has no system or dedicated video memory.
			bool IsSoftwareAdapter = ((Desc.DedicatedVideoMemory | Desc.DedicatedSystemMemory) == 0);
			if ((Desc.DedicatedVideoMemory < CurrentVideoCardMemory) || IsSoftwareAdapter) {
				continue;
			}
			hr = ::D3D11CreateDevice(pAdapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, NULL, NULL, nullptr, 0, D3D11_SDK_VERSION, NULL, NULL, NULL);
			if (SUCCEEDED(hr)) {

				CurrentVideoCardMemory = static_cast<uint32_t>(Desc.DedicatedSystemMemory);
				if (*ppAdapter != nullptr) {
					(*ppAdapter)->Release();
				}
				*ppAdapter = pAdapter.Detach();

				RN_CORE_WARN("Found suitable Direct3D 11 graphics hardware: {0}", StringHelper::WideToString(Desc.Description));
			}
		}
		Desc = {};
		(*ppAdapter)->GetDesc(&Desc);
		RN_CORE_WARN("\"{0}\" selected as Direct3D 11 graphics hardware.", StringHelper::WideToString(Desc.Description));
	}

	void Direct3D11Context::CreateDeviceAndSwapChain()
	{
		HRESULT hr;
		GetHardwareAdapter(m_pDxgiFactory.Get(), &m_pAdapter);

		UINT DeviceCreateFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined RN_DEBUG
		DeviceCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
		if (m_DeviceMaxSupportedFeatureLevel >= D3D_FEATURE_LEVEL_11_1) {
			DeviceCreateFlags |= D3D11_CREATE_DEVICE_DEBUGGABLE;
		}
#endif
		m_SampleDesc = {};
		m_SampleDesc.Count = 1;
		m_SampleDesc.Quality = 0;

		// TODO Query for HDR support
		DXGI_SWAP_CHAIN_DESC SwapChainDesc = { };
		SwapChainDesc.BufferDesc.Width = m_WindowWidth;
		SwapChainDesc.BufferDesc.Height = m_WindowHeight;
		SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		SwapChainDesc.SampleDesc = m_SampleDesc;
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.BufferCount = static_cast<UINT>(m_FrameBufferCount);
		SwapChainDesc.OutputWindow = *m_pWindowHandle;
		SwapChainDesc.Windowed = TRUE;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		SwapChainDesc.Flags = m_AllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		hr = ::D3D11CreateDeviceAndSwapChain(
			m_pAdapter.Get(),
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL,
			DeviceCreateFlags,
			NULL,
			0,
			D3D11_SDK_VERSION,
			&SwapChainDesc,
			&m_pSwapChain,
			&m_pDevice,
			NULL,
			&m_pDeviceContext
		);
		ThrowIfFailed(hr, "Failed to create swapchain for D3D 11 context");

		if (m_AllowTearing) {
			ThrowIfFailed(m_pDxgiFactory->MakeWindowAssociation(*m_pWindowHandle, DXGI_MWA_NO_ALT_ENTER), "Failed to make window association for D3D 11 context.");
		}
	}

	void Direct3D11Context::CreateRTV()
	{
		HRESULT hr;
		hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(m_pBackBuffer.GetAddressOf()));
		ThrowIfFailed(hr, "Failed to get the back buffer from the swapchain for D3D 11 context.");

		hr = m_pDevice->CreateRenderTargetView(m_pBackBuffer.Get(), NULL, m_pRenderTargetView.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create render target view for D3D 11 context.");
	}

	void Direct3D11Context::CreateConstantBufferViews()
	{
		m_PerFrameData.Init(m_pDevice.Get(), m_pDeviceContext.Get());
		m_LightData.Init(m_pDevice.Get(), m_pDeviceContext.Get());
		m_PostFxData.Init(m_pDevice.Get(), m_pDeviceContext.Get());
	}

	void Direct3D11Context::CreateViewports()
	{
		m_ScenePassViewPort = {};
		m_ScenePassViewPort.TopLeftX = 0.0f;
		m_ScenePassViewPort.TopLeftY = 0.0f;
		m_ScenePassViewPort.Width = static_cast<FLOAT>(m_pWindow->GetWidth());
		m_ScenePassViewPort.Height = static_cast<FLOAT>(m_pWindow->GetHeight());
		m_ScenePassViewPort.MinDepth = 0.0f;
		m_ScenePassViewPort.MaxDepth = 1.0f;
	}

	void Direct3D11Context::CreateScissorRect()
	{
		m_ScenePassScissorRect.left = 0;
		m_ScenePassScissorRect.top = 0;
		m_ScenePassScissorRect.right = m_WindowWidth;
		m_ScenePassScissorRect.bottom = m_WindowHeight;
	}

	void Direct3D11Context::CreateSamplers()
	{
		D3D11_SAMPLER_DESC SamplerPointClampDesc = {};
		SamplerPointClampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		SamplerPointClampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamplerPointClampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamplerPointClampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		SamplerPointClampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		SamplerPointClampDesc.MinLOD = 0.0f;
		SamplerPointClampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		HRESULT hr = m_pDevice->CreateSamplerState(&SamplerPointClampDesc, m_pPointClamp_SamplerState.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create linear wrap sampler for D3D11 context.");

		D3D11_SAMPLER_DESC SamplerLinearWrapDesc = {};
		SamplerLinearWrapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SamplerLinearWrapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerLinearWrapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerLinearWrapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerLinearWrapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		SamplerLinearWrapDesc.MinLOD = 0.0f;
		SamplerLinearWrapDesc.MaxLOD = 9.0f;
		SamplerLinearWrapDesc.MipLODBias = m_GraphicsSettings.MipLodBias;
		SamplerLinearWrapDesc.MaxAnisotropy = m_GraphicsSettings.MaxAnisotropy;
		hr = m_pDevice->CreateSamplerState(&SamplerLinearWrapDesc, m_pLinearWrap_SamplerState.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create linear wrap sampler for D3D11 context.");
	}

	void Direct3D11Context::LoadAssets()
	{
	}


}