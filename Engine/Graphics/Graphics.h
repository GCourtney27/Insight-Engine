#pragma once
#include "AdapterReader.h"
#include "Shaders.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h>
#include "Camera3D.h"
#include "..\\Systems\\Timer.h"
#include "ImGui\\imgui.h"
#include "ImGui\\imgui_impl_win32.h"
#include "ImGui\\imgui_impl_dx11.h"
#include "RenderableGameObject.h"
#include "Light.h"
#include "Camera2D.h"
#include "Sprite.h"
#pragma comment (lib, "d3dx11.lib")

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <D3D10_1.h>
#include <DXGI.h>
#include <D2D1.h>
#include <sstream>
#include <dwrite.h>
#include <dinput.h>
#include <DDSTextureLoader.h>

#include "../Scriptor/Compiler.h"
#include "BaseScriptableGameObject.h"

#include "..\\Objects\\Entity.h"

//class Entity;

class Graphics
{
public:
	bool Initialize(HWND hwnd, int width, int height);
	void RenderFrame();
	void Update();

	Camera3D camera3D;
	Camera2D camera2D;
	Sprite sprite;
	Light light; // uncomment for scene 1. ALL ATTRIBUTES FOR LIGHT HAVE BEEN COMMENTED OUT DONT FORGET TO UNCOMMENT THEM


	//RenderableGameObject* selectedGameObject;

	//std::vector<RenderableGameObject*> m_gameObjects;

	Compiler compiler;

	ID3D11Device* GetDevice() { return device.Get(); }
	ID3D11DeviceContext* GetDeviceContext() { return deviceContext.Get(); }

	ConstantBuffer<CB_VS_vertexshader>& GetDefaultVertexShader() { return cb_vs_vertexshader; }

	void SetScene(Scene* _scene) { scene = _scene; }

private:
	bool InitializeDirectX(HWND hwnd);
	bool InitializeShaders();
	bool InitializeScene();
	void UpdateImGui();

	Scene* scene = nullptr;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	
	VertexShader testVertexshader;
	PixelShader testPixelshader;
	ConstantBuffer<CB_VS_vertexshader> test_cb_vs_vertexshader;
	
	VertexShader vertexshader_2d;
	VertexShader vertexshader;
	PixelShader pixelshader;
	PixelShader pixelshader_2d;
	PixelShader pixelshader_nolight;
	PixelShader pixelshader_foliage;
	ConstantBuffer<CB_VS_vertexshader_2d> cb_vs_vertexshader_2d;
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	
	//ConstantBuffer<CB_PS_pixelshader> cb_ps_pixelshader;
	ConstantBuffer<CB_PS_light> cb_ps_light;


	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState_CullFront;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pinkTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grassTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pavementTexture;

	// Skybox
	XMMATRIX Rotationx;
	XMMATRIX Rotationy;
	XMMATRIX Rotationz;
	XMMATRIX Scale;
	XMMATRIX Translation;
	XMMATRIX WVP;

	ID3D11Buffer* sphereIndexBuffer;
	ID3D11Buffer* sphereVertBuffer;

	VertexShader SKYMAP_VS;
	ConstantBuffer<CB_VS_vertexshader> cb_vs_skybox;
	PixelShader SKYMAP_PS;
	ID3D10Blob* SKYMAP_VS_Buffer;
	ID3D10Blob* SKYMAP_PS_Buffer;

	ID3D11ShaderResourceView* smrv;

	ID3D11DepthStencilState* DSLessEqual;
	ID3D11RasterizerState* RSCullNone;

	int NumSphereVerticies;
	int NumSphereFaces;

	XMMATRIX sphereWorld;

	void CreateSphere(int LatLines, int LongLines);

	int windowWidth = 0;
	int windowHeight = 0;
	Timer fpsTimer;
};