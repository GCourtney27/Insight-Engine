#pragma once
#include "AdapterReader.h"
#include "Shaders.h"
#include "Camera3D.h"
#include "Light.h"
#include "Camera2D.h"
#include "Sprite.h"
#include "..\\Objects\\Entity.h"
#include "..\\Framework\\Singleton.h"
#include "..\\Engine.h"
#include "..\\Systems\\Timer.h"
#include "Material.h"
#include "..\Objects\PointLight.h"

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

	Camera3D editorCamera;
	Camera3D gameCamera;

	Camera2D camera2D;
	Sprite sprite;
	//Light light;
	PointLight * pointLight = nullptr;
	std::vector<std::string> textures;

	Material* m_pMaterial = nullptr;

	Entity* skybox = nullptr;
	ImGuiIO* pImGuiIO = nullptr;

	ID3D11Device* GetDevice() { return pDevice.Get(); }
	ID3D11DeviceContext* GetDeviceContext() { return pDeviceContext.Get(); }

	ConstantBuffer<CB_VS_vertexshader>& GetDefaultVertexShader() { return cb_vs_vertexshader; }

	void InitSkybox();
	void IBLPreCompute();

private:
	bool InitializeDirectX(HWND hwnd);
	bool InitializeShaders();
	bool InitializeScene();
	void UpdateImGuiWidgets();

	Engine* m_pEngine;
	Timer fpsTimer;

	// -- 2D Shaders -- //
	VertexShader vertexshader_2d;
	PixelShader pixelshader_2d;
	ConstantBuffer<CB_VS_vertexshader_2d> cb_vs_vertexshader_2d;

	// -- 3D Shaders -- //
	// When adding more shaders:
	// 1. Add them here as a PixelShader or vertex shader
	// 2. make sure to create new constant buffers for pixel and vertex shaders if you shaders to different things
	// 3. Initialize them in InitializeShaders()
	// 4. Create new ps or vs file for them
	// 5. Set the shaders and new ConstantBuffer/s(if any) in RenderFrame()
	// 6. Set them back if other objects require different shaders to be rendered
	//		- eg. Skybox might need a different shader compared to a model shader(Doesnt take imputs from lights etc,)
	//		- This is why floiage is batch drawn becasue they use different shaders compared to say rocks
	VertexShader default_vertexshader;
	PixelShader default_pixelshader;

	ConstantBuffer<CB_VS_Sky> cb_vs_sky;
	VertexShader skyVertexShader;
	PixelShader skyPixelShader;

	ID3D11Resource* skyTexture;
	ID3D11ShaderResourceView* skyboxTextureSRV;
	ID3D11Resource* irradianceMap;
	ID3D11ShaderResourceView* irradianceMapSRV;

	//IBL
	ID3D11Texture2D* skyIBLtex;
	//ID3D11RenderTargetView* skyIBLRTV[6];
	ID3D11ShaderResourceView* skyIBLSRV;

	ID3D11Texture2D* envMaptex;
	//ID3D11RenderTargetView* envMapRTV[6];
	ID3D11ShaderResourceView* envMapSRV;

	ID3D11Texture2D* brdfLUTtex;
	//ID3D11RenderTargetView* brdfLUTRTV;
	ID3D11ShaderResourceView* brdfLUTSRV;



	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	ConstantBuffer<CB_PS_light> cb_ps_light;
	ConstantBuffer<CB_PS_perframe> cb_ps_PerFrame;

	float m_deltaTime = 0;

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
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState; // Sampler for pixel shader to read texture data

	std::unique_ptr<DirectX::SpriteBatch> pSpriteBatch;
	std::unique_ptr<DirectX::SpriteFont> pSpriteFont;


	int windowWidth = 0;
	int windowHeight = 0;

};