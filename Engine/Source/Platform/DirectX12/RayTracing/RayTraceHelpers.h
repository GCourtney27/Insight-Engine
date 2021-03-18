#pragma once

#include <Runtime/Core.h>

#include "DXR/nv_helpers_dx12/TopLevelASGenerator.h"
#include "DXR/nv_helpers_dx12/ShaderBindingTableGenerator.h"
#include "Platform/DirectX12/Wrappers/DescriptorHeapWrapper.h"


namespace Insight {

	class Direct3D12Context;

	
	namespace NvidiaHelpers = nv_helpers_dx12;

	class INSIGHT_API RayTraceHelpers
	{
	public:
		struct AccelerationStructureBuffers
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> pScratch;      // Scratch memory for AS builder
			Microsoft::WRL::ComPtr<ID3D12Resource> pResult;       // Where the AS is
			Microsoft::WRL::ComPtr<ID3D12Resource> pInstanceDesc; // Hold the matrices of the instances
		};
	private:
		struct CB_CHS_LightParams
		{
			DirectX::XMFLOAT4 DirLightDirection;
			float ShadowDarkness;
			float padding[3];
		};
		struct CB_RG_CameraParams
		{
			DirectX::XMMATRIX InverseView;
			DirectX::XMMATRIX InverseProjection;
		};
	public:
		RayTraceHelpers() = default;
		~RayTraceHelpers() = default;

		bool Init(Direct3D12Context* pRendererContext, ID3D12GraphicsCommandList4* pCommandList);
		void GenerateAccelerationStructure();
		void Destroy();
		void UpdateCBVs();
		void SetCommonPipeline();
		void TraceScene();
		inline void ReCreateOutputBuffer() { CreateRTOutputBuffer(); }

		inline void UpdateInstanceTransformByIndex(uint32_t ArrIndex, DirectX::XMMATRIX UpdatedMat) 
		{
			m_Instances[ArrIndex].second = UpdatedMat; 
		}

		inline ID3D12Resource* GetOutputBuffer() { return m_pOutputBuffer_UAV.Get(); }
		uint32_t RegisterBottomLevelASGeometry(Microsoft::WRL::ComPtr<ID3D12Resource> pVertexBuffer, Microsoft::WRL::ComPtr<ID3D12Resource> pIndexBuffer, uint32_t NumVeticies, uint32_t NumIndices, DirectX::XMMATRIX& WorldMat);

	private:
		AccelerationStructureBuffers CreateBottomLevelAS(std::vector<std::pair<Microsoft::WRL::ComPtr<ID3D12Resource>, uint32_t>> VertexBuffers, std::vector<std::pair<Microsoft::WRL::ComPtr<ID3D12Resource>, uint32_t>> IndexBuffers = {});
		void CreateTopLevelAS(const std::vector<std::pair<Microsoft::WRL::ComPtr<ID3D12Resource>, DirectX::XMMATRIX>>& instances, bool UpdateOnly = false);
		void CreateAccelerationStructures();

		void CreateRTPipeline();
		void CreateRTOutputBuffer();
		void CreateShaderBindingTable();
		void CreateShaderResourceHeap();
		void CreateBuffers();
		
		Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateRayGenSignature();
		Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateMissSignature();
		Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateHitSignature();

	private:
		uint32_t m_NextAvailabledInstanceArrIndex = 0U;

		std::vector< std::pair<Microsoft::WRL::ComPtr<ID3D12Resource>, uint32_t> > m_ASVertexBuffers;
		std::vector< std::pair<Microsoft::WRL::ComPtr<ID3D12Resource>, uint32_t> > m_ASIndexBuffers;

		Microsoft::WRL::ComPtr<ID3D12Device5>				m_pDeviceRef;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>	m_pCommandListRef;

		uint32_t m_WindowWidth = 0U;
		uint32_t m_WindowHeight = 0U;
		Direct3D12Context* m_pRenderContextRef;
		D3D12_DISPATCH_RAYS_DESC m_DispatchRaysDesc = {};

		// 0: Ray Tracing output buffer
		// 1: Top-level acceleration structure
		// 2: Camera constant buffer
		// 3: light constant buffer
		CDescriptorHeapWrapper				m_srvUavHeap;

		Microsoft::WRL::ComPtr<ID3D12Resource>	m_pCameraBuffer;
		int										m_CameraBufferSize = 0;
		CB_RG_CameraParams						m_CBCameraParams;
		Microsoft::WRL::ComPtr<ID3D12Resource>	m_pLightBuffer;
		int										m_LightBufferSize = 0;
		CB_CHS_LightParams						m_CBLightParams;

		Microsoft::WRL::ComPtr<ID3D12Resource>	m_pOutputBuffer_UAV;

		NvidiaHelpers::TopLevelASGenerator		m_TopLevelASGenerator;
		AccelerationStructureBuffers			m_TopLevelASBuffers;

		std::vector<std::pair<Microsoft::WRL::ComPtr<ID3D12Resource>, DirectX::XMMATRIX>> m_Instances;
		std::vector<AccelerationStructureBuffers> m_AccelerationStructureBuffers;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RayGenSignature;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_HitSignature;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_MissSignature;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_ShadowSignature;

		Microsoft::WRL::ComPtr<ID3D12StateObject> m_rtStateObject;
		Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> m_rtStateObjectProps;

		NvidiaHelpers::ShaderBindingTableGenerator m_sbtHelper;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_sbtStorage;
	};

}