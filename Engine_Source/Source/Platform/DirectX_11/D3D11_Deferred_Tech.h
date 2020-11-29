#pragma once

#include <Insight/Core.h>

#include "Platform/DirectX_11/Wrappers/D3D11_Shader.h"

namespace Insight {

	class Win32Window;

	struct RenderTargetTexture
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture = nullptr;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView = nullptr;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView = nullptr;
	};

	class INSIGHT_API ieD3D11ScreenQuad
	{
	public:
		ieD3D11ScreenQuad() = default;
		~ieD3D11ScreenQuad() = default;
		
		bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
		void OnRender();

	private:
		Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pDeviceContext;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
		uint32_t m_NumVerticis;
		uint32_t m_NumIndices;
	};

	class INSIGHT_API D3D11DeferredShadingTech
	{
	public:
		D3D11DeferredShadingTech();
		~D3D11DeferredShadingTech();
		
		bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, Window* pWindow);
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
		Microsoft::WRL::ComPtr<ID3D11Device>				m_pDevice = nullptr;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>			m_pDeviceContext = nullptr;
		Window*						m_pWindow;
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

		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DepthStencilView = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthStencilTexture = nullptr;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pSceneDepthView = nullptr;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterizarState;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDefaultDepthStencilState = nullptr;

		// Light Pass
		// ----------
		VertexShader m_LightPassVS;
		PixelShader m_LightPassPS;
		RenderTargetTexture m_LightPassResult;

		// Sky Pass
		// --------
		VertexShader m_SkyPassVS;
		PixelShader m_SkyPassPS;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pSkyPass_DepthStencilState;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pSkyPass_RasterizarState;

		// Transparency Pass
		// -----------------
		VertexShader m_TransparencyPassVS;
		PixelShader m_TransparencyPassPS;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pTransparency_RasterizerState;
		Microsoft::WRL::ComPtr<ID3D11BlendState> m_pTransparencyPass_BlendState;

		// PostFx Pass
		// -----------
		VertexShader m_PostFxPassVS;
		PixelShader m_PostFxPassPS;

	};

}
