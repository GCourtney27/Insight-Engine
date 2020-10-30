#include <Renderer_pch.h>

#include "Render_Passes.h"

#include "Renderer/Platform/Windows/DirectX_12/Direct3D12_Context.h"
#include "Platform/Windows/Windows_Window.h"
#include "Insight/Systems/Managers/Geometry_Manager.h"

namespace Insight {

	
	ieD3D12ScreenQuad g_ScreenQuad;


	/*===========================*/
	/*		Geometry Pass		 */
	/*===========================*/

	bool DeferredGeometryPass::Set()
	{
		PIXBeginEvent(m_pCommandListRef.Get(), 0, L"Rendering Geometry Pass");
		{
			m_pRenderContextRef->SetActiveCommandList(m_pCommandListRef);

			ID3D12DescriptorHeap* ppHeaps[] = { m_pCBVSRVHeapRef->pDH.Get() };
			m_pCommandListRef->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			// Clear the render targets.
			for(uint8_t i = 0; i < m_NumRenderTargets; ++i)
				m_pCommandListRef->ClearRenderTargetView(m_RTVHeap.hCPU(i), m_ScreenClearColor, 0, nullptr);
			
			// Clear the Depth Stencil View.
			m_pCommandListRef->ClearDepthStencilView(m_DSVHeap.hCPU(0), D3D12_CLEAR_FLAG_DEPTH, m_DepthClearValue, 0xFF, 0, nullptr);
			
			// Make sure we can write to the depth buffer in this pass.
			m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_pSceneDepthStencilTexture.Get(), IE_D3D12_DEFAULT_RESOURCE_STATE, D3D12_RESOURCE_STATE_DEPTH_WRITE);

			// Set the render targets and depth stencil for this pass.
			m_pCommandListRef->OMSetRenderTargets(m_NumRenderTargets, &m_RTVHeap.hCPUHeapStart, TRUE, &m_DSVHeap.hCPU(0));

			// Set the root signature for this pass.
			m_pCommandListRef->SetGraphicsRootSignature(m_pRootSignatureRef.Get());
			
			// Set the main pipeline for this pass.
			m_pCommandListRef->SetPipelineState(m_pPipelineState.Get());

			GeometryManager::Render(RenderPassType::RenderPassType_Scene);
		}
		PIXEndEvent(m_pCommandListRef.Get());

		return false;
	}

	void DeferredGeometryPass::UnSet()
	{
		// After we are finished with the buffers but them in a generic state for other passes to use them.
		for (uint8_t i = 0; i < m_NumRenderTargets; ++i)
			m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_pRenderTargetTextures[i].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, IE_D3D12_DEFAULT_RESOURCE_STATE);

		// Set the depth buffer to a generic state for others to use.
		m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_pSceneDepthStencilTexture.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, IE_D3D12_DEFAULT_RESOURCE_STATE);
	}

	bool DeferredGeometryPass::InternalCreate()
	{
		static bool Created = false;
		if (!Created)
		{
			uint32_t QuadIndices[] =
			{
				0, 1, 3,
				0, 3, 2
			};

			ScreenSpaceVertex FullScreenQuadVerts[] =
			{
				{ { -1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } }, // Top Left
				{ {  1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } }, // Top Right
				{ { -1.0f,-1.0f, 0.0f }, { 0.0f, 1.0f } }, // Bottom Left
				{ {  1.0f,-1.0f, 0.0f }, { 1.0f, 1.0f } }, // Bottom Right
			};
			g_ScreenQuad.Init(FullScreenQuadVerts, sizeof(FullScreenQuadVerts), QuadIndices, sizeof(QuadIndices));
			Created = true;
		}

		ComPtr<ID3DBlob> pVertexShader;
		ComPtr<ID3DBlob> pPixelShader;

		const std::wstring_view ExeDirectory = FileSystem::GetExecutbleDirectoryW();
		std::wstring VertexShaderFolder(ExeDirectory);
		VertexShaderFolder += L"../Renderer/Geometry_Pass.vertex.cso";
		std::wstring PixelShaderFolder(ExeDirectory);
		PixelShaderFolder += L"../Renderer/Geometry_Pass.pixel.cso";

		HRESULT hr = D3DReadFileToBlob(VertexShaderFolder.c_str(), &pVertexShader);
		ThrowIfFailed(hr, "Failed to read Vertex Shader for D3D 12 context.");
		hr = D3DReadFileToBlob(PixelShaderFolder.c_str(), &pPixelShader);
		ThrowIfFailed(hr, "Failed to read Pixel Shader for D3D 12 context.");

		D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
		vertexShaderBytecode.BytecodeLength = pVertexShader->GetBufferSize();
		vertexShaderBytecode.pShaderBytecode = pVertexShader->GetBufferPointer();

		D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
		pixelShaderBytecode.BytecodeLength = pPixelShader->GetBufferSize();
		pixelShaderBytecode.pShaderBytecode = pPixelShader->GetBufferPointer();

		D3D12_INPUT_ELEMENT_DESC inputLayout[5] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0  },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0  },
			{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0  },
			{ "TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0  },
			{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0  },
		};

		D3D12_INPUT_LAYOUT_DESC InputLayoutDesc = {};
		InputLayoutDesc.NumElements = _countof(inputLayout);
		InputLayoutDesc.pInputElementDescs = inputLayout;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc = {};
		PsoDesc.VS = vertexShaderBytecode;
		PsoDesc.PS = pixelShaderBytecode;
		PsoDesc.InputLayout = InputLayoutDesc;
		PsoDesc.pRootSignature = m_pRootSignatureRef.Get();
		PsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		PsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		PsoDesc.SampleMask = UINT_MAX;
		PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		PsoDesc.NumRenderTargets = m_NumRenderTargets;
		for (uint32_t i = 0; i < m_NumRenderTargets; ++i)
			PsoDesc.RTVFormats[i] = m_GBufferRTVFormats[i];
		PsoDesc.DSVFormat = m_DSVFormat;
		PsoDesc.SampleDesc = { 1, 0 };

		hr = m_pRenderContextRef->GetDeviceContext().CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&m_pPipelineState));
		ThrowIfFailed(hr, "Failed to create graphics pipeline state for geometry pass.");
		m_pPipelineState->SetName(L"PSO Geometry Pass");

		CreateResources();

		return true;
	}

	void DeferredGeometryPass::CreateResources()
	{
		ID3D12Device* pDevice = &m_pRenderContextRef->GetDeviceContext();
		const UINT WindowWidth = static_cast<UINT>(m_pRenderContextRef->GetWindowRef().GetWidth());
		const UINT WindowHeight = static_cast<UINT>(m_pRenderContextRef->GetWindowRef().GetHeight());
		const CD3DX12_HEAP_PROPERTIES DefaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
		
		
		// Create the render targets
		HRESULT hr = m_RTVHeap.Create(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_NumRenderTargets);
		ThrowIfFailed(hr, "Failed to create render target view descriptor heap for D3D 12 context.");

		D3D12_RESOURCE_DESC ResourceDesc = {};
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc.Alignment = 0;
		ResourceDesc.SampleDesc = { 1, 0 };
		ResourceDesc.MipLevels = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.Width = WindowWidth;
		ResourceDesc.Height = WindowHeight;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_CLEAR_VALUE ClearVal;
		for (uint8_t i = 0; i < 4; i++)
			ClearVal.Color[i] = m_ScreenClearColor[i];

		// Create memory for the buffers on the GPU.
		for (uint8_t i = 0; i < m_NumRenderTargets; i++)
		{
			ResourceDesc.Format = m_GBufferRTVFormats[i];
			ClearVal.Format = m_GBufferRTVFormats[i];
			hr = pDevice->CreateCommittedResource(&DefaultHeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &ClearVal, IID_PPV_ARGS(&m_pRenderTargetTextures[i]));
			ThrowIfFailed(hr, "Failed to create committed resource for RTV at index: " + std::to_string(i));
		}
		m_pRenderTargetTextures[0]->SetName(L"Render Target Texture Albedo");
		m_pRenderTargetTextures[1]->SetName(L"Render Target Texture Normal");
		m_pRenderTargetTextures[2]->SetName(L"Render Target Texture (R)Roughness/(G)Metallic/(B)AO");
		m_pRenderTargetTextures[3]->SetName(L"Render Target Texture Position");

		D3D12_RENDER_TARGET_VIEW_DESC RTVDesc = {};
		RTVDesc.Texture2D.MipSlice = 0;
		RTVDesc.Texture2D.PlaneSlice = 0;
		RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		// Create the Render Target Views.
		for (uint8_t i = 0; i < m_NumRenderTargets; i++) 
		{
			RTVDesc.Format = m_GBufferRTVFormats[i];
			pDevice->CreateRenderTargetView(m_pRenderTargetTextures[i].Get(), &RTVDesc, m_RTVHeap.hCPU(i));
		}

		// Create Shader Resource Views.
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Texture2D.MipLevels = ResourceDesc.MipLevels;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.Format = ResourceDesc.Format;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		for (uint8_t i = 0; i < m_NumRenderTargets; i++)
		{
			SRVDesc.Format = m_GBufferRTVFormats[i];
			pDevice->CreateShaderResourceView(m_pRenderTargetTextures[i].Get(), &SRVDesc, m_pCBVSRVHeapRef->hCPU(i));
		}
		m_pRenderTargetTextures[0]->SetName(L"Render Target SRV Albedo");
		m_pRenderTargetTextures[1]->SetName(L"Render Target SRV Normal");
		m_pRenderTargetTextures[2]->SetName(L"Render Target SRV (R)Roughness/(G)Metallic/(B)AO");
		m_pRenderTargetTextures[3]->SetName(L"Render Target SRV Position");

		// Create the scene depth stencil buffer
		m_DSVHeap.Create(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);

		D3D12_RESOURCE_DESC SceneDepthResourceDesc = {};
		SceneDepthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		SceneDepthResourceDesc.Alignment = 0;
		SceneDepthResourceDesc.SampleDesc = { 1, 0 };
		SceneDepthResourceDesc.MipLevels = 1;
		SceneDepthResourceDesc.Format = m_DSVFormat;
		SceneDepthResourceDesc.DepthOrArraySize = 1;
		SceneDepthResourceDesc.Width = static_cast<UINT64>(WindowWidth);
		SceneDepthResourceDesc.Height = static_cast<UINT64>(WindowHeight);
		SceneDepthResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		SceneDepthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE SceneDepthOptomizedClearValue = {};
		SceneDepthOptomizedClearValue.Format = m_DSVFormat;
		SceneDepthOptomizedClearValue.DepthStencil = { m_DepthClearValue, 0 };

		hr = pDevice->CreateCommittedResource(
			&DefaultHeapProps,
			D3D12_HEAP_FLAG_NONE,
			&SceneDepthResourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&SceneDepthOptomizedClearValue,
			IID_PPV_ARGS(&m_pSceneDepthStencilTexture));
		ThrowIfFailed(hr, "Failed to create comitted resource for depth stencil view");
		m_pSceneDepthStencilTexture->SetName(L"Scene Depth Stencil Buffer");

		D3D12_DEPTH_STENCIL_VIEW_DESC SceneDSVDesc = {};
		SceneDSVDesc.Texture2D.MipSlice = 0;
		SceneDSVDesc.Format = SceneDepthResourceDesc.Format;
		SceneDSVDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		SceneDSVDesc.Flags = D3D12_DSV_FLAG_NONE;
		pDevice->CreateDepthStencilView(m_pSceneDepthStencilTexture.Get(), &SceneDSVDesc, m_DSVHeap.hCPU(0));

		D3D12_SHADER_RESOURCE_VIEW_DESC SceneDSVSRV = {};
		SceneDSVSRV.Texture2D.MipLevels = SceneDepthResourceDesc.MipLevels;
		SceneDSVSRV.Texture2D.MostDetailedMip = 0;
		SceneDSVSRV.Format = m_DSVSRVFormat;
		SceneDSVSRV.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SceneDSVSRV.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		pDevice->CreateShaderResourceView(m_pSceneDepthStencilTexture.Get(), &SceneDSVSRV, m_pCBVSRVHeapRef->hCPU(4));
	}
	

	/*==============================*/
	/*			Light Pass			*/
	/*==============================*/

	bool DeferredLightPass::Set()
	{
		PIXBeginEvent(m_pCommandListRef.Get(), 0, L"Rendering Light Pass");
		{
			m_pRenderContextRef->SetActiveCommandList(m_pCommandListRef);

			ID3D12DescriptorHeap* ppHeaps[] = { m_pCBVSRVHeapRef->pDH.Get() };
			m_pCommandListRef->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			// Clear the render targets.
			for (uint8_t i = 0; i < m_NumRenderTargets; ++i)
				m_pCommandListRef->ClearRenderTargetView(m_RTVHeap.hCPU(i), m_ScreenClearColor, 0, nullptr);

			// Set the render targets and depth stencil for this pass.
			//m_pCommandListRef->OMSetRenderTargets(m_NumRenderTargets, &m_RTVHeap.hCPUHeapStart, FALSE, false);
			D3D12_CPU_DESCRIPTOR_HANDLE pRTVHandles[] = { m_RTVHeap.hCPU(0), m_RTVHeap.hCPU(1) };
			m_pCommandListRef->OMSetRenderTargets(_countof(pRTVHandles), pRTVHandles, FALSE, nullptr);

			// Transition the G-Buffer to shader resources we can sample from.
			for (uint8_t i = 0; i < m_GBufferRefs.size(); ++i)
				m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_GBufferRefs[i].Get(), IE_D3D12_DEFAULT_RESOURCE_STATE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

			// Make sure we can read from the depth buffer.
			m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_pSceneDepthTextureRef.Get(), IE_D3D12_DEFAULT_RESOURCE_STATE, D3D12_RESOURCE_STATE_DEPTH_READ);

			// Set the root signature for this pass.
			m_pCommandListRef->SetGraphicsRootSignature(m_pRootSignatureRef.Get());

			// Set the main pipeline for this pass.
			m_pCommandListRef->SetPipelineState(m_pPipelineState.Get());

			// Render.
			g_ScreenQuad.OnRender(m_pCommandListRef);
		}
		PIXEndEvent(m_pCommandListRef.Get());

		return true;
	}
	
	void DeferredLightPass::UnSet()
	{
		// Set the light pass and bloom threshold results in a generic state.
		for (uint8_t i = 0; i < m_NumRenderTargets; ++i)
			m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_pRenderTargetTextures[i].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, IE_D3D12_DEFAULT_RESOURCE_STATE);
		
		// Set the depth buffer back to a generic state.
		m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_pSceneDepthTextureRef.Get(), D3D12_RESOURCE_STATE_DEPTH_READ, IE_D3D12_DEFAULT_RESOURCE_STATE);
	}
	
	bool DeferredLightPass::InternalCreate()
	{
		ComPtr<ID3DBlob> pVertexShader;
		ComPtr<ID3DBlob> pPixelShader;

		const std::wstring_view ExeDirectory = FileSystem::GetExecutbleDirectoryW();
		std::wstring VertexShaderFolder(ExeDirectory);
		VertexShaderFolder += L"../Renderer/Light_Pass.vertex.cso";
		std::wstring PixelShaderFolder(ExeDirectory);
		PixelShaderFolder += L"../Renderer/Light_Pass.pixel.cso";

		HRESULT hr = D3DReadFileToBlob(VertexShaderFolder.c_str(), &pVertexShader);
		ThrowIfFailed(hr, "Failed to compile Vertex Shader for D3D 12 context.");
		hr = D3DReadFileToBlob(PixelShaderFolder.c_str(), &pPixelShader);
		ThrowIfFailed(hr, "Failed to compile Pixel Shader for D3D 12 context.");


		D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
		vertexShaderBytecode.BytecodeLength = pVertexShader->GetBufferSize();
		vertexShaderBytecode.pShaderBytecode = pVertexShader->GetBufferPointer();

		D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
		pixelShaderBytecode.BytecodeLength = pPixelShader->GetBufferSize();
		pixelShaderBytecode.pShaderBytecode = pPixelShader->GetBufferPointer();

		D3D12_INPUT_ELEMENT_DESC inputLayout[2] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		D3D12_INPUT_LAYOUT_DESC InputLayoutDesc = {};
		InputLayoutDesc.NumElements = _countof(inputLayout);
		InputLayoutDesc.pInputElementDescs = inputLayout;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC descPipelineState = {};
		descPipelineState.VS = vertexShaderBytecode;
		descPipelineState.PS = pixelShaderBytecode;
		descPipelineState.InputLayout = InputLayoutDesc;
		descPipelineState.pRootSignature = m_pRootSignatureRef.Get();
		descPipelineState.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		descPipelineState.DepthStencilState.DepthEnable = false;
		descPipelineState.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		descPipelineState.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		descPipelineState.RasterizerState.DepthClipEnable = false;
		descPipelineState.SampleMask = UINT_MAX;
		descPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		descPipelineState.NumRenderTargets = 2;
		descPipelineState.RTVFormats[0] = m_RTVFormats[0]; // Light Pass
		descPipelineState.RTVFormats[1] = m_RTVFormats[1]; // Bloom Buffer
		descPipelineState.SampleDesc.Count = 1;

		hr = m_pRenderContextRef->GetDeviceContext().CreateGraphicsPipelineState(&descPipelineState, IID_PPV_ARGS(&m_pPipelineState));
		ThrowIfFailed(hr, "Failed to create graphics pipeline state for lighting pass.");
		m_pPipelineState->SetName(L"PSO Light Pass");

		CreateResources();

		return true;
	}
	
	void DeferredLightPass::CreateResources()
	{
		ID3D12Device* pDevice = &m_pRenderContextRef->GetDeviceContext();
		const UINT WindowWidth = static_cast<UINT>(m_pRenderContextRef->GetWindowRef().GetWidth());
		const UINT WindowHeight = static_cast<UINT>(m_pRenderContextRef->GetWindowRef().GetHeight());
		const CD3DX12_HEAP_PROPERTIES DefaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);


		// Create the render targets
		HRESULT hr = m_RTVHeap.Create(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_NumRenderTargets);
		ThrowIfFailed(hr, "Failed to create render target view descriptor heap for D3D 12 context.");

		D3D12_RESOURCE_DESC ResourceDesc = {};
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc.Alignment = 0;
		ResourceDesc.SampleDesc = { 1, 0 };
		ResourceDesc.MipLevels = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.Width = WindowWidth;
		ResourceDesc.Height = WindowHeight;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_CLEAR_VALUE ClearVal;
		for (uint8_t i = 0; i < 4; i++)
			ClearVal.Color[i] = m_ScreenClearColor[i];

		for (uint8_t i = 0; i < m_NumRenderTargets; i++) 
		{
			ResourceDesc.Format = m_RTVFormats[i];
			ClearVal.Format = m_RTVFormats[i];
			hr = pDevice->CreateCommittedResource(&DefaultHeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &ClearVal, IID_PPV_ARGS(&m_pRenderTargetTextures[i]));
			ThrowIfFailed(hr, "Failed to create committed resource for RTV at index: " + std::to_string(i));
		}
		m_pRenderTargetTextures[0]->SetName(L"Render Target Texture Light Pass Result");
		m_pRenderTargetTextures[1]->SetName(L"Render Target Texture Bloom");

		D3D12_RENDER_TARGET_VIEW_DESC RTVDesc = {};
		RTVDesc.Texture2D.MipSlice = 0;
		RTVDesc.Texture2D.PlaneSlice = 0;
		RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		for (uint8_t i = 0; i < m_NumRenderTargets; i++) 
		{
			RTVDesc.Format = m_RTVFormats[i];
			pDevice->CreateRenderTargetView(m_pRenderTargetTextures[i].Get(), &RTVDesc, m_RTVHeap.hCPU(i));
		}

		// Create SRVs for Render Targets
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Texture2D.MipLevels = ResourceDesc.MipLevels;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.Format = ResourceDesc.Format;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		// Light Pass Result
		SRVDesc.Format = m_RTVFormats[0];
		pDevice->CreateShaderResourceView(m_pRenderTargetTextures[0].Get(), &SRVDesc, m_pCBVSRVHeapRef->hCPU(5)); // Slot 4 gets taken by the scene depth buffer.
		m_pRenderTargetTextures[0]->SetName(L"Render Target SRV Light Pass Result");

		// Bloom Threshold
		SRVDesc.Format = m_RTVFormats[1];
		pDevice->CreateShaderResourceView(m_pRenderTargetTextures[1].Get(), &SRVDesc, m_pCBVSRVHeapRef->hCPU(8));
		m_pRenderTargetTextures[1]->SetName(L"Render Target SRV Bloom Buffer");
	}
	
	

	/*=======================================*/
	/*		Post-Process Composite Pass		 */
	/*=======================================*/
	
	bool PostProcessCompositePass::Set()
	{
		PIXBeginEvent(m_pCommandListRef.Get(), 0, L"Rendering Post-Process Pass");
		{
			m_pRenderContextRef->SetActiveCommandList(m_pCommandListRef);

			ID3D12DescriptorHeap* ppHeaps[] = { m_pCBVSRVHeapRef->pDH.Get() };
			m_pCommandListRef->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			// Set the render target and rasterizer settings.
			m_pCommandListRef->OMSetRenderTargets(1, &m_pRenderContextRef->GetSwapChainRTV(), TRUE, nullptr);
			m_pCommandListRef->RSSetScissorRects(1, &m_pRenderContextRef->GetClientScissorRect());
			m_pCommandListRef->RSSetViewports(1, &m_pRenderContextRef->GetClientViewPort());

			m_pCommandListRef->SetGraphicsRootSignature(m_pRootSignatureRef.Get());

			m_pCommandListRef->SetPipelineState(m_pPipelineState.Get());
			m_pCommandListRef->SetGraphicsRootDescriptorTable(16, m_pCBVSRVHeapRef->hGPU(5)); // Light Pass result
			m_pCommandListRef->SetGraphicsRootDescriptorTable(18, m_pCBVSRVHeapRef->hGPU(9)); // Bloom Pass result

			g_ScreenQuad.OnRender(m_pCommandListRef);
		}
		PIXEndEvent(m_pCommandListRef.Get());
		return true;
	}

	void PostProcessCompositePass::UnSet()
	{
	}

	bool PostProcessCompositePass::InternalCreate()
	{
		ComPtr<ID3DBlob> pVertexShader;
		ComPtr<ID3DBlob> pPixelShader;

		const std::wstring_view ExeDirectory = FileSystem::GetExecutbleDirectoryW();
		std::wstring VertexShaderFolder(ExeDirectory);
		VertexShaderFolder += L"../Renderer/Screen_Aligned_Quad.vertex.cso";
		std::wstring PixelShaderFolder(ExeDirectory);
		PixelShaderFolder += L"../Renderer/PostProcess_Composite.pixel.cso";

		HRESULT hr = D3DReadFileToBlob(VertexShaderFolder.c_str(), &pVertexShader);
		ThrowIfFailed(hr, "Failed to compile Vertex Shader for D3D 12 context.");
		hr = D3DReadFileToBlob(PixelShaderFolder.c_str(), &pPixelShader);
		ThrowIfFailed(hr, "Failed to compile Pixel Shader for D3D 12 context.");


		D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
		vertexShaderBytecode.BytecodeLength = pVertexShader->GetBufferSize();
		vertexShaderBytecode.pShaderBytecode = pVertexShader->GetBufferPointer();

		D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
		pixelShaderBytecode.BytecodeLength = pPixelShader->GetBufferSize();
		pixelShaderBytecode.pShaderBytecode = pPixelShader->GetBufferPointer();

		D3D12_INPUT_ELEMENT_DESC inputLayout[2] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		D3D12_INPUT_LAYOUT_DESC InputLayoutDesc = {};
		InputLayoutDesc.NumElements = _countof(inputLayout);
		InputLayoutDesc.pInputElementDescs = inputLayout;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC descPipelineState = {};
		descPipelineState.VS = vertexShaderBytecode;
		descPipelineState.PS = pixelShaderBytecode;
		descPipelineState.InputLayout = InputLayoutDesc;
		descPipelineState.pRootSignature = m_pRootSignatureRef.Get();
		descPipelineState.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		descPipelineState.DepthStencilState.DepthEnable = false;
		descPipelineState.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		descPipelineState.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		descPipelineState.RasterizerState.DepthClipEnable = false;
		descPipelineState.SampleMask = UINT_MAX;
		descPipelineState.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		descPipelineState.NumRenderTargets = 1; // Rendering directly to the back buffer of the swapchain
		descPipelineState.RTVFormats[0] = m_pRenderContextRef->GetSwapChainBackBufferFormat();
		descPipelineState.SampleDesc.Count = 1;

		hr = m_pRenderContextRef->GetDeviceContext().CreateGraphicsPipelineState(&descPipelineState, IID_PPV_ARGS(&m_pPipelineState));
		ThrowIfFailed(hr, "Failed to create graphics pipeline state for Post-Fx pass in D3D 12 context.");
		m_pPipelineState->SetName(L"PSO Post-Process Combine Pass");

		return true;
	}

	void PostProcessCompositePass::CreateResources()
	{
	}
}
