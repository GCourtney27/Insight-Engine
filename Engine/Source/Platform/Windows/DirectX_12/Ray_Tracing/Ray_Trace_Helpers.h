#pragma once

#include <Insight/Core.h>

#include "DXR/nv_helpers_dx12/TopLevelASGenerator.h"
#include "DXR/nv_helpers_dx12/ShaderBindingTableGenerator.h"


namespace Insight {

	class ieD3D12SphereRenderer;
	class Direct3D12Context;

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

		bool OnInit(ComPtr<ID3D12Device> pDevice, ComPtr<ID3D12GraphicsCommandList4> pRTCommandList, std::pair<uint32_t, uint32_t> WindowDimensions, Direct3D12Context* pRendererContext);
		void OnPostInit();
		void OnDestroy();
		void UpdateCBVs();
		void SetCommonPipeline();
		void TraceScene();

		ID3D12Resource* GetOutputBuffer() { return m_OutputBuffer_UAV.Get(); }

	private:
		AccelerationStructureBuffers CreateBottomLevelAS(std::vector<std::pair<ComPtr<ID3D12Resource>, uint32_t>> vVertexBuffers, std::vector<std::pair<ComPtr<ID3D12Resource>, uint32_t>> vIndexBuffers = {});
		void CreateTopLevelAS(const std::vector<std::pair<ComPtr<ID3D12Resource>, DirectX::XMMATRIX>>& instances);
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
		struct SimpleVertex3D
		{
			SimpleVertex3D() {}
			SimpleVertex3D(float x, float y, float z, float r, float u, float v) : pos(x, y, z), textCoord(u, v) {}

			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT2 textCoord;
		};
		void LoadDemoAssets(); 	/*ieD3D12SphereRenderer* m_Sphere*/;
		int numCubeIndices;
		int numCubeVerticies;
		ComPtr<ID3D12Resource> pIndexBuffer;
		ComPtr<ID3D12Resource> pVertexBuffer;
	private:
		
		ComPtr<ID3D12Device5>				m_pDeviceRef;
		ComPtr<ID3D12GraphicsCommandList4>	m_pRayTracePass_CommandList;

		uint32_t m_WindowWidth = 0U;
		uint32_t m_WindowHeight = 0U;
		D3D12_DISPATCH_RAYS_DESC DispatchRaysDesc = {};
		Direct3D12Context* m_pRendererContext;

		ComPtr<ID3D12DescriptorHeap>			m_srvUavHeap;
		ComPtr<ID3D12DescriptorHeap>			m_ConstHeap;
		
		ComPtr<ID3D12Resource>					m_CameraBuffer;
		int										m_CameraBufferSize = 0;

		ComPtr<ID3D12Resource>					m_OutputBuffer_UAV;
		ComPtr<ID3D12Resource>					m_BottomLevelAS;

		nv_helpers_dx12::TopLevelASGenerator	m_TopLevelASGenerator;
		AccelerationStructureBuffers			m_TopLevelASBuffers;
		AccelerationStructureBuffers			m_TempBottomLevelBuffers;

		std::vector<std::pair<ComPtr<ID3D12Resource>, DirectX::XMMATRIX>> m_Instances;

		ComPtr<IDxcBlob> m_RayGenLibrary;
		ComPtr<IDxcBlob> m_HitLibrary;
		ComPtr<IDxcBlob> m_MissLibrary;

		ComPtr<ID3D12RootSignature> m_RayGenSignature;
		ComPtr<ID3D12RootSignature> m_HitSignature;
		ComPtr<ID3D12RootSignature> m_MissSignature;

		ComPtr<ID3D12StateObject> m_rtStateObject;
		ComPtr<ID3D12StateObjectProperties> m_rtStateObjectProps;

		nv_helpers_dx12::ShaderBindingTableGenerator m_sbtHelper;
		ComPtr<ID3D12Resource> m_sbtStorage;
	};

}