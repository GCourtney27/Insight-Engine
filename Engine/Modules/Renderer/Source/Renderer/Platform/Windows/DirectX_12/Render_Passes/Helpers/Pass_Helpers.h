#pragma once

#include <Insight/Core.h>
#include "Renderer/Platform/Windows/DirectX_12/Wrappers/Descriptor_Heap_Wrapper.h"

namespace Insight {

	class FrameResources;

	/*
		A downsampler that can be used to pack a SRV into a UAV.
	*/
	class ThresholdDownSampleHelper
	{
	public:
		friend class BloomPass;
	public:
		ThresholdDownSampleHelper() = default;
		~ThresholdDownSampleHelper() = default;

		inline void Create(ComPtr<ID3D12Device> pDevice, std::pair<uint32_t, uint32_t> DownsampleTargetDimensions, ComPtr<ID3D12GraphicsCommandList> pCommandList, CDescriptorHeapWrapper* pCBVSRVHeap, D3D12_GPU_DESCRIPTOR_HANDLE hDownsampleResource)
		{
			m_pCommandListRef = pCommandList;
			m_UAVDimensions = DownsampleTargetDimensions;
			m_pCBVSRVHeapRef = pCBVSRVHeap;
			m_hDownsampleResource = hDownsampleResource;

			LoadPipeline(pDevice);
			CreateResources(pDevice);
		}

		void Execute(FrameResources* pFrameResources);

	protected:
		void LoadPipeline(ComPtr<ID3D12Device> pDevice);
		void CreateResources(ComPtr<ID3D12Device> pDevice);

	private:
		ComPtr<ID3D12RootSignature>			m_pRootSignature;
		ComPtr<ID3D12PipelineState>			m_pPipeilneState;
		ComPtr<ID3D12GraphicsCommandList>	m_pCommandListRef;
		
		std::pair<uint32_t, uint32_t>		m_UAVDimensions;
		CDescriptorHeapWrapper*				m_pCBVSRVHeapRef = nullptr;
		ComPtr<ID3D12Resource>				m_pDownsampleResult_UAV;

		D3D12_GPU_DESCRIPTOR_HANDLE m_hDownsampleResource;

	};

	class GaussianBlurHelper
	{
	public:
		GaussianBlurHelper() = default;
		~GaussianBlurHelper() = default;

		inline void Create(ComPtr<ID3D12Device> pDevice, ComPtr<ID3D12GraphicsCommandList> pCommandList)
		{
			m_pCommandListRef = pCommandList;

			LoadPipeline(pDevice);
			CreateResources(pDevice);
		}

		/*
			Blur the texture.
			@Param BlurDirection - 0: Horizontal, 1: Vertical
		*/
		void Execute(FrameResources* pFrameResources, uint8_t BlurDirection);

	protected:
		void LoadPipeline(ComPtr<ID3D12Device> pDevice);
		void CreateResources(ComPtr<ID3D12Device> pDevice);

	private:
		ComPtr<ID3D12GraphicsCommandList> m_pCommandListRef;
		ComPtr<ID3D12Resource> m_BlurTargetTextureRef;
		ComPtr<ID3D12RootSignature> m_pRootSignature;
		ComPtr<ID3D12PipelineState> m_pPipelineState;
	};

}
