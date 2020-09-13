#pragma once

#include <Insight/Core.h>

#include "Platform/Windows/DirectX_11/D3D11_Shader.h"

namespace Insight {

	class WindowsWindow;

	struct RenderTargetTexture
	{
		ComPtr<ID3D11Texture2D> Texture = nullptr;
		ComPtr<ID3D11RenderTargetView> RenderTargetView = nullptr;
		ComPtr<ID3D11ShaderResourceView> ShaderResourceView = nullptr;
	};

	class ieD3D11ScreenQuad
	{
	public:
		ieD3D11ScreenQuad() = default;
		~ieD3D11ScreenQuad() = default;
		
		bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		void OnRender();

	private:
		ComPtr<ID3D11Device> m_pDevice;
		ComPtr<ID3D11DeviceContext> m_pDeviceContext;
		ComPtr<ID3D11Buffer> m_pVertexBuffer;
		ComPtr<ID3D11Buffer> m_pIndexBuffer;
		uint32_t m_NumVerticis;
		uint32_t m_NumIndices;
	};

	class INSIGHT_API D3D11DeferredShadingTech
	{
	public:
		D3D11DeferredShadingTech();
		~D3D11DeferredShadingTech();
		
		bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, WindowsWindow* pWindow);
		void Destroy();

		ID3D11ShaderResourceView* GetSceneDepthSRV() { return m_pSceneDepthView.Get(); }
		ID3D11DepthStencilView* GetSceneDepthDSV() { return m_DepthStencilView.Get(); }

		void PrepPipelineForRenderPass();
		void BindGeometryPass();
		void BindLightPass();
		void BindSkyPass();
		void BindTransparencyPass();
		void BindPostFxPass();

		void ReloadShaders();

	private:
		void CreateGeometryPass();
		void CreateLightPass();
		void CreateSkyPass();
		void CreateTransparencyPass();
		void CreatePostEffectsPass();

	private:
		ComPtr<ID3D11Device>				m_pDevice = nullptr;
		ComPtr<ID3D11DeviceContext>			m_pDeviceContext = nullptr;
		WindowsWindow*						m_pWindow;
		float								m_DepthClearValue = 1.0f;
		float								m_ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		static const UINT					m_NumRTV = 4;
		DXGI_FORMAT							m_DsvFormat = DXGI_FORMAT_R32_TYPELESS;
		DXGI_FORMAT							m_RtvFormat[4] = {
			DXGI_FORMAT_R11G11B10_FLOAT,	// Albedo buffer
			DXGI_FORMAT_R8G8B8A8_SNORM,		// Normal
			DXGI_FORMAT_R11G11B10_FLOAT,	// (R)Roughness/(G)Metallic/(B)AO
			DXGI_FORMAT_R32G32B32A32_FLOAT, // Position
		};
		ieD3D11ScreenQuad m_ScreenQuad;

		// Geometry Pass
		// -------------
		VertexShader m_GeometryPassVS;
		PixelShader m_GeometryPassPS;
		//0:  SRV-Albedo(RTV->SRV)
		//1:  SRV-Normal(RTV->SRV)
		//2:  SRV-(R)Roughness/(G)Metallic/(B)AO(RTV->SRV)
		//3:  SRV-Position(RTV->SRV)
		RenderTargetTexture m_GBuffer[m_NumRTV];

		ComPtr<ID3D11DepthStencilView> m_DepthStencilView = nullptr;
		ComPtr<ID3D11Texture2D> m_pDepthStencilTexture = nullptr;
		ComPtr<ID3D11ShaderResourceView> m_pSceneDepthView = nullptr;
		ComPtr<ID3D11RasterizerState> m_pRasterizarState;
		ComPtr<ID3D11DepthStencilState> m_pDefaultDepthStencilState = nullptr;

		// Light Pass
		// ----------
		VertexShader m_LightPassVS;
		PixelShader m_LightPassPS;
		RenderTargetTexture m_LightPassResult;

		// Sky Pass
		// --------
		VertexShader m_SkyPassVS;
		PixelShader m_SkyPassPS;
		ComPtr<ID3D11DepthStencilState> m_pSkyPass_DepthStencilState;
		ComPtr<ID3D11RasterizerState> m_pSkyPass_RasterizarState;

		// Transparency Pass
		// -----------------
		VertexShader m_TransparencyPassVS;
		PixelShader m_TransparencyPassPS;
		ComPtr<ID3D11RasterizerState> m_pTransparency_RasterizerState;
		ComPtr<ID3D11BlendState> m_pTransparencyPass_BlendState;

		// PostFx Pass
		// -----------
		VertexShader m_PostFxPassVS;
		PixelShader m_PostFxPassPS;

	};

}
