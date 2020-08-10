#pragma once

#include <Insight/Core.h>

#include "DXR/nv_helpers_dx12/TopLevelASGenerator.h"
#include "DXR/nv_helpers_dx12/ShaderBindingTableGenerator.h"


namespace Insight {

	class ieD3D12SphereRenderer;
	class Direct3D12Context;

	using Microsoft::WRL::ComPtr;
	namespace NvidiaHelpers = nv_helpers_dx12;

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

		bool OnInit(ComPtr<ID3D12Device> pDevice, ComPtr<ID3D12GraphicsCommandList4> pRTCommandList, std::pair<uint32_t, uint32_t> WindowDimensions, Direct3D12Context* pRendererContext);
		void GenerateAccelerationStructure();
		void OnPostInit();
		void OnDestroy();
		void UpdateCBVs();
		void SetCommonPipeline();
		void TraceScene();

		ID3D12Resource* GetOutputBuffer() { return m_pOutputBuffer_UAV.Get(); }
		void RegisterBottomLevelASGeometry(ComPtr<ID3D12Resource> pVertexBuffer, ComPtr<ID3D12Resource> pIndexBuffer, size_t NumVeticies, size_t NumIndices, DirectX::XMMATRIX WorldMat);

	private:
		AccelerationStructureBuffers CreateBottomLevelAS(std::vector<std::pair<ComPtr<ID3D12Resource>, uint32_t>> VertexBuffers, std::vector<std::pair<ComPtr<ID3D12Resource>, uint32_t>> IndexBuffers = {});
		void CreateTopLevelAS(const std::vector<std::pair<ComPtr<ID3D12Resource>, DirectX::XMMATRIX>>& instances, bool UpdateOnly = false);
		void CreateAccelerationStructures();

		void CreateRaytracingPipeline();
		void CreateRaytracingOutputBuffer();
		void CreateShaderBindingTable();
		void CreateShaderResourceHeap();
		void CreateCameraBuffer();

		ComPtr<ID3D12RootSignature> CreateRayGenSignature();
		ComPtr<ID3D12RootSignature> CreateMissSignature();
		ComPtr<ID3D12RootSignature> CreateHitSignature();

		// TEMP
		void LoadDemoAssets();
		//ieD3D12SphereRenderer* m_pSphere;

	private:
		uint32_t m_Time = 0U;// TEMP

		std::vector< std::pair<ComPtr<ID3D12Resource>, uint32_t> > m_ASVertexBuffers;
		std::vector< std::pair<ComPtr<ID3D12Resource>, uint32_t> > m_ASIndexBuffers;

		ComPtr<ID3D12Device5>				m_pDeviceRef;
		ComPtr<ID3D12GraphicsCommandList4>	m_pRayTracePass_CommandListRef;

		uint32_t m_WindowWidth = 0U;
		uint32_t m_WindowHeight = 0U;
		Direct3D12Context* m_pRendererContext;
		D3D12_DISPATCH_RAYS_DESC m_DispatchRaysDesc = {};

		ComPtr<ID3D12DescriptorHeap>			m_srvUavHeap;
		ComPtr<ID3D12DescriptorHeap>			m_ConstHeap;
		
		ComPtr<ID3D12Resource>					m_CameraBuffer;
		int										m_CameraBufferSize = 0;

		ComPtr<ID3D12Resource>					m_pOutputBuffer_UAV;

		NvidiaHelpers::TopLevelASGenerator		m_TopLevelASGenerator;
		AccelerationStructureBuffers			m_TopLevelASBuffers;

		std::vector<std::pair<ComPtr<ID3D12Resource>, DirectX::XMMATRIX>> m_Instances;
		std::vector<AccelerationStructureBuffers> m_AccelerationStructureBuffers;
		
		ComPtr<IDxcBlob> m_RayGenLibrary;
		ComPtr<IDxcBlob> m_HitLibrary;
		ComPtr<IDxcBlob> m_MissLibrary;

		ComPtr<ID3D12RootSignature> m_RayGenSignature;
		ComPtr<ID3D12RootSignature> m_HitSignature;
		ComPtr<ID3D12RootSignature> m_MissSignature;

		ComPtr<ID3D12StateObject> m_rtStateObject;
		ComPtr<ID3D12StateObjectProperties> m_rtStateObjectProps;

		NvidiaHelpers::ShaderBindingTableGenerator m_sbtHelper;
		ComPtr<ID3D12Resource> m_sbtStorage;
	};

}