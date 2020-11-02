#include <Renderer_pch.h>

#include "Render_Passes.h"

#include "Renderer/Platform/Windows/DirectX_12/Direct3D12_Context.h"
#include "Platform/Windows/Windows_Window.h"
#include "Insight/Systems/Managers/Geometry_Manager.h"
#include "Insight/Rendering/ASky_Light.h"

namespace Insight {


	D3D12ScreenQuad g_ScreenQuad;


	/*===========================*/
	/*		Geometry Pass		 */
	/*===========================*/

	bool DeferredGeometryPass::Set(FrameResources* pFrameResources)
	{
		PIXBeginEvent(m_pCommandListRef.Get(), 0, L"Rendering Geometry Pass");
		{
			m_pRenderContextRef->SetActiveCommandList(m_pCommandListRef);

			ID3D12DescriptorHeap* ppHeaps[] = { m_pCBVSRVHeapRef->pDH.Get() };
			m_pCommandListRef->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			// Set the buffers to a Render Target state.
			for (uint8_t i = 0; i < m_NumRenderTargets; ++i)
				m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_pRenderTargetTextures[i].Get(), IE_D3D12_DEFAULT_RESOURCE_STATE, D3D12_RESOURCE_STATE_RENDER_TARGET);

			// Make sure we can write to the depth buffer in this pass.
			m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_pSceneDepthStencilTexture.Get(), IE_D3D12_DEFAULT_RESOURCE_STATE, D3D12_RESOURCE_STATE_DEPTH_WRITE);

			// Clear the render targets.
			for (uint8_t i = 0; i < m_NumRenderTargets; ++i)
				m_pCommandListRef->ClearRenderTargetView(m_RTVHeap.hCPU(i), m_ScreenClearColor, 0, nullptr);

			// Clear the Depth Stencil View.
			m_pCommandListRef->ClearDepthStencilView(m_DSVHeap.hCPU(0), D3D12_CLEAR_FLAG_DEPTH, m_DepthClearValue, 0xFF, 0, nullptr);

			// Set the render targets and depth stencil for this pass.
			m_pCommandListRef->OMSetRenderTargets(m_NumRenderTargets, &m_RTVHeap.hCPUHeapStart, TRUE, &m_DSVHeap.hCPU(0));
			m_pCommandListRef->RSSetScissorRects(1, &m_pRenderContextRef->GetClientScissorRect());
			m_pCommandListRef->RSSetViewports(1, &m_pRenderContextRef->GetClientViewPort());

			// Set the root signature and pipeline state for this pass.
			m_pCommandListRef->SetGraphicsRootSignature(m_pRootSignatureRef.Get());
			m_pCommandListRef->SetPipelineState(m_pPipelineState.Get());

			// Bind resources to the pipeline.
			pFrameResources->m_CBLights.SetAsGraphicsRootConstantBufferView(m_pCommandListRef.Get(), 2);
			pFrameResources->m_CBPerFrame.SetAsGraphicsRootConstantBufferView(m_pCommandListRef.Get(), 1);

			// Render.
			m_pCommandListRef->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			GeometryManager::Render(RenderPassType::RenderPassType_Scene);
		}
		PIXEndEvent(m_pCommandListRef.Get());

		return false;
	}

	void DeferredGeometryPass::UnSet(FrameResources* pFrameResources)
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
			static UINT QuadIndices[] =
			{
				0, 1, 3,
				0, 3, 2
			};

			static ScreenSpaceVertex FullScreenQuadVerts[] =
			{
				{ { -1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } }, // Top Left
				{ {  1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } }, // Top Right
				{ { -1.0f,-1.0f, 0.0f }, { 0.0f, 1.0f } }, // Bottom Left
				{ {  1.0f,-1.0f, 0.0f }, { 1.0f, 1.0f } }, // Bottom Right
			};
			g_ScreenQuad.Init(FullScreenQuadVerts, sizeof(FullScreenQuadVerts), QuadIndices, sizeof(QuadIndices));
			Created = true;
		}

		LoadPipeline();

		CreateResources();

		return true;
	}

	void DeferredGeometryPass::LoadPipeline()
	{
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

		m_pPipelineState.Reset();
		hr = m_pRenderContextRef->GetDeviceContext().CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&m_pPipelineState));
		ThrowIfFailed(hr, "Failed to create graphics pipeline state for geometry pass.");
		m_pPipelineState->SetName(L"PSO Geometry Pass");
	}

	void DeferredGeometryPass::CreateResources()
	{
		ID3D12Device* pDevice = &m_pRenderContextRef->GetDeviceContext();
		const UINT WindowWidth = m_pRenderContextRef->GetWindowRef().GetWidth();
		const UINT WindowHeight = m_pRenderContextRef->GetWindowRef().GetHeight();
		const CD3DX12_HEAP_PROPERTIES DefaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);

		// Destroy resources if they already exist. We might be resizing the window if we come back here.
		for (auto Resource : m_pRenderTargetTextures)
		{
			if (Resource.Get())
				Resource.Reset();
		}

		if (m_pSceneDepthStencilTexture.Get())
			m_pSceneDepthStencilTexture.Reset();

		// Create the render targets
		if (m_RTVHeap.pDH.Get()) m_RTVHeap.Destroy();
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
		for (uint8_t i = 0; i < m_NumRenderTargets; i++)
			ClearVal.Color[i] = m_ScreenClearColor[i];

		// Create memory for the buffers on the GPU.
		for (uint8_t i = 0; i < m_NumRenderTargets; i++)
		{
			ResourceDesc.Format = m_GBufferRTVFormats[i];
			ClearVal.Format = m_GBufferRTVFormats[i];
			hr = pDevice->CreateCommittedResource(
				&DefaultHeapProps,
				D3D12_HEAP_FLAG_NONE,
				&ResourceDesc,
				IE_D3D12_DEFAULT_RESOURCE_STATE,
				&ClearVal,
				IID_PPV_ARGS(&m_pRenderTargetTextures[i])
			);
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
		if (m_DSVHeap.pDH.Get()) m_DSVHeap.Destroy();
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
			IE_D3D12_DEFAULT_RESOURCE_STATE,
			&SceneDepthOptomizedClearValue,
			IID_PPV_ARGS(&m_pSceneDepthStencilTexture)
		);
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

	bool DeferredLightPass::Set(FrameResources* pFrameResources)
	{
		PIXBeginEvent(m_pCommandListRef.Get(), 0, L"Rendering Light Pass");
		{
			//m_pRenderContextRef->SetActiveCommandList(m_pCommandListRef);

			ID3D12DescriptorHeap* ppHeaps[] = { m_pCBVSRVHeapRef->pDH.Get() };
			m_pCommandListRef->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			// Set the light pass and bloom threshold results in a Render Target state.
			for (uint8_t i = 0; i < m_NumRenderTargets; ++i)
				m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_pRenderTargetTextures[i].Get(), IE_D3D12_DEFAULT_RESOURCE_STATE, D3D12_RESOURCE_STATE_RENDER_TARGET);

			// Transition the G-Buffer to shader resources we can sample from.
			for (uint8_t i = 0; i < m_GBufferRefs.size(); ++i)
				m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_GBufferRefs[i].Get(), IE_D3D12_DEFAULT_RESOURCE_STATE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

			// Make sure we can read from the depth buffer.
			m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_pSceneDepthTextureRef.Get(), IE_D3D12_DEFAULT_RESOURCE_STATE, D3D12_RESOURCE_STATE_DEPTH_READ);

			// Clear the render targets.
			for (uint8_t i = 0; i < m_NumRenderTargets; ++i)
				m_pCommandListRef->ClearRenderTargetView(m_RTVHeap.hCPU(i), m_ScreenClearColor, 0, nullptr);

			// Set the render targets and depth stencil for this pass.
			D3D12_CPU_DESCRIPTOR_HANDLE pRTVHandles[] = { m_RTVHeap.hCPU(0), m_RTVHeap.hCPU(1) };
			m_pCommandListRef->OMSetRenderTargets(_countof(pRTVHandles), pRTVHandles, FALSE, nullptr);
			m_pCommandListRef->RSSetScissorRects(1, &m_pRenderContextRef->GetClientScissorRect());
			m_pCommandListRef->RSSetViewports(1, &m_pRenderContextRef->GetClientViewPort());

			// Set the root signature and pipeline state for this pass.
			m_pCommandListRef->SetGraphicsRootSignature(m_pRootSignatureRef.Get());
			m_pCommandListRef->SetPipelineState(m_pPipelineState.Get());

			// Bind resources to the pipeline.
			m_pCommandListRef->SetGraphicsRootDescriptorTable(17, m_pCBVSRVHeapRef->hGPU(6)); // Ray Trace Result
			m_pCommandListRef->SetGraphicsRootDescriptorTable(11, m_pCBVSRVHeapRef->hGPU(7)); // Shadow Depth
			m_pCommandListRef->SetGraphicsRootDescriptorTable(5,  m_pCBVSRVHeapRef->hGPU(0)); // G-Buffer
			if (m_pSkyLightRef) 
				m_pSkyLightRef->BindCubeMaps(true);

			// Render.
			g_ScreenQuad.OnRender(m_pCommandListRef);
		}
		PIXEndEvent(m_pCommandListRef.Get());

		return true;
	}

	void DeferredLightPass::UnSet(FrameResources* pFrameResources)
	{
		// Set the light pass and bloom threshold results in a generic state.
		for (uint8_t i = 0; i < m_NumRenderTargets; ++i)
			m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_pRenderTargetTextures[i].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, IE_D3D12_DEFAULT_RESOURCE_STATE);

		// Transition the G-Buffer back to a common state for other passes.
		for (uint8_t i = 0; i < m_GBufferRefs.size(); ++i)
			m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_GBufferRefs[i].Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, IE_D3D12_DEFAULT_RESOURCE_STATE);

		// Set the depth buffer back to a generic state.
		m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_pSceneDepthTextureRef.Get(), D3D12_RESOURCE_STATE_DEPTH_READ, IE_D3D12_DEFAULT_RESOURCE_STATE);
	}

	bool DeferredLightPass::InternalCreate()
	{
		LoadPipeline();

		CreateResources();

		return true;
	}

	void DeferredLightPass::LoadPipeline()
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
		descPipelineState.NumRenderTargets = m_NumRenderTargets;
		descPipelineState.RTVFormats[0] = m_RTVFormats[0]; // Light Pass
		descPipelineState.RTVFormats[1] = m_RTVFormats[1]; // Bloom Buffer
		descPipelineState.SampleDesc.Count = 1;

		m_pPipelineState.Reset();
		hr = m_pRenderContextRef->GetDeviceContext().CreateGraphicsPipelineState(&descPipelineState, IID_PPV_ARGS(&m_pPipelineState));
		ThrowIfFailed(hr, "Failed to create graphics pipeline state for lighting pass.");
		m_pPipelineState->SetName(L"PSO Light Pass");
	}

	void DeferredLightPass::CreateResources()
	{
		ID3D12Device* pDevice = &m_pRenderContextRef->GetDeviceContext();
		const UINT WindowWidth = static_cast<UINT>(m_pRenderContextRef->GetWindowRef().GetWidth());
		const UINT WindowHeight = static_cast<UINT>(m_pRenderContextRef->GetWindowRef().GetHeight());
		const CD3DX12_HEAP_PROPERTIES DefaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);

		// Destroy resources if they already exist. We might be resizing the window if we come back here.
		for (auto Resource : m_pRenderTargetTextures)
		{
			if (Resource.Get())
				Resource.Reset();
		}

		if (m_GBufferRefs.size() > 0)
			m_GBufferRefs.clear();

		if (m_pSceneDepthTextureRef.Get())
			m_pSceneDepthTextureRef.ReleaseAndGetAddressOf();

		// Create the render targets
		if (m_RTVHeap.pDH.Get()) m_RTVHeap.Destroy();
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
			hr = pDevice->CreateCommittedResource(
				&DefaultHeapProps,
				D3D12_HEAP_FLAG_NONE,
				&ResourceDesc,
				IE_D3D12_DEFAULT_RESOURCE_STATE,
				&ClearVal,
				IID_PPV_ARGS(&m_pRenderTargetTextures[i])
			);
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


	/*=======================*/
	/*		Sky Pass		 */
	/*=======================*/

	bool SkyPass::Set(FrameResources* pFrameResources)
	{
		PIXBeginEvent(m_pCommandListRef.Get(), 0, L"Rendering Sky Pass");
		{
			m_pRenderContextRef->SetActiveCommandList(m_pCommandListRef);

			if (m_pSkyShereRef)
			{
				m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_pSceneDepthTextureRef.Get(), IE_D3D12_DEFAULT_RESOURCE_STATE, D3D12_RESOURCE_STATE_DEPTH_WRITE);

				m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_pRenderTargetRef.Get(), IE_D3D12_DEFAULT_RESOURCE_STATE, D3D12_RESOURCE_STATE_RENDER_TARGET);
				
				m_pCommandListRef->OMSetRenderTargets(1, &m_pRTVHandle, TRUE, &m_pDSVHandle);
				m_pCommandListRef->SetPipelineState(m_pPipelineState.Get());
				
				m_pSkyShereRef->RenderSky();
			}
		}
		PIXEndEvent(m_pCommandListRef.Get());
		return true;
	}

	void SkyPass::UnSet(FrameResources* pFrameResources)
	{

		m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_pSceneDepthTextureRef.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, IE_D3D12_DEFAULT_RESOURCE_STATE);

		m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_pRenderTargetRef.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, IE_D3D12_DEFAULT_RESOURCE_STATE);
	}

	bool SkyPass::InternalCreate()
	{
		LoadPipeline();
		CreateResources();

		return true;
	}

	void SkyPass::LoadPipeline()
	{
		ComPtr<ID3DBlob> pVertexShader;
		ComPtr<ID3DBlob> pPixelShader;

		const std::wstring_view ExeDirectory = FileSystem::GetExecutbleDirectoryW();
		std::wstring VertexShaderFolder(ExeDirectory);
		VertexShaderFolder += L"../Renderer/Skybox.vertex.cso";
		std::wstring PixelShaderFolder(ExeDirectory);
		PixelShaderFolder += L"../Renderer/Skybox.pixel.cso";

		HRESULT hr = D3DReadFileToBlob(VertexShaderFolder.c_str(), &pVertexShader);
		ThrowIfFailed(hr, "Failed to compile Vertex Shader for D3D 12 context");
		hr = D3DReadFileToBlob(PixelShaderFolder.c_str(), &pPixelShader);
		ThrowIfFailed(hr, "Failed to compile Pixel Shader for D3D 12 context");


		D3D12_SHADER_BYTECODE VertexShaderBytecode = {};
		VertexShaderBytecode.BytecodeLength = pVertexShader->GetBufferSize();
		VertexShaderBytecode.pShaderBytecode = pVertexShader->GetBufferPointer();

		D3D12_SHADER_BYTECODE PixelShaderBytecode = {};
		PixelShaderBytecode.BytecodeLength = pPixelShader->GetBufferSize();
		PixelShaderBytecode.pShaderBytecode = pPixelShader->GetBufferPointer();

		D3D12_INPUT_ELEMENT_DESC InputLayout[2] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		D3D12_INPUT_LAYOUT_DESC InputLayoutDesc = {};
		InputLayoutDesc.NumElements = sizeof(InputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
		InputLayoutDesc.pInputElementDescs = InputLayout;

		auto DepthStencilStateDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		DepthStencilStateDesc.DepthEnable = true;
		DepthStencilStateDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		DepthStencilStateDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		auto RasterizerStateDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		RasterizerStateDesc.DepthClipEnable = true;
		RasterizerStateDesc.CullMode = D3D12_CULL_MODE_FRONT;
		RasterizerStateDesc.FillMode = D3D12_FILL_MODE_SOLID;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
		pipelineDesc.VS = VertexShaderBytecode;
		pipelineDesc.PS = PixelShaderBytecode;
		pipelineDesc.InputLayout = InputLayoutDesc;
		pipelineDesc.pRootSignature = m_pRootSignatureRef.Get();
		pipelineDesc.DepthStencilState = DepthStencilStateDesc;
		pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		pipelineDesc.RasterizerState = RasterizerStateDesc;
		pipelineDesc.SampleMask = UINT_MAX;
		pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineDesc.NumRenderTargets = 1;
		pipelineDesc.RTVFormats[0] = m_pRenderTargetRef->GetDesc().Format;
		pipelineDesc.SampleDesc = { 1, 0 };
		pipelineDesc.DSVFormat = m_pSceneDepthTextureRef->GetDesc().Format;


		hr = m_pRenderContextRef->GetDeviceContext().CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&m_pPipelineState));
		ThrowIfFailed(hr, "Failed to create skybox pipeline state object for .");
		m_pPipelineState->SetName(L"PSO Sky Pass");
	}

	void SkyPass::CreateResources()
	{

	}


	/*=======================================*/
	/*		Post-Process Composite Pass		 */
	/*=======================================*/

	bool PostProcessCompositePass::Set(FrameResources* pFrameResources)
	{
		PIXBeginEvent(m_pCommandListRef.Get(), 0, L"Rendering Post-Process Pass");
		{
			//m_pRenderContextRef->SetActiveCommandList(m_pCommandListRef);

			ID3D12DescriptorHeap* ppHeaps[] = { m_pCBVSRVHeapRef->pDH.Get() };
			m_pCommandListRef->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			// Transition the main render target.
			m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_pRenderContextRef->GetSwapChainRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

			// Make sure we can read from the depth buffer.
			m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_pSceneDepthTextureRef.Get(), IE_D3D12_DEFAULT_RESOURCE_STATE, D3D12_RESOURCE_STATE_DEPTH_READ);

			m_pCommandListRef->ClearRenderTargetView(m_pRenderContextRef->GetSwapChainRTV(), m_ScreenClearColor, 0, nullptr);

			// Set the render target and rasterizer settings.
			m_pCommandListRef->OMSetRenderTargets(1, &m_pRenderContextRef->GetSwapChainRTV(), TRUE, nullptr);
			m_pCommandListRef->RSSetScissorRects(1, &m_pRenderContextRef->GetClientScissorRect());
			m_pCommandListRef->RSSetViewports(1, &m_pRenderContextRef->GetClientViewPort());

			// Set the root signature and pipeline state.
			m_pCommandListRef->SetGraphicsRootSignature(m_pRootSignatureRef.Get());
			m_pCommandListRef->SetPipelineState(m_pPipelineState.Get());

			// Bind resources to the pipeline.
			m_pCommandListRef->SetGraphicsRootDescriptorTable(16, m_pCBVSRVHeapRef->hGPU(5)); // Light Pass result
			m_pCommandListRef->SetGraphicsRootDescriptorTable(18, m_pCBVSRVHeapRef->hGPU(9)); // Bloom Pass result
			pFrameResources->m_CBPerFrame.SetAsGraphicsRootConstantBufferView(m_pCommandListRef.Get(), 1);
			pFrameResources->m_CBPostProcessParams.SetAsGraphicsRootConstantBufferView(m_pCommandListRef.Get(), 3);

			// Render.
			g_ScreenQuad.OnRender(m_pCommandListRef);
		}
		PIXEndEvent(m_pCommandListRef.Get());
		return true;
	}

	void PostProcessCompositePass::UnSet(FrameResources* pFrameResources)
	{
		// Transition the render target for presentation to the sreen on the swap chain.
		IE_ADD_FOR_GAME_DIST(
			m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_pRenderContextRef->GetSwapChainRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		);

		// Set the depth buffer back to a generic state.
		m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_pSceneDepthTextureRef.Get(), D3D12_RESOURCE_STATE_DEPTH_READ, IE_D3D12_DEFAULT_RESOURCE_STATE);
	}

	bool PostProcessCompositePass::InternalCreate()
	{
		LoadPipeline();

		return true;
	}

	void PostProcessCompositePass::LoadPipeline()
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

		D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc = {};
		PipelineStateDesc.VS = vertexShaderBytecode;
		PipelineStateDesc.PS = pixelShaderBytecode;
		PipelineStateDesc.InputLayout = InputLayoutDesc;
		PipelineStateDesc.pRootSignature = m_pRootSignatureRef.Get();
		PipelineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		PipelineStateDesc.DepthStencilState.DepthEnable = false;
		PipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		PipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		PipelineStateDesc.RasterizerState.DepthClipEnable = false;
		PipelineStateDesc.SampleMask = UINT_MAX;
		PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		PipelineStateDesc.NumRenderTargets = m_NumRenderTargets; // Rendering directly to the back buffer of the swapchain
		PipelineStateDesc.RTVFormats[0] = m_pRenderContextRef->GetSwapChainBackBufferFormat();
		PipelineStateDesc.SampleDesc = { 1, 0 };

		m_pPipelineState.Reset();
		hr = m_pRenderContextRef->GetDeviceContext().CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&m_pPipelineState));
		ThrowIfFailed(hr, "Failed to create graphics pipeline state for Post-Fx pass in D3D 12 context.");
		m_pPipelineState->SetName(L"PSO Post-Process Combine Pass");
	}

	void PostProcessCompositePass::CreateResources()
	{
	}


	/*===================================*/
	/*		Ray-Traced Shadows Pass		 */
	/*===================================*/

	bool RayTracedShadowsPass::Set(FrameResources* pFrameResources)
	{
		PIXBeginEvent(m_pCommandListRef.Get(), 0, L"Rendering Ray Trace Pass");
		{
			m_RTHelper.SetCommonPipeline();
			m_RTHelper.TraceScene();

			m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_pRayTraceOutput_SRV.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
			m_pCommandListRef->CopyResource(m_pRayTraceOutput_SRV.Get(), m_RTHelper.GetOutputBuffer());
			m_pRenderContextRef->ResourceBarrier(m_pCommandListRef.Get(), m_pRayTraceOutput_SRV.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}
		PIXEndEvent(m_pCommandListRef.Get());
		return true;
	}

	void RayTracedShadowsPass::UnSet(FrameResources* pFrameResources)
	{
	}

	bool RayTracedShadowsPass::InternalCreate()
	{
		LoadPipeline();
		CreateResources();

		return true;
	}

	void RayTracedShadowsPass::LoadPipeline()
	{
		m_RTHelper.Init(m_pRenderContextRef, reinterpret_cast<ID3D12GraphicsCommandList4*>(m_pCommandListRef.Get()));
	}

	void RayTracedShadowsPass::CreateResources()
	{
		m_pRayTraceOutput_SRV.Reset();

		ID3D12Device* pDevice = &m_pRenderContextRef->GetDeviceContext();
		const UINT WindowWidth = static_cast<UINT>(m_pRenderContextRef->GetWindowRef().GetWidth());
		const UINT WindowHeight = static_cast<UINT>(m_pRenderContextRef->GetWindowRef().GetHeight());
		const CD3DX12_HEAP_PROPERTIES DefaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);

		D3D12_RESOURCE_DESC ResourceDesc = {};
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc.Alignment = 0;
		ResourceDesc.SampleDesc.Count = 1;
		ResourceDesc.SampleDesc.Quality = 0;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.Width = WindowWidth;
		ResourceDesc.Height = WindowHeight;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		ResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		// Ray Trace Pass Result SRV
		HRESULT hr = pDevice->CreateCommittedResource(
			&DefaultHeapProps,
			D3D12_HEAP_FLAG_NONE, 
			&ResourceDesc, 
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 
			NULL, 
			IID_PPV_ARGS(&m_pRayTraceOutput_SRV)
		);
		ThrowIfFailed(hr, "Failed to create ray trace output SRV.");

		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SRVDesc.Texture2D.MipLevels = 1U;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		pDevice->CreateShaderResourceView(m_pRayTraceOutput_SRV.Get(), &SRVDesc, m_pCBVSRVHeapRef->hCPU(6));
	}

}
