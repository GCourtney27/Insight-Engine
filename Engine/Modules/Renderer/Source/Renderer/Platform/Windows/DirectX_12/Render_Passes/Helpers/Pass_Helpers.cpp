#include <Renderer_pch.h>

#include "Pass_Helpers.h"

#include "Platform/Windows/Error/COM_Exception.h"
#include "Renderer/Platform/Windows/DirectX_12/Direct3D12_Context.h"


namespace Insight {





	/*======================================*/
	/*		Threshold Downsample Helper		*/
	/*======================================*/


	void ThresholdDownSampleHelper::Execute(FrameResources* pFrameResources)
	{
		constexpr UINT ThreadsPerPixel = 16U;

		PIXBeginEvent(m_pCommandListRef.Get(), 0, L"Downsampling Texture");
		{
			m_pCommandListRef->SetPipelineState(m_pPipeilneState.Get());
			m_pCommandListRef->SetComputeRootSignature(m_pRootSignature.Get());

			pFrameResources->m_CBDownSampleParams.Data.Threshold = 0.5f;
			pFrameResources->m_CBDownSampleParams.SubmitToGPU();
			pFrameResources->m_CBDownSampleParams.SetAsComputeRootConstantBufferView(m_pCommandListRef.Get(), 0);
			m_pCommandListRef->SetComputeRootDescriptorTable(1, m_hDownsampleResource);
			m_pCommandListRef->SetComputeRootDescriptorTable(2, m_pCBVSRVHeapRef->hGPU(9));

			m_pCommandListRef->Dispatch(
				static_cast<UINT>(m_UAVDimensions.first / ThreadsPerPixel),
				static_cast<UINT>(m_UAVDimensions.second / ThreadsPerPixel),
				1													
			);
		}
		PIXEndEvent(m_pCommandListRef.Get());
	}

	void ThresholdDownSampleHelper::LoadPipeline(ComPtr<ID3D12Device> pDevice)
	{
		// Create the root signature.

		CD3DX12_DESCRIPTOR_RANGE DescriptorRanges[2] = {};
		DescriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // Bloom SRV Source
		DescriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0); // Bloom UAV Destination

		CD3DX12_ROOT_PARAMETER RootParameters[3] = {};
		RootParameters[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); // Ping buffer
		RootParameters[1].InitAsDescriptorTable(1, &DescriptorRanges[0], D3D12_SHADER_VISIBILITY_ALL); // Source Texture
		RootParameters[2].InitAsDescriptorTable(1, &DescriptorRanges[1], D3D12_SHADER_VISIBILITY_ALL); // Destination Texture

		CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc;
		RootSignatureDesc.Init(_countof(RootParameters), RootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_NONE);

		ComPtr<ID3DBlob> RootSignatureBlob;
		ComPtr<ID3DBlob> ErrorBlob;

		HRESULT hr = D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, RootSignatureBlob.GetAddressOf(), ErrorBlob.GetAddressOf());
		ThrowIfFailed(hr, "Failed to serialize root signature for D3D 12 context.");

		hr = pDevice->CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature));
		ThrowIfFailed(hr, "Failed to create root signature for D3D 12 context.");


		// Create the pipeline state.

		ComPtr<ID3DBlob> pComputeShader;
		const std::wstring_view ExeDirectory = FileSystem::GetExecutbleDirectoryW();
		std::wstring VertexShaderFolder(ExeDirectory);
		VertexShaderFolder += L"../Renderer/Threshold_Down_Sample.compute.cso";

		hr = D3DReadFileToBlob(VertexShaderFolder.c_str(), &pComputeShader);
		ThrowIfFailed(hr, "Failed to read compute shader for D3D 12 context");

		D3D12_SHADER_BYTECODE ComputeShaderBytecode = {};
		ComputeShaderBytecode.BytecodeLength = pComputeShader->GetBufferSize();
		ComputeShaderBytecode.pShaderBytecode = pComputeShader->GetBufferPointer();

		D3D12_COMPUTE_PIPELINE_STATE_DESC PipelineDesc = {};
		PipelineDesc.CS = ComputeShaderBytecode;
		PipelineDesc.pRootSignature = m_pRootSignature.Get();

		hr = pDevice->CreateComputePipelineState(&PipelineDesc, IID_PPV_ARGS(&m_pPipeilneState));
		ThrowIfFailed(hr, "Failed to create texture downsampling pipeline for bloom pass.");
		m_pPipeilneState->SetName(L"PSO Threshold Downsample");
	}

	void ThresholdDownSampleHelper::CreateResources(ComPtr<ID3D12Device> pDevice)
	{
		CD3DX12_HEAP_PROPERTIES DefaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);

		D3D12_RESOURCE_DESC ResourceDesc = {};
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc.Alignment = 0;
		ResourceDesc.SampleDesc = { 1, 0 };
		ResourceDesc.MipLevels = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.Width = m_UAVDimensions.first / 2u; // Downsample to half-resolution.
		ResourceDesc.Height = m_UAVDimensions.second / 2u;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		ResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		m_pDownsampleResult_UAV.Reset();
		HRESULT hr = pDevice->CreateCommittedResource(
			&DefaultHeapProps, 
			D3D12_HEAP_FLAG_NONE, 
			&ResourceDesc,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS, 
			nullptr,
			IID_PPV_ARGS(&m_pDownsampleResult_UAV)
		);
		ThrowIfFailed(hr, "Failed to create committed resource for bloom down sampled UAV.");
		
		D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
		UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		pDevice->CreateUnorderedAccessView(m_pDownsampleResult_UAV.Get(), nullptr, &UAVDesc, m_pCBVSRVHeapRef->hCPU(9));
		m_pDownsampleResult_UAV->SetName(L"UAV: Bloom Pass Down Sampled Buffer");
	}




	/*==================================*/
	/*		Gaussian Blur Helper		*/
	/*==================================*/

	void GaussianBlurHelper::Execute(FrameResources* pFrameResources, uint8_t BlurDirection)
	{
		// Horizontal Pass
		// ---------------
		//PIXBeginEvent(m_pCommandListRef.Get(), 0, L"Blurring bloom texture HORIZONTALLY");
		//{
		//	ResourceBarrier(m_pCommandListRef.Get(), m_pBloomBlurResult_UAV.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
		//	// Copy the UAV to a shader readable SRV
		//	ResourceBarrier(m_pCommandListRef.Get(), m_pBloomBlurResult_SRV.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
		//	m_pCommandListRef->CopyResource(m_pBloomBlurResult_SRV.Get(), m_pBloomBlurResult_UAV.Get());
		//	ResourceBarrier(m_pCommandListRef.Get(), m_pBloomBlurResult_SRV.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		//	ResourceBarrier(m_pCommandListRef.Get(), m_pBloomBlurResult_UAV.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);


		//	m_pCommandListRef->SetComputeRootDescriptorTable(1, m_cbvsrvHeap.hGPU(10)); // Set the SRV of the down sampled bloom texture
		//	m_pCommandListRef->SetComputeRootDescriptorTable(2, m_cbvsrvHeap.hGPU(11)); // Set the UAV for the intermediate buffer

		//	m_FrameResources.m_CBBlurParams.Data.Direction = 0;
		//	m_FrameResources.m_CBBlurParams.SubmitToGPU();
		//	m_FrameResources.m_CBBlurParams.SetAsComputeRootConstantBufferView(m_pDownSample_CommandList.Get(), 0);

		//	m_pCommandListRef->Dispatch(m_WindowWidth / ThreadsPerPixel, m_WindowHeight / ThreadsPerPixel, 1);
		//}
		//PIXEndEvent(m_pCommandListRef.Get());
	}

	void GaussianBlurHelper::LoadPipeline(ComPtr<ID3D12Device> pDevice)
	{
		ComPtr<ID3DBlob> pComputeShader;

		const std::wstring_view ExeDirectory = FileSystem::GetExecutbleDirectoryW();
		std::wstring ComputeShaderFolder(ExeDirectory);
		ComputeShaderFolder += L"../Renderer/Gaussian_Blur.compute.cso";

		HRESULT hr = D3DReadFileToBlob(ComputeShaderFolder.c_str(), &pComputeShader);
		ThrowIfFailed(hr, "Failed to read compute shader for D3D 12 context");

		D3D12_SHADER_BYTECODE ComputeShaderBytecode = {};
		ComputeShaderBytecode.BytecodeLength = pComputeShader->GetBufferSize();
		ComputeShaderBytecode.pShaderBytecode = pComputeShader->GetBufferPointer();

		D3D12_COMPUTE_PIPELINE_STATE_DESC PipelineDesc = {};
		PipelineDesc.CS = ComputeShaderBytecode;
		PipelineDesc.pRootSignature = m_pRootSignature.Get();

		hr = pDevice->CreateComputePipelineState(&PipelineDesc, IID_PPV_ARGS(&m_pPipelineState));
		ThrowIfFailed(hr, "Failed to create gaussian blur pipeline for bloom pass.");
		m_pPipelineState->SetName(L"Bloom Pass PSO");
	}

	void GaussianBlurHelper::CreateResources(ComPtr<ID3D12Device> pDevice)
	{
	}

}