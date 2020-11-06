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
			m_pCommandListRef->SetComputeRootDescriptorTable(1, m_hSRVSource);
			m_pCommandListRef->SetComputeRootDescriptorTable(2, m_hUAVDestination);

			m_pCommandListRef->Dispatch(
				static_cast<UINT>(m_DownSampleTargetDimensions.first / ThreadsPerPixel),
				static_cast<UINT>(m_DownSampleTargetDimensions.second / ThreadsPerPixel),
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


	/*==================================*/
	/*		Gaussian Blur Helper		*/
	/*==================================*/

	void GaussianBlurHelper::Execute(FrameResources* pFrameResources)
	{
		m_pCommandListRef->SetPipelineState(m_pPipelineState.Get());
		m_pCommandListRef->SetComputeRootSignature(m_pRootSignature.Get());

		constexpr UINT ThreadsPerPixel = 16U;
		// Horizontal Pass
		// ---------------
		PIXBeginEvent(m_pCommandListRef.Get(), 0, L"Blurring HORIZONTALLY");
		{
			m_pCommandListRef->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pSourceUAVRef.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE));
			// Copy the UAV to a shader readable SRV
			m_pCommandListRef->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pSourceSRVRef.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST));
			m_pCommandListRef->CopyResource(m_pSourceSRVRef.Get(), m_pSourceUAVRef.Get());
			m_pCommandListRef->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pSourceSRVRef.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));
			m_pCommandListRef->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pSourceUAVRef.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

			m_pCommandListRef->SetComputeRootDescriptorTable(1, m_hSourceSRV); // Set the SRV of the down sampled bloom texture
			m_pCommandListRef->SetComputeRootDescriptorTable(2, m_hIntermediateUAV); // Set the UAV for the intermediate buffer

			pFrameResources->m_CBBlurParams.Data.Direction = BlurPassHorizontal;
			pFrameResources->m_CBBlurParams.SubmitToGPU();
			pFrameResources->m_CBBlurParams.SetAsComputeRootConstantBufferView(m_pCommandListRef.Get(), 0);

			m_pCommandListRef->Dispatch(m_WindowDimensions.first / ThreadsPerPixel, m_WindowDimensions.second / ThreadsPerPixel, 1);
		}
		PIXEndEvent(m_pCommandListRef.Get());

		// Vertical Pass
		// -------------
		PIXBeginEvent(m_pCommandListRef.Get(), 0, L"Blurring VERTICALLY");
		{
			m_pCommandListRef->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pIntermediateUAVRef.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE));
			// Copy the UAV to a shader readable SRV
			m_pCommandListRef->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pIntermediateSRVRef.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST));
			m_pCommandListRef->CopyResource(m_pIntermediateSRVRef.Get(), m_pIntermediateUAVRef.Get());
			m_pCommandListRef->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pIntermediateSRVRef.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));
			m_pCommandListRef->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pIntermediateUAVRef.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

			pFrameResources->m_CBBlurParams.Data.Direction = BlurPassVertical;
			pFrameResources->m_CBBlurParams.SubmitToGPU();
			pFrameResources->m_CBBlurParams.SetAsComputeRootConstantBufferView(m_pCommandListRef.Get(), 0);

			m_pCommandListRef->SetComputeRootDescriptorTable(1, m_hIntermediateSRV);	// Set the SRV of the intermediate texture, Which now contains a horizontally blurred texture
			m_pCommandListRef->SetComputeRootDescriptorTable(2, m_hSourceUAV);			// Set the origional UAV for final blur pass.

			m_pCommandListRef->Dispatch(m_WindowDimensions.first / ThreadsPerPixel, m_WindowDimensions.second / ThreadsPerPixel, 1);
		}
		PIXEndEvent(m_pCommandListRef.Get());
	}

	void GaussianBlurHelper::LoadPipeline(ComPtr<ID3D12Device> pDevice)
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

		const std::wstring_view ExeDirectory = FileSystem::GetExecutbleDirectoryW();
		std::wstring ComputeShaderFolder(ExeDirectory);
		ComputeShaderFolder += L"../Renderer/Gaussian_Blur.compute.cso";

		ComPtr<ID3DBlob> pComputeShader;
		hr = D3DReadFileToBlob(ComputeShaderFolder.c_str(), &pComputeShader);
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

}