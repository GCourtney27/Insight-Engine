#pragma once
#include "AdapterReader.h"
#include "Shaders.h"
#include "Camera3D.h"
#include "Light.h"
#include "Camera2D.h"
#include "Sprite.h"
#include "..\\Framework\\Singleton.h"
#include "..\\Engine.h"
#include "..\Objects\PointLight.h"
#include "..\Objects\DirectionalLight.h"
#include "MaterialSky.h"

#include "ImGui\\imgui.h"
#include "ImGui\\imgui_impl_win32.h"
#include "ImGui\\imgui_impl_dx11.h"
#include "ImGui/ImGuizmo.h"

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <d3dx11async.h>
#pragma comment (lib, "d3dx11.lib")

class Graphics : public Singleton<Graphics>
{
public:
	bool Initialize(HWND hwnd, int width, int height, Engine* engine);
	void RenderFrame();
	void Update(const float& deltaTime);
	void Shutdown();
	void InitialzeImGui(HWND hwnd);

	Camera3D editorCamera;

	Camera2D camera2D;
	Sprite sprite;

	PointLight * pointLight = nullptr;
	DirectionalLight * directionalLight = nullptr;

	Material* m_pMaterial = nullptr;

	Entity* skybox = nullptr;
	MaterialSky* m_pSkyMaterial = nullptr;
	void SetSkybox(Entity* _skybox) { this->skybox = _skybox; }
	Entity* GetSkybox() { return skybox; }

	ImGuiIO* pImGuiIO = nullptr;

	ID3D11Device* GetDevice() { return pDevice.Get(); }
	ID3D11DeviceContext* GetDeviceContext() { return pDeviceContext.Get(); }

	ConstantBuffer<CB_VS_PerObject>& GetDefaultVertexShader() { return cb_vs_vertexshader; }

	void InitSkybox();

	void LogDrawCall(UINT call) { m_drawCalls += call; }

private:
	bool InitializeDirectX(HWND hwnd);
	bool InitializeShaders();
	bool InitializeScene();
	void UpdateImGuiWidgets();

	Engine* m_pEngine;

	frame_timer m_frameTimer;
	bool m_drawfpsGPU = false;
	bool m_drawfpsCPU = true;
	bool m_drawFrameTimeGPU = false;
	bool m_drawFrameTimeCPU = false;
	bool m_logDrawCalls = true;
	UINT m_drawCalls = 0;
	bool m_drawWireframe = false;

	std::wstring m_shaderFolder;

	// -- 2D Shaders -- //
	VertexShader vertexshader_2d;
	PixelShader pixelshader_2d;
	ConstantBuffer<CB_VS_vertexshader_2d> cb_vs_vertexshader_2d;


	ConstantBuffer<CB_VS_Sky> cb_vs_sky;
	VertexShader skyVertexShader;
	PixelShader skyPixelShader;

	// For skybox, no mip levels becasue we dont want the actual game sky to get blurry
	ID3D11Texture2D* skyTexture;
	ID3D11ShaderResourceView* skyboxTextureSRV;

	ID3D11Texture2D* irradianceMap;
	ID3D11ShaderResourceView* irradianceMapSRV;

	// Sky texture with mip levels
	ID3D11Texture2D* environmentMap;
	ID3D11ShaderResourceView* environmentMapSRV;

	ID3D11Texture2D* brdfLUTtex;
	ID3D11ShaderResourceView* brdfLUTSRV;



	ConstantBuffer<CB_VS_PerObject> cb_vs_vertexshader;
	ConstantBuffer<CB_PS_light> cb_ps_light;
	ConstantBuffer<CB_PS_directionalLight> cb_ps_directionalLight;
	ConstantBuffer<CB_PS_perframe> cb_ps_PerFrame;
	ConstantBuffer<CB_VS_perframe> cb_vs_PerFrame;


	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext;

	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView;
	
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthStencilState;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizerStateCULLNONE;
	Microsoft::WRL::ComPtr<ID3D11BlendState> pBlendState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	std::unique_ptr<DirectX::SpriteBatch> pSpriteBatch;
	std::unique_ptr<DirectX::SpriteFont> pSpriteFont;

	float m_deltaTime = 0;

	int windowWidth = 0;
	int windowHeight = 0;

};