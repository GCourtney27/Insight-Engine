#pragma once

#include <Runtime/Core.h>
#include "Platform/DirectX12/Wrappers/DescriptorHeapWrapper.h"

namespace Insight {

	class FrameResources;
	class Direct3D12Context;


	/*
		A reusable downsampler that can be used to pack a SRV into a UAV.
		Has no resources and relies on existing ones.
	*/
	class INSIGHT_API ThresholdDownSampleHelper
	{
	public:
		friend class BloomPass;
	public:
		ThresholdDownSampleHelper() = default;
		~ThresholdDownSampleHelper() = default;

		/* 
			Initialize the downsampler.
			@param pDevice - The device context that will be used to initialize the internal pipeline.
			@param DownSampleTargetDimensions - The target dimenstions of the UAV that will be downsampled to.
			@param pCommandList - A reference to the command list the downsample will execute on. Note: This command list must be executed on a Compute command queue.
			@param hSRVSource - A handle to a Shader Resource View that will be downsmpled.
			@parame hUAVDestination - A handle to a Unordered Access View that will be the render target for the downsample process.
		*/
		inline void Create(
			Microsoft::WRL::ComPtr<ID3D12Device> pDevice,
			std::pair<uint32_t, uint32_t> DownSampleTargetDimensions,
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> pCommandList,
			D3D12_GPU_DESCRIPTOR_HANDLE hSRVSource, D3D12_GPU_DESCRIPTOR_HANDLE hUAVDestination
		)
		{
			m_pCommandListRef = pCommandList;
			m_hSRVSource = hSRVSource;
			this->SetSRVSourceHandle(hSRVSource);
			this->SetUAVDestinationHandle(hUAVDestination);
			m_DownSampleTargetDimensions = DownSampleTargetDimensions;

			static bool PipelineLoaded = false;
			if (!PipelineLoaded)
			{
				LoadPipeline(pDevice);
				PipelineLoaded = true;
			}
		}

		inline void SetSRVSourceHandle(D3D12_GPU_DESCRIPTOR_HANDLE hSRVSourceTexture) { m_hSRVSource = hSRVSourceTexture; }
		inline void SetUAVDestinationHandle(D3D12_GPU_DESCRIPTOR_HANDLE hUAVDestination) { m_hUAVDestination = hUAVDestination; }

		/*
			Executes the downsample process.
			@param pFrameResources - A pointer to the resources for this frame.
		*/
		void Execute(FrameResources* pFrameResources);

	protected:
		// Load the pipeline and root signature.
		void LoadPipeline(Microsoft::WRL::ComPtr<ID3D12Device> pDevice);

	private:
		// The root signature for this pass.
		Microsoft::WRL::ComPtr<ID3D12RootSignature>			m_pRootSignature;
		// The pipeline state for the pass.
		Microsoft::WRL::ComPtr<ID3D12PipelineState>			m_pPipeilneState;
		// A reference to a command list that will compute the downsampling on.
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_pCommandListRef;
		
		// A GPU hande to a Shader Resource View to downsize into "ThresholdDownSampleHelper::m_hUAVDestination".
		D3D12_GPU_DESCRIPTOR_HANDLE m_hSRVSource;
		// A GPU hande to a Unordered Access View for "ThresholdDownSampleHelper::m_hSRVSource" to downsize to.
		D3D12_GPU_DESCRIPTOR_HANDLE m_hUAVDestination;
		// The dimensions the dimensions of the UV to downsample too.
		std::pair<uint32_t, uint32_t> m_DownSampleTargetDimensions;
	};





	class INSIGHT_API GaussianBlurHelper
	{
	public:
		GaussianBlurHelper() = default;
		~GaussianBlurHelper() = default;

		inline void Create(
			Direct3D12Context* pRenderContext,
			Microsoft::WRL::ComPtr<ID3D12Device> pDevice, 
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> pFirstPass_CommandList,
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> pSecondPass_CommandList,
			std::pair<uint32_t, uint32_t> WindowDimensions,
			Microsoft::WRL::ComPtr<ID3D12Resource> pSourceUAV, D3D12_GPU_DESCRIPTOR_HANDLE hSourceUAV,				//9
			Microsoft::WRL::ComPtr<ID3D12Resource> pSourceSRV, D3D12_GPU_DESCRIPTOR_HANDLE hSourceSRV,				//10
			Microsoft::WRL::ComPtr<ID3D12Resource> pIntermediateUAV, D3D12_GPU_DESCRIPTOR_HANDLE hIntermediateUAV,	//11
			Microsoft::WRL::ComPtr<ID3D12Resource> pIntermediateSRV, D3D12_GPU_DESCRIPTOR_HANDLE hIntermediateSRV	//12
		)
		{
			m_pRenderContext = pRenderContext;

			m_pFirstPass_CommandListRef	= pFirstPass_CommandList;
			m_pSecondPass_CommandListRef = pSecondPass_CommandList;
			m_WindowDimensions	= WindowDimensions;

			if(!m_pSourceUAVRef.Get())
				m_pSourceUAVRef = pSourceUAV;
			if(!m_pSourceSRVRef.Get())
				m_pSourceSRVRef = pSourceSRV;
			if(!m_pIntermediateUAVRef.Get())
				m_pIntermediateUAVRef = pIntermediateUAV;
			if(!m_pIntermediateSRVRef.Get())
				m_pIntermediateSRVRef = pIntermediateSRV;

			m_hSourceUAV = hSourceUAV;
			m_hSourceSRV = hSourceSRV;
			m_hIntermediateUAV = hIntermediateUAV;
			m_hIntermediateSRV = hIntermediateSRV;

			LoadPipeline(pDevice);
		}

		inline void SetSourceUAV(Microsoft::WRL::ComPtr<ID3D12Resource> pSourceUAV, D3D12_GPU_DESCRIPTOR_HANDLE hSourceUAV)
		{ m_pSourceUAVRef = pSourceUAV; m_hSourceUAV = hSourceUAV; }
		inline void SetSourceSRV(Microsoft::WRL::ComPtr<ID3D12Resource> pSourceSRV, D3D12_GPU_DESCRIPTOR_HANDLE hSourceSRV)
		{ m_pSourceSRVRef = pSourceSRV; m_hSourceSRV = hSourceSRV; }

		inline void SetIntermediateUAV(Microsoft::WRL::ComPtr<ID3D12Resource> pIntermediateUAV, D3D12_GPU_DESCRIPTOR_HANDLE hIntermediateUAV)
		{ m_pIntermediateUAVRef = pIntermediateUAV; m_hIntermediateUAV = hIntermediateUAV; }
		
		inline void SetIntermediateSRV(Microsoft::WRL::ComPtr<ID3D12Resource> pIntermediateSRV, D3D12_GPU_DESCRIPTOR_HANDLE hIntermediateSRV)
		{ m_pIntermediateSRVRef = pIntermediateSRV; m_hIntermediateSRV = hIntermediateSRV; }

		void Execute(FrameResources* pFrameResources);

	protected:
		void LoadPipeline(Microsoft::WRL::ComPtr<ID3D12Device> pDevice);

	private:
		Direct3D12Context* m_pRenderContext;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pFirstPass_CommandListRef;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pSecondPass_CommandListRef;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_pRootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pPipelineState;
		std::pair<uint32_t, uint32_t> m_WindowDimensions;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_pSourceUAVRef; D3D12_GPU_DESCRIPTOR_HANDLE m_hSourceUAV;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_pSourceSRVRef; D3D12_GPU_DESCRIPTOR_HANDLE m_hSourceSRV;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_pIntermediateUAVRef; D3D12_GPU_DESCRIPTOR_HANDLE m_hIntermediateUAV;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_pIntermediateSRVRef; D3D12_GPU_DESCRIPTOR_HANDLE m_hIntermediateSRV;
	};

}
