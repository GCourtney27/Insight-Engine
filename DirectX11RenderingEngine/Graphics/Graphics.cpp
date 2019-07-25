#include "Graphics.h"
#include "../FileLoader.h"

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	fpsTimer.Start();

	if (!InitializeDirectX(hwnd))
		return false;


	if (!InitializeShaders())
		return false;


	if (!InitializeScene())
		return false;

	//selectedGameObject = &gameObject;
	selectedGameObject = m_gameObjects[0];

	// Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device.Get(), deviceContext.Get());
	ImGui::StyleColorsDark();

	return true;
}

void Graphics::RenderFrame()
{
	cb_ps_light.data.dynamicLightColor = light.lightColor;
	cb_ps_light.data.dynamicLightStrength = light.lightStrength;
	cb_ps_light.data.dynamicLightPosition = light.GetPositionFloat3();
	cb_ps_light.data.dynamicLightAttenuation_a = light.attenuation_a;
	cb_ps_light.data.dynamicLightAttenuation_b = light.attenuation_b;
	cb_ps_light.data.dynamicLightAttenuation_c = light.attenuation_c;

	cb_ps_light.ApplyChanges();
	deviceContext->PSSetConstantBuffers(0, 1, cb_ps_light.GetAddressOf());

	float bgcolor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), bgcolor);
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	deviceContext->IASetInputLayout(vertexshader.GetInputLayout());
	deviceContext->IASetInputLayout(testVertexshader.GetInputLayout());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->RSSetState(rasterizerState.Get());
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
	deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
	deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
	deviceContext->VSSetShader(vertexshader.GetShader(), NULL, 0);
	deviceContext->VSSetShader(testVertexshader.GetShader(), NULL, 0);
	deviceContext->PSSetShader(pixelshader.GetShader(), NULL, 0);
	deviceContext->PSSetShader(testPixelshader.GetShader(), NULL, 0);

	UINT offset = 0;

	{
		size_t gos = m_gameObjects.size();
		for (int i = 0; i < gos; i++)
		{
			m_gameObjects[i]->Draw(camera3D.GetViewMatrix() * camera3D.GetProjectionMatrix());
		}
	}
	{ // Pavement cube Texture

		//gameObject.Draw(camera3D.GetViewMatrix() * camera3D.GetProjectionMatrix());
		//test.Draw(camera3D.GetViewMatrix() * camera3D.GetProjectionMatrix());

	}
	{
		deviceContext->PSSetShader(pixelshader_nolight.GetShader(), NULL, 0);
		light.Draw(camera3D.GetViewMatrix() * camera3D.GetProjectionMatrix());
	}
	//gameObject.UpdateAABB();

	deviceContext->IASetInputLayout(vertexshader_2d.GetInputLayout());
	deviceContext->PSSetShader(pixelshader_2d.GetShader(), NULL, 0);
	deviceContext->VSSetShader(vertexshader_2d.GetShader(), NULL, 0);
	sprite.Draw(camera2D.GetWorldmatrix() * camera2D.GetOrthoMatrix());

	// Draw Text
	static int fpsCounter = 0;
	static std::string fpsString = "FPS: 0";
	fpsCounter += 1;
	if (fpsTimer.GetMilisecondsElapsed() > 1000.0)
	{
		fpsString = "FPS: " + std::to_string(fpsCounter);
		fpsCounter = 0;
		fpsTimer.Restart();
	}
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(fpsString).c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();

	static int counter = 0;
	UpdateImGui();
	swapchain->Present(0, NULL); // Enable Vertical sync with 1 or 0

}

bool Graphics::InitializeDirectX(HWND hwnd)
{
	try
	{
		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

		if (adapters.size() < 1)
		{
			ErrorLogger::Log("No IDXGI Adapters found.");
			return false;
		}

		DXGI_SWAP_CHAIN_DESC scd = { 0 };
		scd.BufferDesc.Width = windowWidth;
		scd.BufferDesc.Height = windowHeight;
		scd.BufferDesc.RefreshRate.Numerator = 60;
		scd.BufferDesc.RefreshRate.Denominator = 1;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 1;
		scd.OutputWindow = hwnd;
		scd.Windowed = TRUE;
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		HRESULT hr;
		hr = D3D11CreateDeviceAndSwapChain(adapters[0].pAdapter, //IDXGI Adapter
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL, // NULL for software driver type
			NULL, // Flags for runtime layers
			NULL, // Feature levels array
			0, //NUmber of Feature levels in array
			D3D11_SDK_VERSION,
			&scd, // Swapchain desciption
			swapchain.GetAddressOf(), // Swapchain address
			device.GetAddressOf(), // Device address
			NULL, //Supported feature level
			deviceContext.GetAddressOf()); // Device context address
		COM_ERROR_IF_FAILED(hr, "Failed to create device swap chain.");

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "GetBuffer Failed.");

		hr = device->CreateRenderTargetView(backBuffer.Get(), NULL, renderTargetView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");

		// Describe out Depth/Stencil Buffer
		CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, windowWidth, windowHeight);
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		hr = device->CreateTexture2D(&depthStencilDesc, NULL, depthStencilBuffer.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer Texture2D.");

		hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), NULL, depthStencilView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil view.");

		deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

		// Create depth stencil state
		CD3D11_DEPTH_STENCIL_DESC depthstancildesc(D3D11_DEFAULT); // Z Buffer
		depthstancildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depthstancildesc, depthStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Filaed to create depth stencil state.");

		// Create and set the ViewPort
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight));
		deviceContext->RSSetViewports(1, &viewport);
		
		// Create Rasterizer State
		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
		hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");

		// Create rasterizer State for cull front
		CD3D11_RASTERIZER_DESC rasterizerDesc_CullFront(D3D11_DEFAULT);
		rasterizerDesc_CullFront.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
		hr = device->CreateRasterizerState(&rasterizerDesc_CullFront, rasterizerState_CullFront.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state for cull front.");

		// Create Blend State
		D3D11_BLEND_DESC blendDesc = { 0 };
		D3D11_RENDER_TARGET_BLEND_DESC rtbd = { 0 };
		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[0] = rtbd;
		hr = device->CreateBlendState(&blendDesc, blendState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create blend state.");

		spriteBatch = std::make_unique<DirectX::SpriteBatch>(deviceContext.Get());
		spriteFont = std::make_unique<DirectX::SpriteFont>(device.Get(), L"Data\\Fonts\\comic_sans_ms_16.spritefont");

		// Create sampler description for sampler state
		CD3D11_SAMPLER_DESC sampleDesc(D3D11_DEFAULT);
		sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		hr = device->CreateSamplerState(&sampleDesc, samplerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

bool Graphics::InitializeShaders()
{
	std::wstring shaderfolder = L"";
#pragma region DetermineShaderPath
	//if (IsDebuggerPresent() == TRUE)
	{
#ifdef _DEBUG // Debug Mode
#ifdef _WIN64 //x64
		shaderfolder = L"..\\x64\\Debug\\";
#else // x86 (Win32)
		shaderfolder = L"..\\Debug\\";
#endif
#else // Release Mode
#ifdef _WIN64 //x64
		shaderfolder = L"..\\x64\\Release\\";
#else //x86 (Win32)
		shaderfolder = L"..\\Release\\";
#endif
#endif
	}
	// 2D shaders
	D3D11_INPUT_ELEMENT_DESC layout2D[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  }

	};
	UINT numElements2D = ARRAYSIZE(layout2D);

	if (!vertexshader_2d.Initialize(device, shaderfolder + L"vertexshader_2d.cso", layout2D, numElements2D))
		return false;

	if (!pixelshader_2d.Initialize(device, shaderfolder + L"pixelshader_2d.cso"))
		return false;

	// 3D shaders
	D3D11_INPUT_ELEMENT_DESC layout3D[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  }
	};
	UINT numElements3D = ARRAYSIZE(layout3D);

	if (!vertexshader.Initialize(device, shaderfolder + L"vertexshader.cso", layout3D, numElements3D))
		return false;

	if (!pixelshader.Initialize(device, shaderfolder + L"pixelshader.cso"))
		return false;

	if (!testVertexshader.Initialize(device, shaderfolder + L"vertexshader.cso", layout3D, numElements3D))
		return false;

	if (!testPixelshader.Initialize(device, shaderfolder + L"pixelshader.cso"))
		return false;

	if (!pixelshader_nolight.Initialize(device, shaderfolder + L"pixelshader_nolight.cso"))
		return false;


	return true;
}

bool Graphics::InitializeScene()
{
	try
	{
		// Load Texture
		HRESULT hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Data\\Textures\\Grass_Diff.jpg", nullptr, grassTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize wic Texture from file.");

		hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Data\\Textures\\PinkSquare_Diff.jpeg", nullptr, pinkTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize wic Texture from file.");

		hr = DirectX::CreateWICTextureFromFile(device.Get(), L"Data\\Textures\\TileFloor_Diff.jpg", nullptr, pavementTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize wic Texture from file.");

		// Initialize Constant Buffer(s)
		hr = cb_vs_vertexshader_2d.Initialize(device.Get(), deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer 2d for vertex shader.");

		hr = cb_vs_vertexshader.Initialize(device.Get(), deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer for vertex shader.");

		// Test Vertex Shader
		hr = test_cb_vs_vertexshader.Initialize(device.Get(), deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer for test vertex shader.");

		hr = cb_ps_light.Initialize(device.Get(), deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer for pixel shader.");

		cb_ps_light.data.ambientLightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
		cb_ps_light.data.ambientLightStrength = 1.0f;

		// Initialize Model(s)
		// Cube
		/*if (!gameObject.Initialize("Data\\Objects\\Cube.obj", device.Get(), deviceContext.Get(), cb_vs_vertexshader))
			return false;*/

		/*Working Code*/
		{
			// Dandelion GameObject
		//if (!gameObject.Initialize("Data\\Objects\\Dandelion\\Var1\\Var1_LOD0.fbx", device.Get(), deviceContext.Get(), cb_vs_vertexshader))
		//	return false;
		//gameObject.SetName("gameObject");
		//gameObject.SetPosition(DirectX::XMFLOAT3(45.0f, 0.0f, 0.0f));
		//m_gameObjects.push_back(&gameObject);

		//// Dandelion test
		//if (!test.Initialize("Data\\Objects\\Dandelion\\Var1\\Var1_LOD0.fbx", device.Get(), deviceContext.Get(), cb_vs_vertexshader))
		//	return false;
		//test.SetName("test");
		//m_gameObjects.push_back(&test);
		}
		
		if (!FileLoader::LoadSceneFromFile("Data//Scenes//Scene01.txt", m_gameObjects, device.Get(), deviceContext.Get(), cb_vs_vertexshader))
			return false;

		/*int gos = m_gameObjects.size();
		for (int i = 0; i < gos; i++)
		{
			if (!m_gameObjects[i]->Initialize("Data\\Objects\\Dandelion\\Var1\\Var1_LOD0.fbx", device.Get(), deviceContext.Get(), cb_vs_vertexshader))
				return false;
		}*/

		cb_ps_light.data.ambientLightStrength = 0.268f;

		light.lightStrength = 6.848f;
		light.attenuation_a = 1.968f;
		light.attenuation_b = 0.2f;
		light.attenuation_c = 0.0f;

		// Granite Rock
		/*if (!gameObject.Initialize("Data\\Objects\\GraniteRock\\Aset_rock_granite_M_oheuu_LOD0.fbx", device.Get(), deviceContext.Get(), cb_vs_vertexshader))
			return false;*/
		// Nanosuit
		/*if (!gameObject.Initialize("Data\\Objects\\nanosuit\\nanosuit.obj", device.Get(), deviceContext.Get(), cb_vs_vertexshader))
			return false;*/
		// Light
		if (!light.Initialize(device.Get(), deviceContext.Get(), cb_vs_vertexshader))
			return false;
		// Hello World sprite
		if (!sprite.Initialize(device.Get(), deviceContext.Get(), 256, 256, "Data/Textures/sprite_256x256.png", cb_vs_vertexshader_2d))
			return false;

		camera2D.SetProjectionValues((float)windowWidth, (float)windowHeight, 0.0f, 1.0f);

		camera3D.SetPosition(0.0f, 0.0f, -2.0f);
		camera3D.SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 1000.0f);

	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

void Graphics::UpdateImGui()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Create ImGui Test Window
	ImGui::Begin("Light Controls");
	ImGui::DragFloat3("Ambient Light Color", &cb_ps_light.data.ambientLightColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Ambient Light Strength", &cb_ps_light.data.ambientLightStrength, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat3("Dynamic Light Color", &light.lightColor.x, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Dynamic Light Strength", &light.lightStrength, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("DynamicLight Attenuation A", &light.attenuation_a, 0.01f, 0.1f, 10.0f);
	ImGui::DragFloat("DynamicLight Attenuation B", &light.attenuation_b, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("DynamicLight Attenuation C", &light.attenuation_c, 0.01f, 0.0f, 10.0f);

	ImGui::End();

	/*ImGui::Begin("Game object location");
	ImGui::DragFloat3("Position", &gameObject.GetPosition().x, 0.01f, 0.0f, 100.0f);
	ImGui::End();*/
	std::string goName = selectedGameObject->GetName();
	ImGui::Begin("Selected Game Object transform");
	ImGui::Text(goName.c_str());
	ImGui::DragFloat3("Position", &selectedGameObject->GetPosition().x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat3("Rotation", &selectedGameObject->GetRotation().x, 0.1f, -100.0f, 100.0f);
	ImGui::DragFloat3("Scale", &selectedGameObject->GetScale().x, 0.1f, -100.0f, 100.0f);
	//ImGui::DragFloat3("Position", &test.GetPosition().x, 0.1f, -100.0f, 100.0f);
	ImGui::End();

	// Assemble Together Draw Data
	ImGui::Render();
	// Render Draw Data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


}


