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



	bool RayTraceHelpers::OnInit(ComPtr<ID3D12Device> pDevice, ComPtr<ID3D12GraphicsCommandList4> pRTCommandList, std::pair<uint32_t, uint32_t> WindowDimensions, Direct3D12Context* pRendererContext)
	{
		m_pDeviceRef = reinterpret_cast<ID3D12Device5*>(pDevice.Get());
		m_pRayTracePass_CommandListRef = pRTCommandList;
		m_pRendererContext = pRendererContext;
		
		m_WindowWidth = WindowDimensions.first;
		m_WindowHeight = WindowDimensions.second;

		//LoadDemoAssets();


		return true;
	}

	void RayTraceHelpers::GenerateAccelerationStructure()
	{
		CreateAccelerationStructures();
		CreateRaytracingPipeline();
		CreateRaytracingOutputBuffer();
		CreateCameraBuffer();
		CreateShaderResourceHeap();
		CreateShaderBindingTable();
	}

	void RayTraceHelpers::OnPostInit()
	{
	}

	void RayTraceHelpers::OnDestroy()
	{
		//delete m_Sphere;
	}

	void RayTraceHelpers::UpdateCBVs()
	{
		m_Time++;

		//m_Instances[0].second = XMMatrixRotationAxis({ 0.f, 1.f, 0.f }, static_cast<float>(m_Time) / 50.0f) *
		//	XMMatrixTranslation(0.f, 0.1f * cosf(m_Time / 20.f), 0.f);

		std::vector<XMMATRIX> matrices(4);

		const CB_PS_VS_PerFrame PerFrameData = m_pRendererContext->GetPerFrameCB();

		matrices[0] = XMLoadFloat4x4(&PerFrameData.view);
		matrices[1] = XMLoadFloat4x4(&PerFrameData.projection);

		matrices[2] = XMLoadFloat4x4(&PerFrameData.inverseView);
		matrices[3] = XMLoadFloat4x4(&PerFrameData.inverseProjection);

		// Copy the matrix contents
		uint8_t* pData;
		ThrowIfFailed(m_CameraBuffer->Map(0, nullptr, (void**)&pData), "Failed to map camera buffer");
		memcpy(pData, matrices.data(), m_CameraBufferSize);
		m_CameraBuffer->Unmap(0, nullptr);

	}

	void RayTraceHelpers::SetCommonPipeline()
	{
		ID3D12DescriptorHeap* ppHeaps[] = { m_srvUavHeap.Get() };
		m_pRayTracePass_CommandListRef->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		m_pRayTracePass_CommandListRef->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pOutputBuffer_UAV.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

		m_DispatchRaysDesc = {};
		uint32_t RayGenerationSectionSizeInBytes = m_sbtHelper.GetRayGenSectionSize();
		m_DispatchRaysDesc.RayGenerationShaderRecord.StartAddress = m_sbtStorage->GetGPUVirtualAddress();
		m_DispatchRaysDesc.RayGenerationShaderRecord.SizeInBytes = RayGenerationSectionSizeInBytes;

		uint32_t MissSectionSizeInBytes = m_sbtHelper.GetMissSectionSize();
		m_DispatchRaysDesc.MissShaderTable.StartAddress = m_sbtStorage->GetGPUVirtualAddress() + RayGenerationSectionSizeInBytes;
		m_DispatchRaysDesc.MissShaderTable.SizeInBytes = MissSectionSizeInBytes;
		m_DispatchRaysDesc.MissShaderTable.StrideInBytes = m_sbtHelper.GetMissEntrySize();

		uint32_t HitGroupsSectionSize = m_sbtHelper.GetHitGroupSectionSize();
		m_DispatchRaysDesc.HitGroupTable.StartAddress = m_sbtStorage->GetGPUVirtualAddress() + RayGenerationSectionSizeInBytes + MissSectionSizeInBytes;
		m_DispatchRaysDesc.HitGroupTable.SizeInBytes = HitGroupsSectionSize;
		m_DispatchRaysDesc.HitGroupTable.StrideInBytes = m_sbtHelper.GetHitGroupEntrySize();

		m_DispatchRaysDesc.Width = m_WindowWidth;
		m_DispatchRaysDesc.Height = m_WindowHeight;
		m_DispatchRaysDesc.Depth = 1;

		m_pRayTracePass_CommandListRef->SetPipelineState1(m_rtStateObject.Get());

		// Animate the instance
		//CreateTopLevelAS(m_Instances, true);
	}

	void RayTraceHelpers::TraceScene()
	{
		m_pRayTracePass_CommandListRef->DispatchRays(&m_DispatchRaysDesc);

		m_pRayTracePass_CommandListRef->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pOutputBuffer_UAV.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE));
	}

	void RayTraceHelpers::RegisterBottomLevelASGeometry(ComPtr<ID3D12Resource> pVertexBuffer, ComPtr<ID3D12Resource> pIndexBuffer, size_t NumVeticies, size_t NumIndices, XMMATRIX WorldMat)
	{
		m_ASVertexBuffers.push_back(std::pair(pVertexBuffer, NumVeticies));
		m_ASIndexBuffers.push_back(std::pair(pIndexBuffer, NumIndices));

		AccelerationStructureBuffers bottomLevelBuffers = CreateBottomLevelAS({ {pVertexBuffer.Get(), NumVeticies} }, { {pIndexBuffer.Get(), NumIndices} });
		//m_Instances = { {bottomLevelBuffers.pResult, WorldMat} };
		m_Instances.push_back(std::pair(bottomLevelBuffers.pResult, WorldMat));
	}

	RayTraceHelpers::AccelerationStructureBuffers RayTraceHelpers::CreateBottomLevelAS(std::vector<std::pair<ComPtr<ID3D12Resource>, uint32_t>> VertexBuffers, std::vector<std::pair<ComPtr<ID3D12Resource>, uint32_t>> IndexBuffers)
	{
		NvidiaHelpers::BottomLevelASGenerator BottomLevelAS;

		for (size_t i = 0; i < VertexBuffers.size(); i++) {

			if (i < IndexBuffers.size() && IndexBuffers[i].second > 0) {

				BottomLevelAS.AddVertexBuffer(VertexBuffers[i].first.Get(), 0,
					VertexBuffers[i].second, sizeof(Vertex3D),
					IndexBuffers[i].first.Get(), 0,
					IndexBuffers[i].second, nullptr, 0, true
				);
			}
			else {
				BottomLevelAS.AddVertexBuffer(VertexBuffers[i].first.Get(), 0, VertexBuffers[i].second, sizeof(Vertex3D), 0, 0);
			}
		}

		UINT64 ScratchSizeInBytes = 0;
		UINT64 ResultSizeInBytes = 0;

		BottomLevelAS.ComputeASBufferSizes(m_pDeviceRef.Get(), false, &ScratchSizeInBytes, &ResultSizeInBytes);

		AccelerationStructureBuffers Buffers;
		Buffers.pScratch = NvidiaHelpers::CreateBuffer(
			m_pDeviceRef.Get(), ScratchSizeInBytes,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON,
			NvidiaHelpers::kDefaultHeapProps
		);
		Buffers.pResult = NvidiaHelpers::CreateBuffer(
			m_pDeviceRef.Get(), ResultSizeInBytes,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
			NvidiaHelpers::kDefaultHeapProps
		);

		BottomLevelAS.Generate(m_pRayTracePass_CommandListRef.Get(), Buffers.pScratch.Get(), Buffers.pResult.Get(), false, nullptr);

		return Buffers;
	}

	void RayTraceHelpers::CreateTopLevelAS(const std::vector<std::pair<ComPtr<ID3D12Resource>, DirectX::XMMATRIX>>& instances, bool UpdateOnly /*= false*/)
	{
		if (!UpdateOnly) {

			for (size_t i = 0; i < instances.size(); i++) {
				m_TopLevelASGenerator.AddInstance(instances[i].first.Get(), instances[i].second, static_cast<UINT>(i), static_cast<UINT>(0));
			}

			UINT64 scratchSize, resultSize, instanceDescsSize;
			m_TopLevelASGenerator.ComputeASBufferSizes(m_pDeviceRef.Get(), true, &scratchSize, &resultSize, &instanceDescsSize);

			m_TopLevelASBuffers.pScratch = NvidiaHelpers::CreateBuffer(m_pDeviceRef.Get(),
				resultSize,
				D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
				D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
				NvidiaHelpers::kDefaultHeapProps
			);
			m_TopLevelASBuffers.pResult = NvidiaHelpers::CreateBuffer(m_pDeviceRef.Get(),
				resultSize,
				D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
				D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
				NvidiaHelpers::kDefaultHeapProps
			);
			m_TopLevelASBuffers.pInstanceDesc = NvidiaHelpers::CreateBuffer(m_pDeviceRef.Get(),
				instanceDescsSize,
				D3D12_RESOURCE_FLAG_NONE,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				NvidiaHelpers::kUploadHeapProps
			);
		}

		
		m_TopLevelASGenerator.Generate(m_pRayTracePass_CommandListRef.Get(),
			m_TopLevelASBuffers.pScratch.Get(),
			m_TopLevelASBuffers.pResult.Get(),
			m_TopLevelASBuffers.pInstanceDesc.Get(),
			UpdateOnly, 
			m_TopLevelASBuffers.pResult.Get()
		);
	}

	void RayTraceHelpers::CreateAccelerationStructures()
	{
		// TODO: Make a AccelerationStructureBuffers for each mesh
		//AccelerationStructureBuffers bottomLevelBuffers = CreateBottomLevelAS(
		//	{ {m_pSphere->GetVertexBuffer().Get(), m_pSphere->GetVertexCount()} }, { {m_pSphere->GetIndexBuffer(), m_pSphere->GetIndexBufferCount()} }
		//);

		//AccelerationStructureBuffers BottomLevelBuffers = CreateBottomLevelAS(m_ASVertexBuffers, m_ASIndexBuffers);

		//XMMATRIX WorldMat = XMMatrixIdentity();
		//WorldMat = XMMatrixScaling(10.0f, 10.0f, 10.0f);
		//// TODO: Put the AccelerationStructureBuffers from above into the m_Instances with the mesh's world matrix
		//m_Instances = { {bottomLevelBuffers.pResult, WorldMat} };
		
		
		CreateTopLevelAS(m_Instances);
	}

	void RayTraceHelpers::CreateCameraBuffer() {
		
		uint32_t NumMatricies = 4; // view, perspective, viewInv, perspectiveInv
		m_CameraBufferSize = NumMatricies * sizeof(XMMATRIX);

		// Create the constant buffer for all matrices
		m_CameraBuffer = NvidiaHelpers::CreateBuffer(
			m_pDeviceRef.Get(),
			m_CameraBufferSize, 
			D3D12_RESOURCE_FLAG_NONE,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			NvidiaHelpers::kUploadHeapProps
		);

		// Create a descriptor heap that will be used by the rasterization shaders
		m_ConstHeap = NvidiaHelpers::CreateDescriptorHeap(m_pDeviceRef.Get(), 1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);

		// Describe and create the constant buffer view.
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = m_CameraBuffer->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = m_CameraBufferSize;

		// Get a handle to the heap memory on the CPU side, to be able to write the
		// descriptors directly
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = m_ConstHeap->GetCPUDescriptorHandleForHeapStart();
		m_pDeviceRef->CreateConstantBufferView(&cbvDesc, srvHandle);
	}

	void RayTraceHelpers::CreateRaytracingPipeline()
	{
		NvidiaHelpers::RayTracingPipelineGenerator Pipeline(reinterpret_cast<ID3D12Device5*>(m_pDeviceRef.Get()));
		
#ifndef IE_IS_STANDALONE
		LPCWSTR RayGenShaderFolder = L"Source/Shaders/HLSL/Ray_Tracing/RayGen.hlsl";
		LPCWSTR MissShaderFolder = L"Source/Shaders/HLSL/Ray_Tracing/Miss.hlsl";
		LPCWSTR HitShaderFolder = L"Source/Shaders/HLSL/Ray_Tracing/Closest_Hit.hlsl";
#else
		LPCWSTR RayGenShaderFolder = L"RayGen.hlsl";
		LPCWSTR MissShaderFolder = L"Miss.hlsl";
		LPCWSTR HitShaderFolder = L"Closest_Hit.hlsl";
#endif
		// TODO Change the shader paths to be path independent!
		m_RayGenLibrary = NvidiaHelpers::CompileShaderLibrary(RayGenShaderFolder);
		m_MissLibrary = NvidiaHelpers::CompileShaderLibrary(MissShaderFolder);
		m_HitLibrary = NvidiaHelpers::CompileShaderLibrary(HitShaderFolder);

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
		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.DepthOrArraySize = 1;
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		// The backbuffer is actually DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, but sRGB
		// formats cannot be used with UAVs. For accuracy we should convert to sRGB
		// ourselves in the shader
		resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		resDesc.Width = static_cast<UINT64>(m_WindowWidth);
		resDesc.Height = static_cast<UINT64>(m_WindowHeight);
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resDesc.MipLevels = 1;
		resDesc.SampleDesc.Count = 1;
		ThrowIfFailed(m_pDeviceRef->CreateCommittedResource(
			&NvidiaHelpers::kDefaultHeapProps, D3D12_HEAP_FLAG_NONE, &resDesc,
			D3D12_RESOURCE_STATE_COPY_SOURCE, nullptr,
			IID_PPV_ARGS(&m_pOutputBuffer_UAV)), "Failed to create raytracing outptut buffer.");
	}

	void RayTraceHelpers::CreateShaderBindingTable()
	{
		m_sbtHelper.Reset();
		//CDescriptorHeapWrapper& cbvsrvHeap = reinterpret_cast<Direct3D12Context*>(&Renderer::Get())->GetCBVSRVDescriptorHeap();
		D3D12_GPU_DESCRIPTOR_HANDLE srvUavHeapHandle = m_srvUavHeap->GetGPUDescriptorHandleForHeapStart();

		//D3D12_GPU_DESCRIPTOR_HANDLE srvUavHeapHandle = cbvsrvHeap.hGPU(6);
		auto HeapPointer = reinterpret_cast<UINT64*>(srvUavHeapHandle.ptr);

		// The ray generation only uses heap data
		m_sbtHelper.AddRayGenerationProgram(L"RayGen", { HeapPointer });

		// The miss and hit shaders do not access any external resources: instead they
		// communicate their results through the ray payload
		m_sbtHelper.AddMissProgram(L"Miss", {});
		
		for (uint32_t i = 0; i < m_ASVertexBuffers.size(); ++i) {

			m_sbtHelper.AddHitGroup(L"HitGroup", { (void*)m_ASVertexBuffers[i].first->GetGPUVirtualAddress(),
													(void*)m_ASIndexBuffers[i].first->GetGPUVirtualAddress() });

		}

		// Adding the triangle hit shader
		//m_sbtHelper.AddHitGroup(L"HitGroup", { (void*)(m_pSphere->GetVertexBuffer()->GetGPUVirtualAddress()),
		//									   (void*)(m_pSphere->GetIndexBuffer()->GetGPUVirtualAddress()) });

		uint32_t sbtSize = m_sbtHelper.ComputeSBTSize();

		m_sbtStorage = NvidiaHelpers::CreateBuffer(
			m_pDeviceRef.Get(), 
			sbtSize, 
			D3D12_RESOURCE_FLAG_NONE,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			NvidiaHelpers::kUploadHeapProps
		);
		if (!m_sbtStorage) {
			throw std::logic_error("Could not allocate the shader binding table");
		}

		m_sbtHelper.Generate(m_sbtStorage.Get(), m_rtStateObjectProps.Get());
	}

	void RayTraceHelpers::CreateShaderResourceHeap()
	{
		m_srvUavHeap = NvidiaHelpers::CreateDescriptorHeap(m_pDeviceRef.Get(), 3, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);

		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = m_srvUavHeap->GetCPUDescriptorHandleForHeapStart();

		// Output Buffer
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		m_pDeviceRef->CreateUnorderedAccessView(m_pOutputBuffer_UAV.Get(), nullptr, &uavDesc, srvHandle);

		srvHandle.ptr += m_pDeviceRef->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// Tol-Level Accereration Structure
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.RaytracingAccelerationStructure.Location = m_TopLevelASBuffers.pResult->GetGPUVirtualAddress();
		m_pDeviceRef->CreateShaderResourceView(nullptr, &srvDesc, srvHandle);

		srvHandle.ptr +=
			m_pDeviceRef->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// Describe and create a constant buffer view for the camera
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = m_CameraBuffer->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = m_CameraBufferSize;
		m_pDeviceRef->CreateConstantBufferView(&cbvDesc, srvHandle);
	}

	ComPtr<ID3D12RootSignature> RayTraceHelpers::CreateRayGenSignature()
	{
		NvidiaHelpers::RootSignatureGenerator rsc;

		rsc.AddHeapRangesParameter(
			{
				 {0 /*u0*/, 1 /*1 descriptor */, 0 /*use the implicit register space 0*/,
				  D3D12_DESCRIPTOR_RANGE_TYPE_UAV /* UAV representing the output buffer*/,
				  0 /*heap slot where the UAV is defined*/},
			 
				{0 /*t0*/, 1, 0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV /*Top-level acceleration structure*/, 1},
			
				{0 /*b0*/, 1, 0, D3D12_DESCRIPTOR_RANGE_TYPE_CBV /*Camera parameters*/, 2} 
			}
		);

		return rsc.Generate(m_pDeviceRef.Get(), true);
	}

	ComPtr<ID3D12RootSignature> RayTraceHelpers::CreateMissSignature()
	{
		NvidiaHelpers::RootSignatureGenerator rsc;
		return rsc.Generate(m_pDeviceRef.Get(), true);
	}

	ComPtr<ID3D12RootSignature> RayTraceHelpers::CreateHitSignature()
	{
		NvidiaHelpers::RootSignatureGenerator rsc;
		rsc.AddRootParameter(D3D12_ROOT_PARAMETER_TYPE_SRV, 0 /*t0*/); // vertices and colors
		rsc.AddRootParameter(D3D12_ROOT_PARAMETER_TYPE_SRV, 1 /*t1*/); // indices
		return rsc.Generate(m_pDeviceRef.Get(), true);
	}

	void RayTraceHelpers::LoadDemoAssets()
	{
		/*m_pSphere = new ieD3D12SphereRenderer();
		m_pSphere->Init(10, 20, 20);*/

	}

}