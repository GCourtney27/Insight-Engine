#include <Renderer_pch.h>

#include "D3D11_Deferred_Tech.h"

#include "Platform/Windows/Windows_Window.h"
#include "Platform/Windows/Error/COM_Exception.h"
#include "Insight/Rendering/Geometry/Vertex.h"
#include "Insight/Systems/File_System.h"

namespace Insight {


	D3D11DeferredShadingTech::D3D11DeferredShadingTech()
	{
	}

	D3D11DeferredShadingTech::~D3D11DeferredShadingTech()
	{
	}

	bool D3D11DeferredShadingTech::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, WindowsWindow* pWindow)
	{
		m_pDevice = pDevice;
		m_pDeviceContext = pDeviceContext;
		m_pWindow = pWindow;

		CreateGeometryPass();
		CreateLightPass();
		CreateSkyPass();
		CreateTransparencyPass();
		CreatePostEffectsPass();

		m_ScreenQuad.Init(pDevice, pDeviceContext);
		return true;
	}

	void D3D11DeferredShadingTech::ReloadShaders()
	{
		m_GeometryPassVS.Reload();
		m_GeometryPassPS.Reload();
		CreateGeometryPass();

		m_LightPassVS.Reload();
		m_LightPassPS.Reload();
		CreateLightPass();

		m_SkyPassVS.Reload();
		m_SkyPassPS.Reload();
		CreateSkyPass();

		m_TransparencyPassVS.Reload();
		m_TransparencyPassPS.Reload();
		CreateTransparencyPass();

		m_PostFxPassVS.Reload();
		m_PostFxPassPS.Reload();
		CreatePostEffectsPass();
	}

	void D3D11DeferredShadingTech::Destroy()
	{
		for (uint8_t i = 0; i < m_NumRTV; ++i) {

			m_GBuffer[i].RenderTargetView.Reset();
			m_GBuffer[i].ShaderResourceView.Reset();
			m_GBuffer[i].Texture.Reset();
		}

		m_DepthStencilView.Reset();
		m_pDepthStencilTexture.Reset();
		m_pSceneDepthView.Reset();
		m_pRasterizarState.Reset();
		m_pDefaultDepthStencilState.Reset();

		m_LightPassResult.RenderTargetView.Reset();
		m_LightPassResult.ShaderResourceView.Reset();
		m_LightPassResult.Texture.Reset();

		m_pSkyPass_DepthStencilState.Reset();
		m_pSkyPass_RasterizarState.Reset();
	}

	void D3D11DeferredShadingTech::PrepPipelineForRenderPass()
	{
		ID3D11ShaderResourceView* NullSRV[1] = { nullptr };

		// Null the skybox SRV
		{
			m_pDeviceContext->PSSetShaderResources(15, 1, NullSRV);
		}

		// Unbind the RTV SRVs from last frame to prepare them as render targets again
		{
			m_pDeviceContext->PSSetShaderResources(0, 1, NullSRV); // Albedo
			m_pDeviceContext->PSSetShaderResources(1, 1, NullSRV); // Normal
			m_pDeviceContext->PSSetShaderResources(2, 1, NullSRV); // Roughness/Metallic/AO
			m_pDeviceContext->PSSetShaderResources(3, 1, NullSRV); // Position
		}

		// Null the scene depth texture
		{
			m_pDeviceContext->PSSetShaderResources(4, 1, NullSRV);
		}

		// Reset the blend stat from the Transparency pass
		{
			ID3D11BlendState* NullBlendState[1] = { nullptr };
			m_pDeviceContext->OMSetBlendState(NullBlendState[0], nullptr, 0xFFFFFFFF);
		}
	}

	void D3D11DeferredShadingTech::BindGeometryPass()
	{
		m_pDeviceContext->IASetInputLayout(m_GeometryPassVS.GetInputLayout());
		m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_pDeviceContext->RSSetState(m_pRasterizarState.Get());
		m_pDeviceContext->OMSetDepthStencilState(m_pDefaultDepthStencilState.Get(), 0);

		// Set render targets
		ID3D11RenderTargetView* RenderTargets[] = {
			m_GBuffer[0].RenderTargetView.Get(), // Albedo
			m_GBuffer[1].RenderTargetView.Get(), // Normal
			m_GBuffer[2].RenderTargetView.Get(), // Roughness/Metallic/AO
			m_GBuffer[3].RenderTargetView.Get(), // Position
		};
		m_pDeviceContext->OMSetRenderTargets(m_NumRTV, RenderTargets, m_DepthStencilView.Get());

		// Clear render targets
		m_pDeviceContext->ClearRenderTargetView(m_GBuffer[0].RenderTargetView.Get(), m_ClearColor);
		m_pDeviceContext->ClearRenderTargetView(m_GBuffer[1].RenderTargetView.Get(), m_ClearColor);
		m_pDeviceContext->ClearRenderTargetView(m_GBuffer[2].RenderTargetView.Get(), m_ClearColor);
		m_pDeviceContext->ClearRenderTargetView(m_GBuffer[3].RenderTargetView.Get(), m_ClearColor);
		m_pDeviceContext->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0U);

		// Set shaders
		m_pDeviceContext->VSSetShader(m_GeometryPassVS.GetShader(), nullptr, 0);
		m_pDeviceContext->PSSetShader(m_GeometryPassPS.GetShader(), nullptr, 0);
	}

	void D3D11DeferredShadingTech::BindLightPass()
	{
		m_pDeviceContext->IASetInputLayout(m_LightPassVS.GetInputLayout());
		m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_pDeviceContext->OMSetRenderTargets(1, m_LightPassResult.RenderTargetView.GetAddressOf(), nullptr);

		m_pDeviceContext->PSSetShaderResources(0, 1, m_GBuffer[0].ShaderResourceView.GetAddressOf()); // Albedo
		m_pDeviceContext->PSSetShaderResources(1, 1, m_GBuffer[1].ShaderResourceView.GetAddressOf()); // Normal
		m_pDeviceContext->PSSetShaderResources(2, 1, m_GBuffer[2].ShaderResourceView.GetAddressOf()); // Roughness/Metallic/AO
		m_pDeviceContext->PSSetShaderResources(3, 1, m_GBuffer[3].ShaderResourceView.GetAddressOf()); // Position
		m_pDeviceContext->PSSetShaderResources(4, 1, m_pSceneDepthView.GetAddressOf()); // Scene Depth

		m_pDeviceContext->OMSetRenderTargets(1, m_LightPassResult.RenderTargetView.GetAddressOf(), nullptr);
		m_pDeviceContext->ClearRenderTargetView(m_LightPassResult.RenderTargetView.Get(), m_ClearColor);

		// Set shaders
		m_pDeviceContext->VSSetShader(m_LightPassVS.GetShader(), nullptr, 0);
		m_pDeviceContext->PSSetShader(m_LightPassPS.GetShader(), nullptr, 0);

		m_ScreenQuad.OnRender();
	}

	void D3D11DeferredShadingTech::BindSkyPass()
	{
		m_pDeviceContext->OMSetDepthStencilState(m_pSkyPass_DepthStencilState.Get(), 0U);
		m_pDeviceContext->IASetInputLayout(m_SkyPassVS.GetInputLayout());
		
		// Unbind the depth stencil SRV from the Pixel Shader
		ID3D11ShaderResourceView* NullSRV[1] = { nullptr };
		m_pDeviceContext->PSSetShaderResources(4, 1, NullSRV); // Scene Depth

		m_pDeviceContext->RSSetState(m_pSkyPass_RasterizarState.Get());
		m_pDeviceContext->OMSetDepthStencilState(m_pSkyPass_DepthStencilState.Get(), 0U);
		m_pDeviceContext->OMSetRenderTargets(1, m_LightPassResult.RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());

		m_pDeviceContext->VSSetShader(m_SkyPassVS.GetShader(), nullptr, 0);
		m_pDeviceContext->PSSetShader(m_SkyPassPS.GetShader(), nullptr, 0);

	}

	void D3D11DeferredShadingTech::BindTransparencyPass()
	{
		//m_pDeviceContext->PSSetShaderResources(4, 1, m_pSceneDepthView.GetAddressOf());
		m_pDeviceContext->OMSetRenderTargets(1, m_LightPassResult.RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());

		m_pDeviceContext->RSSetState(m_pTransparency_RasterizerState.Get());
		m_pDeviceContext->OMSetBlendState(m_pTransparencyPass_BlendState.Get(), nullptr, 0xFFFFFFFF);
		m_pDeviceContext->IASetInputLayout(m_TransparencyPassVS.GetInputLayout());

		m_pDeviceContext->VSSetShader(m_TransparencyPassVS.GetShader(), nullptr, 0);
		m_pDeviceContext->PSSetShader(m_TransparencyPassPS.GetShader(), nullptr, 0);
	}

	void D3D11DeferredShadingTech::BindPostFxPass()
	{
		m_pDeviceContext->OMSetDepthStencilState(m_pDefaultDepthStencilState.Get(), 0U);
		m_pDeviceContext->RSSetState(m_pRasterizarState.Get());
		m_pDeviceContext->IASetInputLayout(m_PostFxPassVS.GetInputLayout());

		m_pDeviceContext->PSSetShaderResources(4, 1, m_pSceneDepthView.GetAddressOf());
		m_pDeviceContext->PSSetShaderResources(15, 1, m_LightPassResult.ShaderResourceView.GetAddressOf());

		m_pDeviceContext->VSSetShader(m_PostFxPassVS.GetShader(), nullptr, 0);
		m_pDeviceContext->PSSetShader(m_PostFxPassPS.GetShader(), nullptr, 0);

		m_ScreenQuad.OnRender();
	}

	void D3D11DeferredShadingTech::CreateGeometryPass()
	{
		HRESULT hr;

		std::wstring ExeDirectory = FileSystem::GetExecutbleDirectoryW();
		std::wstring vertShaderFolder = ExeDirectory + L"../Renderer/Geometry_Pass.vertex.cso";
		LPCWSTR VertexShaderFolder = vertShaderFolder.c_str();
		std::wstring pixShaderFolder = ExeDirectory + L"../Renderer/Geometry_Pass.pixel.cso";
		LPCWSTR PixelShaderFolder = pixShaderFolder.c_str();

		D3D11_INPUT_ELEMENT_DESC InputLayout[5] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0  },
			{ "TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0  },
			{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		};
		m_GeometryPassVS.Init(m_pDevice, VertexShaderFolder, InputLayout, ARRAYSIZE(InputLayout));
		m_GeometryPassPS.Init(m_pDevice, PixelShaderFolder);

		D3D11_TEXTURE2D_DESC TextureDesc = {};
		TextureDesc.Width = m_pWindow->GetWidth();
		TextureDesc.Height = m_pWindow->GetHeight();
		TextureDesc.MipLevels = 1U;
		TextureDesc.ArraySize = 1U;
		TextureDesc.SampleDesc.Count = 1U;
		TextureDesc.Usage = D3D11_USAGE_DEFAULT;
		TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		
		D3D11_RENDER_TARGET_VIEW_DESC RTVDesc = {};
		RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1U;

		for (uint32_t i = 0; i < m_NumRTV; ++i) {
		
			// Create Textures
			TextureDesc.Format = m_RtvFormat[i];
			hr = m_pDevice->CreateTexture2D(&TextureDesc, nullptr, m_GBuffer[i].Texture.GetAddressOf());
			ThrowIfFailed(hr, "Failed to create 2D texture for g-buffer in D3D 11 context.");

			// Create Render Targets
			RTVDesc.Format = m_RtvFormat[i];
			hr = m_pDevice->CreateRenderTargetView(m_GBuffer[i].Texture.Get(), &RTVDesc, m_GBuffer[i].RenderTargetView.GetAddressOf());
			ThrowIfFailed(hr, "Failed to create render target view for g-buffer in D3D 11 context.");

			// Create Shader Resource Views
			SRVDesc.Format = m_RtvFormat[i];
			m_pDevice->CreateShaderResourceView(m_GBuffer[i].Texture.Get(), &SRVDesc, m_GBuffer[i].ShaderResourceView.GetAddressOf());
			ThrowIfFailed(hr, "Failed to create shader resource view for g-buffer in D3D 11 context.");
		}

		D3D11_TEXTURE2D_DESC DepthStencilBufferDesc = {};
		DepthStencilBufferDesc.Width = m_pWindow->GetWidth();
		DepthStencilBufferDesc.Height = m_pWindow->GetHeight();
		DepthStencilBufferDesc.MipLevels = 1U;
		DepthStencilBufferDesc.ArraySize = 1U;
		DepthStencilBufferDesc.Format = m_DsvFormat;
		DepthStencilBufferDesc.SampleDesc.Count = 1U;
		DepthStencilBufferDesc.SampleDesc.Quality = 0U;
		DepthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		DepthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		DepthStencilBufferDesc.CPUAccessFlags = 0U;
		DepthStencilBufferDesc.MiscFlags = 0U;
		hr = m_pDevice->CreateTexture2D(&DepthStencilBufferDesc, nullptr, m_pDepthStencilTexture.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create 2D texture for depth buffer in D3D 11 context.");

		D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc = {};
		DSVDesc.Format = DXGI_FORMAT_D32_FLOAT;
		DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		DSVDesc.Texture2D.MipSlice = 0;
		m_pDevice->CreateDepthStencilView(m_pDepthStencilTexture.Get(), &DSVDesc, m_DepthStencilView.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create depth stencil view for g-buffer in D3D 11 context.");

		D3D11_SHADER_RESOURCE_VIEW_DESC DSVSRVDesc = {};
		DSVSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		DSVSRVDesc.Texture2D.MipLevels = 1U;
		DSVSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
		DSVSRVDesc.Texture2D.MipLevels = 1U;
		DSVSRVDesc.Texture2D.MostDetailedMip = 0;
		hr = m_pDevice->CreateShaderResourceView(m_pDepthStencilTexture.Get(), &DSVSRVDesc, &m_pSceneDepthView);
		ThrowIfFailed(hr, "Faield to create shader resource view for depth stencil.");

		// Create Rasterizer State
		D3D11_RASTERIZER_DESC RasterizerDesc = {};
		RasterizerDesc.FillMode = D3D11_FILL_SOLID;
		RasterizerDesc.CullMode = D3D11_CULL_BACK;
		hr = m_pDevice->CreateRasterizerState(&RasterizerDesc, m_pRasterizarState.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create rasterizer state for D3D11 context");

		D3D11_DEPTH_STENCIL_DESC DSDesc = {};
		DSDesc.DepthEnable = TRUE;
		DSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		DSDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = m_pDevice->CreateDepthStencilState(&DSDesc, m_pDefaultDepthStencilState.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create depthstencil state for D3D11 context");
	}

	void D3D11DeferredShadingTech::CreateLightPass()
	{
		HRESULT hr;

		std::wstring ExeDirectory = FileSystem::GetExecutbleDirectoryW();
		std::wstring vertShaderFolder = ExeDirectory + L"../Renderer/Light_Pass.vertex.cso";
		LPCWSTR VertexShaderFolder = vertShaderFolder.c_str();
		std::wstring pixShaderFolder = ExeDirectory + L"../Renderer/Light_Pass.pixel.cso";
		LPCWSTR PixelShaderFolder = pixShaderFolder.c_str();

		D3D11_INPUT_ELEMENT_DESC InputLayout[2] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		m_LightPassVS.Init(m_pDevice, VertexShaderFolder, InputLayout, ARRAYSIZE(InputLayout));
		m_LightPassPS.Init(m_pDevice, PixelShaderFolder);
		
		D3D11_TEXTURE2D_DESC TextureDesc = {};
		TextureDesc.Width = m_pWindow->GetWidth();
		TextureDesc.Height = m_pWindow->GetHeight();
		TextureDesc.MipLevels = 1U;
		TextureDesc.ArraySize = 1U;
		TextureDesc.SampleDesc.Count = 1U;
		TextureDesc.Usage = D3D11_USAGE_DEFAULT;
		TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		D3D11_RENDER_TARGET_VIEW_DESC RTVDesc = {};
		RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1U;

		// Create Texture
		TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		hr = m_pDevice->CreateTexture2D(&TextureDesc, nullptr, m_LightPassResult.Texture.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create 2D texture for g-buffer in D3D 11 context.");

		// Create Render Target
		RTVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		hr = m_pDevice->CreateRenderTargetView(m_LightPassResult.Texture.Get(), &RTVDesc, m_LightPassResult.RenderTargetView.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create render target view for g-buffer in D3D 11 context.");

		// Create Shader Resource View
		SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		m_pDevice->CreateShaderResourceView(m_LightPassResult.Texture.Get(), &SRVDesc, m_LightPassResult.ShaderResourceView.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create shader resource view for g-buffer in D3D 11 context.");
	}

	void D3D11DeferredShadingTech::CreateSkyPass()
	{
		std::wstring ExeDirectory = FileSystem::GetExecutbleDirectoryW();
		std::wstring vertShaderFolder = ExeDirectory + L"../Renderer/Skybox.vertex.cso";
		LPCWSTR VertexShaderFolder = vertShaderFolder.c_str();
		std::wstring pixShaderFolder = ExeDirectory + L"../Renderer/Skybox.pixel.cso";
		LPCWSTR PixelShaderFolder = pixShaderFolder.c_str();

		D3D11_INPUT_ELEMENT_DESC InputLayout[2] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		m_SkyPassVS.Init(m_pDevice, VertexShaderFolder, InputLayout, ARRAYSIZE(InputLayout));
		m_SkyPassPS.Init(m_pDevice, PixelShaderFolder);

		D3D11_DEPTH_STENCIL_DESC DSVDesc = {};
		DSVDesc.DepthEnable = TRUE;
		DSVDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		DSVDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		m_pDevice->CreateDepthStencilState(&DSVDesc, m_pSkyPass_DepthStencilState.GetAddressOf());

		D3D11_RASTERIZER_DESC RasterizerDesc = {};
		RasterizerDesc.DepthClipEnable = true;
		RasterizerDesc.CullMode = D3D11_CULL_FRONT;
		RasterizerDesc.FillMode = D3D11_FILL_SOLID;
		HRESULT hr = m_pDevice->CreateRasterizerState(&RasterizerDesc, m_pSkyPass_RasterizarState.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create rasterizer state for D3D11 context");

	}

	void D3D11DeferredShadingTech::CreateTransparencyPass()
	{
		std::wstring ExeDirectory = FileSystem::GetExecutbleDirectoryW();
		std::wstring vertShaderFolder = ExeDirectory + L"../Renderer/Transparency_Pass.vertex.cso";
		LPCWSTR VertexShaderFolder = vertShaderFolder.c_str();
		std::wstring pixShaderFolder = ExeDirectory + L"../Renderer/Transparency_Pass.pixel.cso";
		LPCWSTR PixelShaderFolder = pixShaderFolder.c_str();

		D3D11_INPUT_ELEMENT_DESC InputLayout[5] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0  },
			{ "TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0  },
			{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0  },
		};
		m_TransparencyPassVS.Init(m_pDevice, VertexShaderFolder, InputLayout, ARRAYSIZE(InputLayout));
		m_TransparencyPassPS.Init(m_pDevice, PixelShaderFolder);

		D3D11_RASTERIZER_DESC RasterizerDesc = {};
		RasterizerDesc.CullMode = D3D11_CULL_NONE;
		RasterizerDesc.FillMode = D3D11_FILL_SOLID;
		m_pDevice->CreateRasterizerState(&RasterizerDesc, m_pTransparency_RasterizerState.GetAddressOf());

		D3D11_RENDER_TARGET_BLEND_DESC RenderTargetBlendDesc = {};
		RenderTargetBlendDesc.BlendEnable = true;
		RenderTargetBlendDesc.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		RenderTargetBlendDesc.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		RenderTargetBlendDesc.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		RenderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		RenderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		RenderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		RenderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;
		D3D11_BLEND_DESC BlendDesc = {};
		BlendDesc.RenderTarget[0] = RenderTargetBlendDesc;
		m_pDevice->CreateBlendState(&BlendDesc, m_pTransparencyPass_BlendState.GetAddressOf());
	}

	void D3D11DeferredShadingTech::CreatePostEffectsPass()
	{
		std::wstring ExeDirectory = FileSystem::GetExecutbleDirectoryW();
		std::wstring vertShaderFolder = ExeDirectory + L"../Renderer/PostFx.vertex.cso";
		LPCWSTR VertexShaderFolder = vertShaderFolder.c_str();
		std::wstring pixShaderFolder = ExeDirectory + L"../Renderer/PostFx.pixel.cso";
		LPCWSTR PixelShaderFolder = pixShaderFolder.c_str();


		D3D11_INPUT_ELEMENT_DESC InputLayout[2] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		m_PostFxPassVS.Init(m_pDevice, VertexShaderFolder, InputLayout, ARRAYSIZE(InputLayout));
		m_PostFxPassPS.Init(m_pDevice, PixelShaderFolder);
	}

	bool ieD3D11ScreenQuad::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	{
		HRESULT hr;

		// If the device local pointer to the device is not null that means 
		// this instance has already been initialized and a widow resize is 
		// probably taking place. Just return, dont recreate the resourcs.
		/*if (m_pDevice->GetFeatureLevel()) {
			return true;
		}*/
		m_pDevice = pDevice;
		m_pDeviceContext = pDeviceContext;

		// Square
		{
			ScreenSpaceVertex Verticies[] =
			{
				{ ieFloat3{-1.0f, -1.0f, 1.0f}, ieFloat2{0.0f, 1.0f} },// Bottom Left
				{ ieFloat3{-1.0f, 1.0f, 1.0f}, ieFloat2{0.0f, 0.0f} }, // Top Left
				{ ieFloat3{1.0f, 1.0f, 1.0f}, ieFloat2{1.0f, 0.0f} }, // Top Right
				{ ieFloat3{1.0f, -1.0f, 1.0f}, ieFloat2{1.0f, 1.0f} } // Bottom Right
			};
			m_NumVerticis = 4U;

			D3D11_BUFFER_DESC VertexBufferDesc = {};
			VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			VertexBufferDesc.ByteWidth = sizeof(ScreenSpaceVertex) * ARRAYSIZE(Verticies);
			VertexBufferDesc.CPUAccessFlags = 0;
			VertexBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA VertexBufferData = {};
			VertexBufferData.pSysMem = Verticies;

			hr = m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexBufferData, m_pVertexBuffer.GetAddressOf());
			ThrowIfFailed(hr, "Failed to create vertex buffer for D3D11 context.");

			DWORD Indices[] =
			{
				0, 1, 2,
				0, 2, 3
			};
			m_NumIndices = 6U;

			D3D11_BUFFER_DESC IndexBufferDesc = {};
			IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			IndexBufferDesc.ByteWidth = sizeof(DWORD) * ARRAYSIZE(Indices);
			IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			IndexBufferDesc.CPUAccessFlags = 0U;
			IndexBufferDesc.MiscFlags = 0U;

			D3D11_SUBRESOURCE_DATA IndexBufferData = {};
			IndexBufferData.pSysMem = Indices;
			hr = m_pDevice->CreateBuffer(&IndexBufferDesc, &IndexBufferData, m_pIndexBuffer.GetAddressOf());
			ThrowIfFailed(hr, "Failed to create D3D 11 index buffer");
		}

		return true;
	}

	void ieD3D11ScreenQuad::OnRender()
	{
		UINT Offsets = 0U;
		UINT Stride = sizeof(ScreenSpaceVertex);
		m_pDeviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &Stride, &Offsets);
		m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		m_pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}


}


