#pragma once

#include <Insight/Core.h>

#include "DXR/nv_helpers_dx12/TopLevelASGenerator.h"


namespace Insight {

	class ieD3D12SphereRenderer;

	using Microsoft::WRL::ComPtr;

	class INSIGHT_API RayTraceHelpers
	{
	public:
		struct AccelerationStructureBuffers
		{
			ComPtr<ID3D12Resource> pScratch;      // Scratch memory for AS builder
			ComPtr<ID3D12Resource> pResult;       // Where the AS is
			ComPtr<ID3D12Resource> pInstanceDesc; // Hold the matrices of the instances
		};
	public:
		RayTraceHelpers() = default;
		~RayTraceHelpers() = default;

		bool OnInit(ComPtr<ID3D12Device> pDevice, ComPtr<ID3D12GraphicsCommandList4> pRTCommandList);
		void OnPostInit();
		void OnDestroy();

	private:
		AccelerationStructureBuffers CreateBottomLevelAS(std::vector<std::pair<ComPtr<ID3D12Resource>, uint32_t>> vVertexBuffers);
		void CreateTopLevelAS(const std::vector<std::pair<ComPtr<ID3D12Resource>, DirectX::XMMATRIX>>& instances);
		void CreateAccelerationStructures();
		// TEMP
		void LoadDemoAssets(); 	ieD3D12SphereRenderer* m_Sphere;

	private:
		
		ComPtr<ID3D12Device>				m_pDeviceRef;
		ComPtr<ID3D12GraphicsCommandList4>	m_pRayTracePass_CommandList;

		ComPtr<ID3D12Resource>					m_bottomLevelAS;

		nv_helpers_dx12::TopLevelASGenerator	m_TopLevelASGenerator;
		AccelerationStructureBuffers			m_TopLevelASBuffers;
		AccelerationStructureBuffers			m_TempBottomLevelBuffers;

		std::vector<std::pair<ComPtr<ID3D12Resource>, DirectX::XMMATRIX>> m_Instances;

	};

}