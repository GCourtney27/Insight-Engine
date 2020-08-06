#include <ie_pch.h>

#include "Ray_Trace_Helpers.h"

#include "Insight/Rendering/Geometry/Vertex.h"

#include "Platform/Windows/Error/COM_Exception.h"
#include "Platform/Windows/DirectX_12/Geometry/D3D12_Sphere_Renderer.h"

#include "DXR/DXRHelper.h"
#include "DXR/nv_helpers_dx12/BottomLevelASGenerator.h"
#include "DXR/nv_helpers_dx12/RootSignatureGenerator.h"
#include "DXR/nv_helpers_dx12/ShaderBindingTableGenerator.h"
#include "DXR/nv_helpers_dx12/RaytracingPipelineGenerator.h"

namespace Insight {



	bool RayTraceHelpers::OnInit(ComPtr<ID3D12Device> pDevice, ComPtr<ID3D12GraphicsCommandList4> pRTCommandList)
	{
		m_pDeviceRef = pDevice;
		m_pRayTracePass_CommandList = pRTCommandList;

		LoadDemoAssets();

		CreateAccelerationStructures();

		return true;
	}

	void RayTraceHelpers::OnPostInit()
	{
		m_bottomLevelAS = m_TempBottomLevelBuffers.pResult;

		m_TempBottomLevelBuffers.pScratch->Release();
		m_TempBottomLevelBuffers.pResult->Release();
	}

	void RayTraceHelpers::OnDestroy()
	{
		delete m_Sphere;
	}

	RayTraceHelpers::AccelerationStructureBuffers RayTraceHelpers::CreateBottomLevelAS(std::vector<std::pair<ComPtr<ID3D12Resource>, uint32_t>> vVertexBuffers)
	{
		nv_helpers_dx12::BottomLevelASGenerator BottomLevelAS;

		for (const auto& buffer : vVertexBuffers) {
			BottomLevelAS.AddVertexBuffer(buffer.first.Get(), 0, buffer.second, sizeof(Vertex3D), 0, 0);
		}

		UINT64 ScratchSizeInBytes = 0;
		UINT64 ResultSizeInBytes = 0;

		BottomLevelAS.ComputeASBufferSizes(reinterpret_cast<ID3D12Device5*>(m_pDeviceRef.Get()), false, &ScratchSizeInBytes, &ResultSizeInBytes);

		AccelerationStructureBuffers buffers;
		buffers.pScratch = nv_helpers_dx12::CreateBuffer(
			m_pDeviceRef.Get(),
			ScratchSizeInBytes,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, 
			D3D12_RESOURCE_STATE_COMMON,
			nv_helpers_dx12::kDefaultHeapProps
		);
		buffers.pResult = nv_helpers_dx12::CreateBuffer(
			m_pDeviceRef.Get(), 
			ResultSizeInBytes,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
			nv_helpers_dx12::kDefaultHeapProps
		);

		BottomLevelAS.Generate(m_pRayTracePass_CommandList.Get(), buffers.pScratch.Get(), buffers.pResult.Get(), false, nullptr);

		return buffers;
	}

	void RayTraceHelpers::CreateTopLevelAS(const std::vector<std::pair<ComPtr<ID3D12Resource>, DirectX::XMMATRIX>>& instances)
	{
		for (size_t i = 0; i < instances.size(); i++) {
			m_TopLevelASGenerator.AddInstance(instances[i].first.Get(), instances[i].second, static_cast<UINT>(i), static_cast<UINT>(0));
		}

		UINT64 ScratchSize, ResultSize, InstanceDescsSize;

		m_TopLevelASGenerator.ComputeASBufferSizes(reinterpret_cast<ID3D12Device5*>(m_pDeviceRef.Get()), true, &ScratchSize, &ResultSize, &InstanceDescsSize);

		m_TopLevelASBuffers.pScratch = nv_helpers_dx12::CreateBuffer(
			m_pDeviceRef.Get(), ScratchSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			nv_helpers_dx12::kDefaultHeapProps);
		m_TopLevelASBuffers.pResult = nv_helpers_dx12::CreateBuffer(
			m_pDeviceRef.Get(), ResultSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
			nv_helpers_dx12::kDefaultHeapProps);

		m_TopLevelASBuffers.pInstanceDesc = nv_helpers_dx12::CreateBuffer(
			m_pDeviceRef.Get(),
			InstanceDescsSize, 
			D3D12_RESOURCE_FLAG_NONE,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nv_helpers_dx12::kUploadHeapProps
		);
		
		m_TopLevelASGenerator.Generate(
			m_pRayTracePass_CommandList.Get(),
			m_TopLevelASBuffers.pScratch.Get(),
			m_TopLevelASBuffers.pResult.Get(),
			m_TopLevelASBuffers.pInstanceDesc.Get()
		);
	}

	void RayTraceHelpers::CreateAccelerationStructures()
	{
		m_TempBottomLevelBuffers = CreateBottomLevelAS({ {m_Sphere->GetVertexBuffer(), m_Sphere->GetVertexCount()} });

		// Just one instance for now
		m_Instances = { {m_TempBottomLevelBuffers.pResult, DirectX::XMMatrixIdentity()} };
		CreateTopLevelAS(m_Instances);
	}

	void RayTraceHelpers::LoadDemoAssets()
	{
		m_Sphere = new ieD3D12SphereRenderer();
		m_Sphere->Init(10, 20, 20);
	}

}