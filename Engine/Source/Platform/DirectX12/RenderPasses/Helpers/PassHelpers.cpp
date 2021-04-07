#include <Engine_pch.h>

#include "PassHelpers.h"

#include "Platform/Public/Utility/COMException.h"
#include "Platform/DirectX12/Direct3D12Context.h"
#include "Platform/DirectX12/Wrappers/D3D12Shader.h"

namespace Insight {





	/*======================================*/
	/*		Threshold Downsample Helper		*/
	/*======================================*/


	void ThresholdDownSampleHelper::Execute(FrameResources* pFrameResources)
	{
		constexpr Int32 ThreadsPerPixel = 16U;

		BeginTrackRenderEvent(m_pCommandListRef.Get(), 0, L"Downsampling Texture");
		{
			m_pCommandListRef->SetPipelineState(m_pPipeilneState.Get());
			m_pCommandListRef->SetComputeRootSignature(m_pRootSignature.Get());

			pFrameResources->m_CBDownSampleParams.Data.Threshold = 0.5f;
			pFrameResources->m_CBDownSampleParams.SubmitToGPU();
			pFrameResources->m_CBDownSampleParams.SetAsComputeRootConstantBufferView(m_pCommandListRef.Get(), 0);
			m_pCommandListRef->SetComputeRootDescriptorTable(1, m_hSRVSource);
			m_pCommandListRef->SetComputeRootDescriptorTable(2, m_hUAVDestination);

			m_pCommandListRef->Dispatch(
				(Int32)(m_DownSampleTargetDimensions.x / ThreadsPerPixel),
				(Int32)(m_DownSampleTargetDimensions.y / ThreadsPerPixel),
				1													
			);
		}
		EndTrackRenderEvent(m_pCommandListRef.Get());
	}

	void ThresholdDownSampleHelper::LoadPipeline(Microsoft::WRL::ComPtr<ID3D12Device> pDevice)
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

		Microsoft::WRL::ComPtr<ID3DBlob> RootSignatureBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> ErrorBlob;

		HRESULT hr = D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, RootSignatureBlob.GetAddressOf(), ErrorBlob.GetAddressOf());
		ThrowIfFailed(hr, TEXT("Failed to serialize root signature for D3D 12 context."));

		hr = pDevice->CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature));
		ThrowIfFailed(hr, TEXT("Failed to create root signature for D3D 12 context."));


		// Create the pipeline state.

		D3D12Shader ComputeShader;
		hr = ComputeShader.LoadFromFile(FileSystem::GetShaderPathW(L"ThresholdDownSample.cs.cso").c_str());
		ThrowIfFailed(hr, TEXT("Failed to read Pixel Shader for D3D 12 context."));

		D3D12_COMPUTE_PIPELINE_STATE_DESC PipelineDesc = {};
		PipelineDesc.CS = ComputeShader.GetByteCode();
		PipelineDesc.pRootSignature = m_pRootSignature.Get();

		hr = pDevice->CreateComputePipelineState(&PipelineDesc, IID_PPV_ARGS(&m_pPipeilneState));
		ThrowIfFailed(hr, TEXT("Failed to create texture downsampling pipeline for bloom pass."));
		m_pPipeilneState->SetName(L"PSO Threshold Downsample");
	}


	/*==================================*/
	/*		Gaussian Blur Helper		*/
	/*==================================*/

	void GaussianBlurHelper::Execute(FrameResources* pFrameResources)
	{
		m_pFirstPass_CommandListRef->SetPipelineState(m_pPipelineState.Get());
		m_pFirstPass_CommandListRef->SetComputeRootSignature(m_pRootSignature.Get());

		constexpr uint16_t ThreadsPerPixel = 16U;

		// Horizontal Pass
		// ---------------
		BeginTrackRenderEvent(m_pFirstPass_CommandListRef.Get(), 0, L"Blurring HORIZONTALLY");
		{
			m_pFirstPass_CommandListRef->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pSourceUAVRef.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE));
			// Copy the UAV to a shader readable SRV
			m_pFirstPass_CommandListRef->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pSourceSRVRef.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST));
			m_pFirstPass_CommandListRef->CopyResource(m_pSourceSRVRef.Get(), m_pSourceUAVRef.Get());
			m_pFirstPass_CommandListRef->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pSourceSRVRef.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));
			m_pFirstPass_CommandListRef->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pSourceUAVRef.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

			m_pFirstPass_CommandListRef->SetComputeRootDescriptorTable(1, m_hSourceSRV); // Set the SRV of the down sampled bloom texture
			m_pFirstPass_CommandListRef->SetComputeRootDescriptorTable(2, m_hIntermediateUAV); // Set the UAV for the intermediate buffer

			pFrameResources->m_CBBlurParams.Data.Direction = BlurPassHorizontal;
			pFrameResources->m_CBBlurParams.SubmitToGPU();
			pFrameResources->m_CBBlurParams.SetAsComputeRootConstantBufferView(m_pFirstPass_CommandListRef.Get(), 0);

			m_pFirstPass_CommandListRef->Dispatch((Int32)(m_WindowDimensions.x / ThreadsPerPixel), (Int32)(m_WindowDimensions.y / ThreadsPerPixel), 1);
		}
		EndTrackRenderEvent(m_pFirstPass_CommandListRef.Get());

		// Wait for the first pass to complete so the buffers can be blurred for the second blur pass.
		ThrowIfFailed(m_pFirstPass_CommandListRef->Close(), TEXT("Failed to close command list for D3D 12 context bloom blur pass."));

		ID3D12CommandList* ppComputeLists[] = {
			m_pFirstPass_CommandListRef.Get(),
		};
		m_pRenderContext->GetDeviceResources().GetComputeCommandQueue().ExecuteCommandLists(_countof(ppComputeLists), ppComputeLists);
		m_pRenderContext->ResetBloomFirstPass();

		// Vertical Pass
		// -------------
		BeginTrackRenderEvent(m_pSecondPass_CommandListRef.Get(), 0, L"Blurring VERTICALLY");
		{
			// Prep the pass.
			m_pSecondPass_CommandListRef->SetPipelineState(m_pPipelineState.Get());
			m_pSecondPass_CommandListRef->SetComputeRootSignature(m_pRootSignature.Get());

			ID3D12DescriptorHeap* ppHeaps[] = { m_pRenderContext->GetCBVSRVDescriptorHeap().pDH.Get() };
			m_pSecondPass_CommandListRef->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			m_pSecondPass_CommandListRef->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pIntermediateUAVRef.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE));
			// Copy the UAV to a shader readable SRV
			m_pSecondPass_CommandListRef->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pIntermediateSRVRef.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST));
			m_pSecondPass_CommandListRef->CopyResource(m_pIntermediateSRVRef.Get(), m_pIntermediateUAVRef.Get());
			m_pSecondPass_CommandListRef->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pIntermediateSRVRef.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));
			m_pSecondPass_CommandListRef->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pIntermediateUAVRef.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

			pFrameResources->m_CBBlurParams.Data.Direction = BlurPassVertical;
			pFrameResources->m_CBBlurParams.SubmitToGPU();
			pFrameResources->m_CBBlurParams.SetAsComputeRootConstantBufferView(m_pSecondPass_CommandListRef.Get(), 0);

			m_pSecondPass_CommandListRef->SetComputeRootDescriptorTable(1, m_hIntermediateSRV);	// Set the SRV of the intermediate texture, Which now contains a horizontally blurred texture
			m_pSecondPass_CommandListRef->SetComputeRootDescriptorTable(2, m_hSourceUAV);		// Set the origional UAV for final blur pass.

			m_pSecondPass_CommandListRef->Dispatch((Int32)(m_WindowDimensions.x / ThreadsPerPixel), (Int32)(m_WindowDimensions.y / ThreadsPerPixel), 1);
		}
		EndTrackRenderEvent(m_pSecondPass_CommandListRef.Get());
	}

	void GaussianBlurHelper::LoadPipeline(Microsoft::WRL::ComPtr<ID3D12Device> pDevice)
	{
		// Create the root signature.

		CD3DX12_DESCRIPTOR_RANGE DescriptorRanges[2] = {};
		DescriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // Bloom SRV Source
		DescriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0); // Bloom UAV Destination

		CD3DX12_ROOT_PARAMETER RootParameters[3] = {};
		RootParameters[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL); 
		RootParameters[1].InitAsDescriptorTable(1, &DescriptorRanges[0], D3D12_SHADER_VISIBILITY_ALL); // Source Texture
		RootParameters[2].InitAsDescriptorTable(1, &DescriptorRanges[1], D3D12_SHADER_VISIBILITY_ALL); // Destination Texture

		CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc;
		RootSignatureDesc.Init(_countof(RootParameters), RootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_NONE);

		Microsoft::WRL::ComPtr<ID3DBlob> RootSignatureBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> ErrorBlob;

		HRESULT hr = D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, RootSignatureBlob.GetAddressOf(), ErrorBlob.GetAddressOf());
		ThrowIfFailed(hr, TEXT("Failed to serialize root signature for D3D 12 context."));

		hr = pDevice->CreateRootSignature(0, RootSignatureBlob->GetBufferPointer(), RootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature));
		ThrowIfFailed(hr, TEXT("Failed to create root signature for D3D 12 context."));

		// Create the pipeline state.

		D3D12Shader ComputeShader;
		hr = ComputeShader.LoadFromFile(FileSystem::GetShaderPathW(L"GaussianBlur.cs.cso").c_str());
		ThrowIfFailed(hr, TEXT("Failed to read Pixel Shader for D3D 12 context."));

		D3D12_COMPUTE_PIPELINE_STATE_DESC PipelineDesc = {};
		PipelineDesc.CS = ComputeShader.GetByteCode();
		PipelineDesc.pRootSignature = m_pRootSignature.Get();

		hr = pDevice->CreateComputePipelineState(&PipelineDesc, IID_PPV_ARGS(&m_pPipelineState));
		ThrowIfFailed(hr, TEXT("Failed to create gaussian blur pipeline for bloom pass."));
		m_pPipelineState->SetName(L"Bloom Pass PSO");
	}

}