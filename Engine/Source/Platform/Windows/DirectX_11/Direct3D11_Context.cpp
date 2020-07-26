#include <ie_pch.h>

#include "Direct3D11_Context.h"

#include "Insight/Core/Application.h"
#include "Platform/Windows/Windows_Window.h"
#include "Platform/Windows/DirectX_11/Geometry/D3D11_Index_Buffer.h"
#include "Platform/Windows/DirectX_11/Geometry/D3D11_Vertex_Buffer.h"

#include "Insight/Runtime/APlayer_Character.h"
#include "Insight/Systems/Managers/Geometry_Manager.h"

#include "Insight/Rendering/APost_Fx.h"
#include "Insight/Rendering/ASky_Light.h"
#include "Insight/Rendering/ASky_Sphere.h"
#include "Insight/Rendering/Lighting/ASpot_Light.h"
#include "Insight/Rendering/Lighting/APoint_Light.h"
#include "Insight/Rendering/Lighting/ADirectional_Light.h"

namespace Insight {




	Direct3D11Context::Direct3D11Context(WindowsWindow* WindowHandle)
		: m_pWindowHandle(&WindowHandle->GetWindowHandleReference()),
		m_pWindow(WindowHandle),
		Renderer(WindowHandle->GetWidth(), WindowHandle->GetHeight(), false)
	{
	}

	Direct3D11Context::~Direct3D11Context()
	{
	}

	bool Direct3D11Context::InitImpl()
	{
		IE_CORE_INFO("Renderer: D3D 11");

		CreateDXGIFactory();
		CreateDeviceAndSwapChain();
		CreateRTVs();
		CreateDSV();
		CreateConstantBufferViews();
		InitShadersLayout();
		CreateViewports();
		CreateRasterizer();
		CreateSamplers();
		
		m_DeferredShadingTech.Init(m_pDevice.Get(), m_pDeviceContext.Get(), m_pWindow);
		LoadAssets();

		return true;
	}

	void Direct3D11Context::SetVertexBuffersImpl(uint32_t StartSlot, uint32_t NumBuffers, ieVertexBuffer* pBuffers)
	{
		m_pDeviceContext->IASetVertexBuffers(StartSlot, NumBuffers, reinterpret_cast<D3D11VertexBuffer*>(pBuffers)->GetBufferPtr(), reinterpret_cast<D3D11VertexBuffer*>(pBuffers)->GetStridePtr(), reinterpret_cast<D3D11VertexBuffer*>(pBuffers)->GetBufferOffset());
	}

	void Direct3D11Context::SetIndexBufferImpl(ieIndexBuffer* pBuffer)
	{
		m_pDeviceContext->IASetIndexBuffer(reinterpret_cast<D3D11IndexBuffer*>(pBuffer)->GetBufferPtr(), reinterpret_cast<D3D11IndexBuffer*>(pBuffer)->GetFormat(), reinterpret_cast<D3D11IndexBuffer*>(pBuffer)->GetBufferOffset());
	}

	void Direct3D11Context::DrawIndexedInstancedImpl(uint32_t IndexCountPerInstance, uint32_t NumInstances, uint32_t StartIndexLocation, uint32_t BaseVertexLoaction, uint32_t StartInstanceLocation)
	{
		m_pDeviceContext->DrawIndexed(IndexCountPerInstance, StartIndexLocation, BaseVertexLoaction);
	}

	void Direct3D11Context::RenderSkySphereImpl()
	{
	}

	bool Direct3D11Context::CreateSkyboxImpl()
	{
		return false;
	}

	void Direct3D11Context::DestroySkyboxImpl()
	{
	}

	void Direct3D11Context::DestroyImpl()
	{
	}

	bool Direct3D11Context::PostInitImpl()
	{
		m_pWorldCamera = &ACamera::Get();

		return true;
	}

	void Direct3D11Context::OnUpdateImpl(const float DeltaMs)
	{
		XMFLOAT4X4 viewFloat;
		XMStoreFloat4x4(&viewFloat, XMMatrixTranspose(m_pWorldCamera->GetViewMatrix()));
		XMFLOAT4X4 projectionFloat;
		XMStoreFloat4x4(&projectionFloat, XMMatrixTranspose(m_pWorldCamera->GetProjectionMatrix()));

		m_PerFrameData.Data.deltaMs = DeltaMs;
		m_PerFrameData.Data.time = static_cast<float>(Application::Get().GetFrameTimer().Seconds());
		m_PerFrameData.Data.view = viewFloat;
		m_PerFrameData.Data.projection = projectionFloat;
		m_PerFrameData.SubmitToGPU();

	}

	void Direct3D11Context::OnPreFrameRenderImpl()
	{
		// Set Persistant Pass Properties
		m_pDeviceContext->RSSetViewports(1, &m_ScenePassViewport);
		m_pDeviceContext->RSSetState(m_pRasterizarState.Get());
		m_pDeviceContext->PSSetSamplers(0, 1, m_pSamplerStatePointClamp.GetAddressOf());
		m_pDeviceContext->PSSetSamplers(1, 1, m_pSamplerStateLinearWrap.GetAddressOf());
	}

	void Direct3D11Context::OnRenderImpl()
	{
		// Geometry Pass
		m_DeferredShadingTech.BindGeometryPass();
		m_pDeviceContext->VSSetConstantBuffers(1, 1, m_PerFrameData.GetAddressOf());
		GeometryManager::Render(eRenderPass::RenderPass_Scene);

		// Light Pass
		m_pDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), nullptr);
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView.Get(), m_ClearColor);
		m_DeferredShadingTech.BindLightPass();


	}

	void Direct3D11Context::OnMidFrameRenderImpl()
	{
	}

	void Direct3D11Context::ExecuteDrawImpl()
	{
	}

	void Direct3D11Context::SwapBuffersImpl()
	{
		UINT PresentFlags = (m_AllowTearing && m_WindowedMode) ? DXGI_PRESENT_ALLOW_TEARING : 0;
		HRESULT hr = m_pSwapChain->Present(m_VSyncEnabled, PresentFlags);
		ThrowIfFailed(hr, "Failed to present frame for D3D 11 context.");
	}

	void Direct3D11Context::OnWindowResizeImpl()
	{
	}

	void Direct3D11Context::OnWindowFullScreenImpl()
	{
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

				IE_CORE_WARN("Found suitable Direct3D 11 graphics hardware: {0}", StringHelper::WideToString(Desc.Description));
			}
		}
		Desc = {};
		(*ppAdapter)->GetDesc(&Desc);
		IE_CORE_WARN("\"{0}\" selected as Direct3D 11 graphics hardware.", StringHelper::WideToString(Desc.Description));
	}

	void Direct3D11Context::CreateDeviceAndSwapChain()
	{
		HRESULT hr;
		GetHardwareAdapter(m_pDxgiFactory.Get(), &m_pAdapter);

		UINT DeviceCreateFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined IE_DEBUG
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

	void Direct3D11Context::CreateRTVs()
	{
		HRESULT hr;
		hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(m_pBackBuffer.GetAddressOf()));
		ThrowIfFailed(hr, "Failed to get the back buffer from the swapchain for D3D 11 context.");

		hr = m_pDevice->CreateRenderTargetView(m_pBackBuffer.Get(), NULL, m_pRenderTargetView.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create render target view for D3D 11 context.");
	}

	void Direct3D11Context::CreateDSV()
	{
		D3D11_DEPTH_STENCIL_DESC DSVDesc = {};
		DSVDesc.DepthEnable = TRUE;
		DSVDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		DSVDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		m_pDevice->CreateDepthStencilState(&DSVDesc, m_pDepthStencilState.GetAddressOf());

	}

	void Direct3D11Context::CreateConstantBufferViews()
	{
		m_PerFrameData.Init(m_pDevice.Get(), m_pDeviceContext.Get());

	}

	void Direct3D11Context::CreateViewports()
	{
		m_ScenePassViewport = {};
		m_ScenePassViewport.TopLeftX = 0.0f;
		m_ScenePassViewport.TopLeftY = 0.0f;
		m_ScenePassViewport.Width = static_cast<FLOAT>(m_pWindow->GetWidth());
		m_ScenePassViewport.Height = static_cast<FLOAT>(m_pWindow->GetHeight());
		m_ScenePassViewport.MinDepth = 0.0f;
		m_ScenePassViewport.MaxDepth = 1.0f;
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
		HRESULT hr = m_pDevice->CreateSamplerState(&SamplerPointClampDesc, m_pSamplerStatePointClamp.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create linear wrap sampler for D3D11 context.");

		D3D11_SAMPLER_DESC SamplerLinearWrapDesc = {};
		SamplerLinearWrapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		SamplerLinearWrapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerLinearWrapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerLinearWrapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		SamplerLinearWrapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		SamplerLinearWrapDesc.MinLOD = 0.0f;
		SamplerLinearWrapDesc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = m_pDevice->CreateSamplerState(&SamplerLinearWrapDesc, m_pSamplerStateLinearWrap.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create linear wrap sampler for D3D11 context.");
	}

	void Direct3D11Context::LoadAssets()
	{
	}

	void Direct3D11Context::InitShadersLayout()
	{

	}

	void Direct3D11Context::CreateRasterizer()
	{
		D3D11_RASTERIZER_DESC RasterizerDesc = {};
		RasterizerDesc.FillMode = D3D11_FILL_SOLID;
		RasterizerDesc.CullMode = D3D11_CULL_BACK;
		HRESULT hr = m_pDevice->CreateRasterizerState(&RasterizerDesc, m_pRasterizarState.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create rasterizer state for D3D11 context");
	}


}