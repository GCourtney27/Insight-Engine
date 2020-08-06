#include <ie_pch.h>

#include "Ray_Trace_Helpers.h"

#include "Insight/Core/Application.h"
#include "Insight/Rendering/Geometry/Vertex.h"

#include "Platform/Windows/DirectX_12/Geometry/D3D12_Sphere_Renderer.h"
#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"

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
		CreateRaytracingPipeline();
		CreateRaytracingOutputBuffer();

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

	void RayTraceHelpers::CreateRaytracingPipeline()
	{
		nv_helpers_dx12::RayTracingPipelineGenerator Pipeline(reinterpret_cast<ID3D12Device5*>(m_pDeviceRef.Get()));
		
		// TODO Change the shader paths to be path independent!
		m_RayGenLibrary = nv_helpers_dx12::CompileShaderLibrary(L"Source/Shaders/HLSL/Ray_Tracing/RayGen.hlsl");
		m_MissLibrary = nv_helpers_dx12::CompileShaderLibrary(L"Source/Shaders/HLSL/Ray_Tracing/Miss.hlsl");
		m_HitLibrary = nv_helpers_dx12::CompileShaderLibrary(L"Source/Shaders/HLSL/Ray_Tracing/Hit.hlsl");

		Pipeline.AddLibrary(m_RayGenLibrary.Get(), { L"RayGen" });
		Pipeline.AddLibrary(m_MissLibrary.Get(), { L"Miss" });
		Pipeline.AddLibrary(m_HitLibrary.Get(), { L"ClosestHit" });

		m_RayGenSignature = CreateRayGenSignature();
		m_MissSignature = CreateMissSignature();
		m_HitSignature = CreateHitSignature();

		Pipeline.AddHitGroup(L"HitGroup", L"ClosestHit");

		Pipeline.AddRootSignatureAssociation(m_RayGenSignature.Get(), { L"RayGen" });
		Pipeline.AddRootSignatureAssociation(m_MissSignature.Get(), { L"Miss" });
		Pipeline.AddRootSignatureAssociation(m_HitSignature.Get(), { L"HitGroup" });

		Pipeline.SetMaxPayloadSize(4 * sizeof(float)); // RGB + distance

		Pipeline.SetMaxAttributeSize(2 * sizeof(float)); // barycentric coordinates

		Pipeline.SetMaxRecursionDepth(1);

		// Compile the pipeline for execution on the GPU
		m_rtStateObject = Pipeline.Generate();

		ThrowIfFailed(m_rtStateObject->QueryInterface(IID_PPV_ARGS(&m_rtStateObjectProps)), "Failed to query interface when creating rt state object.");
	}

	void RayTraceHelpers::CreateRaytracingOutputBuffer()
	{
		CDescriptorHeapWrapper& cbvsrvHeap = reinterpret_cast<Direct3D12Context*>(&Renderer::Get())->GetCBVSRVDescriptorHeap();

		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.DepthOrArraySize = 1;
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		// The backbuffer is actually DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, but sRGB
		// formats cannot be used with UAVs. For accuracy we should convert to sRGB
		// ourselves in the shader
		resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		resDesc.Width = static_cast<UINT64>(Application::Get().GetWindow().GetWidth());
		resDesc.Height = static_cast<UINT64>(Application::Get().GetWindow().GetHeight());
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resDesc.MipLevels = 1;
		resDesc.SampleDesc.Count = 1;
		ThrowIfFailed(m_pDeviceRef->CreateCommittedResource(
			&nv_helpers_dx12::kDefaultHeapProps, D3D12_HEAP_FLAG_NONE, &resDesc,
			D3D12_RESOURCE_STATE_COPY_SOURCE, nullptr,
			IID_PPV_ARGS(&m_OutputBuffer)), "Failed to create commited resource for RT output buffer.");

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		m_pDeviceRef->CreateUnorderedAccessView(m_OutputBuffer.Get(), nullptr, &uavDesc, cbvsrvHeap.hCPU(6));

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.RaytracingAccelerationStructure.Location = m_TopLevelASBuffers.pResult->GetGPUVirtualAddress();
		m_pDeviceRef->CreateShaderResourceView(nullptr, &srvDesc, cbvsrvHeap.hCPU(7));
	}

	void RayTraceHelpers::CreateShaderBindingTable()
	{

	}

	ComPtr<ID3D12RootSignature> RayTraceHelpers::CreateRayGenSignature()
	{
		nv_helpers_dx12::RootSignatureGenerator rsc;
		rsc.AddHeapRangesParameter(
			{ {0 /*u0*/, 1 /*1 descriptor */, 0 /*use the implicit register space 0*/,
			  D3D12_DESCRIPTOR_RANGE_TYPE_UAV /* UAV representing the output buffer*/,
			  0 /*heap slot where the UAV is defined*/},
			 {0 /*t0*/, 1, 0,
			  D3D12_DESCRIPTOR_RANGE_TYPE_SRV /*Top-level acceleration structure*/,
			  1} }
		);

		return rsc.Generate(m_pDeviceRef.Get(), true);
	}

	ComPtr<ID3D12RootSignature> RayTraceHelpers::CreateMissSignature()
	{
		nv_helpers_dx12::RootSignatureGenerator rsc;
		return rsc.Generate(m_pDeviceRef.Get(), true);
	}

	ComPtr<ID3D12RootSignature> RayTraceHelpers::CreateHitSignature()
	{
		nv_helpers_dx12::RootSignatureGenerator rsc;
		return rsc.Generate(m_pDeviceRef.Get(), true);
	}

	void RayTraceHelpers::LoadDemoAssets()
	{
		m_Sphere = new ieD3D12SphereRenderer();
		m_Sphere->Init(10, 20, 20);
	}

}