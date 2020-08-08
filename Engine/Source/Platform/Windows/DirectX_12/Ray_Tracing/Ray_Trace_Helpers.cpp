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
		m_pRayTracePass_CommandList = pRTCommandList;
		m_pRendererContext = pRendererContext;
		
		m_WindowWidth = WindowDimensions.first;
		m_WindowHeight = WindowDimensions.second;

		LoadDemoAssets();

		CreateAccelerationStructures();
		CreateRaytracingPipeline();
		CreateRaytracingOutputBuffer();
		CreateCameraBuffer();
		CreateShaderResourceHeap();
		CreateShaderBindingTable();
		return true;
	}

	void RayTraceHelpers::OnPostInit()
	{
		return;
		m_BottomLevelAS = m_TempBottomLevelBuffers.pResult;

		m_TempBottomLevelBuffers.pScratch->Release();
		m_TempBottomLevelBuffers.pResult->Release();
	}

	void RayTraceHelpers::OnDestroy()
	{
		//delete m_Sphere;
	}

	void RayTraceHelpers::UpdateCBVs()
	{
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
		m_pRayTracePass_CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		m_pRayTracePass_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_OutputBuffer_UAV.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

		//m_pRayTracePass_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_OutputBuffer_UAV.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_SOURCE));
		//m_pRayTracePass_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_OutputBuffer_UAV.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

		DispatchRaysDesc = {};
		uint32_t RayGenerationSectionSizeInBytes = m_sbtHelper.GetRayGenSectionSize();
		DispatchRaysDesc.RayGenerationShaderRecord.StartAddress = m_sbtStorage->GetGPUVirtualAddress();
		DispatchRaysDesc.RayGenerationShaderRecord.SizeInBytes = RayGenerationSectionSizeInBytes;

		uint32_t MissSectionSizeInBytes = m_sbtHelper.GetMissSectionSize();
		DispatchRaysDesc.MissShaderTable.StartAddress = m_sbtStorage->GetGPUVirtualAddress() + RayGenerationSectionSizeInBytes;
		DispatchRaysDesc.MissShaderTable.SizeInBytes = MissSectionSizeInBytes;
		DispatchRaysDesc.MissShaderTable.StrideInBytes = m_sbtHelper.GetMissEntrySize();

		uint32_t HitGroupsSectionSize = m_sbtHelper.GetHitGroupSectionSize();
		DispatchRaysDesc.HitGroupTable.StartAddress = m_sbtStorage->GetGPUVirtualAddress() + RayGenerationSectionSizeInBytes + MissSectionSizeInBytes;
		DispatchRaysDesc.HitGroupTable.SizeInBytes = HitGroupsSectionSize;
		DispatchRaysDesc.HitGroupTable.StrideInBytes = m_sbtHelper.GetHitGroupEntrySize();

		DispatchRaysDesc.Width = m_WindowWidth;
		DispatchRaysDesc.Height = m_WindowHeight;
		DispatchRaysDesc.Depth = 1;

		m_pRayTracePass_CommandList->SetPipelineState1(m_rtStateObject.Get());
	}

	void RayTraceHelpers::TraceScene()
	{
		m_pRayTracePass_CommandList->DispatchRays(&DispatchRaysDesc);

		m_pRayTracePass_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_OutputBuffer_UAV.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE));
		
		
		//m_pRayTracePass_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_OutputBuffer_UAV.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST));
		//m_pRayTracePass_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_OutputBuffer_UAV.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COPY_SOURCE));
		//m_pRayTracePass_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_OutputBuffer_UAV.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
		
	}

	RayTraceHelpers::AccelerationStructureBuffers RayTraceHelpers::CreateBottomLevelAS(std::vector<std::pair<ComPtr<ID3D12Resource>, uint32_t>> vVertexBuffers, std::vector<std::pair<ComPtr<ID3D12Resource>, uint32_t>> vIndexBuffers)
	{
		nv_helpers_dx12::BottomLevelASGenerator BottomLevelAS;

		for (size_t i = 0; i < vVertexBuffers.size(); i++) {

			if (i < vIndexBuffers.size() && vIndexBuffers[i].second > 0)
			{
				BottomLevelAS.AddVertexBuffer(vVertexBuffers[i].first.Get(), 0,
					vVertexBuffers[i].second, sizeof(SimpleVertex3D),
					vIndexBuffers[i].first.Get(), 0,
					vIndexBuffers[i].second, nullptr, 0, true);
			}
			else
			{
				BottomLevelAS.AddVertexBuffer(vVertexBuffers[i].first.Get(), 0, vVertexBuffers[i].second, sizeof(SimpleVertex3D), 0, 0);
			}
		}

		UINT64 scratchSizeInBytes = 0;
		UINT64 resultSizeInBytes = 0;

		BottomLevelAS.ComputeASBufferSizes(m_pDeviceRef.Get(), false, &scratchSizeInBytes, &resultSizeInBytes);

		AccelerationStructureBuffers buffers;
		buffers.pScratch = nv_helpers_dx12::CreateBuffer(
			m_pDeviceRef.Get(), scratchSizeInBytes,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON,
			nv_helpers_dx12::kDefaultHeapProps);
		buffers.pResult = nv_helpers_dx12::CreateBuffer(
			m_pDeviceRef.Get(), resultSizeInBytes,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
			nv_helpers_dx12::kDefaultHeapProps);

		BottomLevelAS.Generate(m_pRayTracePass_CommandList.Get(), buffers.pScratch.Get(), buffers.pResult.Get(), false, nullptr);

		return buffers;
	}

	void RayTraceHelpers::CreateTopLevelAS(const std::vector<std::pair<ComPtr<ID3D12Resource>, DirectX::XMMATRIX>>& instances)
	{
		for (size_t i = 0; i < instances.size(); i++) {
			m_TopLevelASGenerator.AddInstance(instances[i].first.Get(), instances[i].second, static_cast<UINT>(i), static_cast<UINT>(0));
		}

		UINT64 scratchSize, resultSize, instanceDescsSize;
		m_TopLevelASGenerator.ComputeASBufferSizes(m_pDeviceRef.Get(), true, &scratchSize, &resultSize, &instanceDescsSize);

		m_TopLevelASBuffers.pScratch = nv_helpers_dx12::CreateBuffer(m_pDeviceRef.Get(),
			resultSize,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
			nv_helpers_dx12::kDefaultHeapProps);
		m_TopLevelASBuffers.pResult = nv_helpers_dx12::CreateBuffer(m_pDeviceRef.Get(),
			resultSize,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
			nv_helpers_dx12::kDefaultHeapProps);

		m_TopLevelASBuffers.pInstanceDesc = nv_helpers_dx12::CreateBuffer(m_pDeviceRef.Get(),
			instanceDescsSize,
			D3D12_RESOURCE_FLAG_NONE,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nv_helpers_dx12::kUploadHeapProps);

		m_TopLevelASGenerator.Generate(m_pRayTracePass_CommandList.Get(),
			m_TopLevelASBuffers.pScratch.Get(),
			m_TopLevelASBuffers.pResult.Get(),
			m_TopLevelASBuffers.pInstanceDesc.Get());
	}

	void RayTraceHelpers::CreateAccelerationStructures()
	{
		// Build the bottom AS from the Triangle vertex buffer
		AccelerationStructureBuffers bottomLevelBuffers = CreateBottomLevelAS({ {pVertexBuffer.Get(), numCubeVerticies} }, { {pIndexBuffer.Get(), numCubeIndices} });

		m_Instances = { {bottomLevelBuffers.pResult, XMMatrixIdentity()} };
		CreateTopLevelAS(m_Instances);

		m_BottomLevelAS = bottomLevelBuffers.pResult;
	}

	void RayTraceHelpers::CreateCameraBuffer() {
		uint32_t nbMatrix = 4; // view, perspective, viewInv, perspectiveInv
		m_CameraBufferSize = nbMatrix * sizeof(XMMATRIX);

		// Create the constant buffer for all matrices
		m_CameraBuffer = nv_helpers_dx12::CreateBuffer(
			m_pDeviceRef.Get(), m_CameraBufferSize, D3D12_RESOURCE_FLAG_NONE,
			D3D12_RESOURCE_STATE_GENERIC_READ, nv_helpers_dx12::kUploadHeapProps);

		// Create a descriptor heap that will be used by the rasterization shaders
		m_ConstHeap = nv_helpers_dx12::CreateDescriptorHeap(
			m_pDeviceRef.Get(), 1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);

		// Describe and create the constant buffer view.
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = m_CameraBuffer->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = m_CameraBufferSize;

		// Get a handle to the heap memory on the CPU side, to be able to write the
		// descriptors directly
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle =
			m_ConstHeap->GetCPUDescriptorHandleForHeapStart();
		m_pDeviceRef->CreateConstantBufferView(&cbvDesc, srvHandle);
	}

	void RayTraceHelpers::CreateRaytracingPipeline()
	{
		nv_helpers_dx12::RayTracingPipelineGenerator Pipeline(reinterpret_cast<ID3D12Device5*>(m_pDeviceRef.Get()));
		
#ifndef IE_IS_STANDALONE
		LPCWSTR RayGenShaderFolder = L"Source/Shaders/HLSL/Ray_Tracing/RayGen.hlsl";
		LPCWSTR MissShaderFolder = L"Source/Shaders/HLSL/Ray_Tracing/Miss.hlsl";
		LPCWSTR HitShaderFolder = L"Source/Shaders/HLSL/Ray_Tracing/Hit.hlsl";
#else
		LPCWSTR RayGenShaderFolder = L"RayGen.hlsl";
		LPCWSTR MissShaderFolder = L"Miss.hlsl";
		LPCWSTR HitShaderFolder = L"Hit.hlsl";
#endif
		// TODO Change the shader paths to be path independent!
		m_RayGenLibrary = nv_helpers_dx12::CompileShaderLibrary(RayGenShaderFolder);
		m_MissLibrary = nv_helpers_dx12::CompileShaderLibrary(MissShaderFolder);
		m_HitLibrary = nv_helpers_dx12::CompileShaderLibrary(HitShaderFolder);

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
			&nv_helpers_dx12::kDefaultHeapProps, D3D12_HEAP_FLAG_NONE, &resDesc,
			D3D12_RESOURCE_STATE_COPY_SOURCE, nullptr,
			IID_PPV_ARGS(&m_OutputBuffer_UAV)), "Failed to create raytracing outptut buffer.");

		/*CDescriptorHeapWrapper& cbvsrvHeap = reinterpret_cast<Direct3D12Context*>(&Renderer::Get())->GetCBVSRVDescriptorHeap();

		D3D12_RESOURCE_DESC ResourceDesc = {};
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		ResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		ResourceDesc.Width = static_cast<UINT64>(m_WindowWidth);
		ResourceDesc.Height = static_cast<UINT64>(m_WindowHeight);
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.SampleDesc.Count = 1;
		ThrowIfFailed(m_pDeviceRef->CreateCommittedResource(
			&nv_helpers_dx12::kDefaultHeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc,
			D3D12_RESOURCE_STATE_COPY_SOURCE, nullptr,
			IID_PPV_ARGS(&m_OutputBuffer_UAV)
		), "Failed to create commited resource for Raytracing output buffer.");

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		m_pDeviceRef->CreateUnorderedAccessView(m_OutputBuffer_UAV.Get(), nullptr, &uavDesc, cbvsrvHeap.hCPU(6));
		m_OutputBuffer_UAV->SetName(L"Raytracing UAV output buffer");

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.RaytracingAccelerationStructure.Location = m_TopLevelASBuffers.pResult->GetGPUVirtualAddress();
		m_pDeviceRef->CreateShaderResourceView(nullptr, &srvDesc, cbvsrvHeap.hCPU(7));*/
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

		// Adding the triangle hit shader
		m_sbtHelper.AddHitGroup(L"HitGroup", { (void*)(pVertexBuffer->GetGPUVirtualAddress()),
											   (void*)(pIndexBuffer->GetGPUVirtualAddress()) });

		uint32_t sbtSize = m_sbtHelper.ComputeSBTSize();

		m_sbtStorage = nv_helpers_dx12::CreateBuffer(
			m_pDeviceRef.Get(), 
			sbtSize, 
			D3D12_RESOURCE_FLAG_NONE,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nv_helpers_dx12::kUploadHeapProps
		);
		if (!m_sbtStorage) {
			throw std::logic_error("Could not allocate the shader binding table");
		}

		m_sbtHelper.Generate(m_sbtStorage.Get(), m_rtStateObjectProps.Get());
	}

	void RayTraceHelpers::CreateShaderResourceHeap()
	{
		m_srvUavHeap = nv_helpers_dx12::CreateDescriptorHeap(m_pDeviceRef.Get(), 3, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);

		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = m_srvUavHeap->GetCPUDescriptorHandleForHeapStart();

		// Output Buffer
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		m_pDeviceRef->CreateUnorderedAccessView(m_OutputBuffer_UAV.Get(), nullptr, &uavDesc, srvHandle);

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
		nv_helpers_dx12::RootSignatureGenerator rsc;

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
		nv_helpers_dx12::RootSignatureGenerator rsc;
		return rsc.Generate(m_pDeviceRef.Get(), true);
	}

	ComPtr<ID3D12RootSignature> RayTraceHelpers::CreateHitSignature()
	{
		nv_helpers_dx12::RootSignatureGenerator rsc;
		rsc.AddRootParameter(D3D12_ROOT_PARAMETER_TYPE_SRV, 0 /*t0*/); // vertices and colors
		rsc.AddRootParameter(D3D12_ROOT_PARAMETER_TYPE_SRV, 1 /*t1*/); // indices
		return rsc.Generate(m_pDeviceRef.Get(), true);
	}

	void RayTraceHelpers::LoadDemoAssets()
	{
		/*m_Sphere = new ieD3D12SphereRenderer();
		m_Sphere->Init(10, 20, 20);*/
		// Create vertex buffer

	// A triangle
	// a triangle
		SimpleVertex3D vList[] = {
			// front face
			{ -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f },
			{  0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f },
			{ -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f },
			{  0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f },

			// right side face
			{  0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f },
			{  0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f },
			{  0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f },
			{  0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f },

			// left side face					    
			{ -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f },
			{ -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f },
			{ -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f },

			// back face						    
			{  0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f },
			{ -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f },
			{  0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f },
			{ -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f },

			// top face
			{ -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f },
			{ 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f },
			{ 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f },
			{ -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f },

			// bottom face
			{  0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f },
			{ -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f },
			{  0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f },
			{ -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f },
		};
		int vBufferSize = sizeof(vList);
		numCubeVerticies = vBufferSize / sizeof(SimpleVertex3D);

		// Create default heap
		// Default heap is memeory on the GPU. Only the GPU has access to this memory
		// To get data into the heap, we will have to upload the data using
		// an upload heap
		m_pDeviceRef->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // A default heap
			D3D12_HEAP_FLAG_NONE, // No flags
			&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // Resource description for a buffer
			D3D12_RESOURCE_STATE_COPY_DEST, // We will start this heap in the copy destination state since we will copy
											// will copy data from the upload hea to this heap
			nullptr, // Optomized clear value must be null for this type of resource. Used for render targets and depth/stencil buffers
			IID_PPV_ARGS(&pVertexBuffer)
		);

		// We can give resource heaps a name so we debug with the graphics debugger we know what resource we are looking at
		pVertexBuffer->SetName(L"Vertex Buffer Resource Heap");

		// Create upload heap
		// Upload heaps are used to upload data to the GPU. CPU can write to it, GPU can read from it
		// We will upload the vertex buffer using this heap to the deafault heap
		ID3D12Resource* vBufferUploadHeap;
		m_pDeviceRef->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // Upload heap
			D3D12_HEAP_FLAG_NONE, // No Flags
			&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // Resource description for a buffer
			D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read form this buffer and copy it's contents to the default heap
			nullptr,
			IID_PPV_ARGS(&vBufferUploadHeap)
		);
		vBufferUploadHeap->SetName(L"Vetex Buffer Upload Resource Heap");

		// Store Vertes buffer in upload heap
		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = reinterpret_cast<BYTE*>(vList); // Pointer to our vertex array
		vertexData.RowPitch = vBufferSize; // Soze of our triangle vertex data
		vertexData.SlicePitch = vBufferSize; // Also the size of our triangle vertex data

		// We are now creating a command with the command list to copu the data from
		// the upload heap to the default heap
		UpdateSubresources(m_pRayTracePass_CommandList.Get(), pVertexBuffer.Get(), vBufferUploadHeap, 0, 0, 1, &vertexData);

		// Transition the vertex buffer data from copy destination state to verte buffer state
		m_pRayTracePass_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pVertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

		// Create Index Buffer

		// A quad (2 triangles)
		DWORD iList[] = {
			// front face
			0, 1, 2, // first triangle
			0, 3, 1, // second triangle

			// left face
			4, 5, 6, // first triangle
			4, 7, 5, // second triangle

			// right face
			8, 9, 10, // first triangle
			8, 11, 9, // second triangle

			// back face
			12, 13, 14, // first triangle
			12, 15, 13, // second triangle

			// top face
			16, 17, 18, // first triangle
			16, 19, 17, // second triangle

			// bottom face
			20, 21, 22, // first triangle
			20, 23, 21, // second triangle
		};

		int iBufferSize = sizeof(iList);

		numCubeIndices = sizeof(iList) / sizeof(DWORD);

		// Create default heap to hold index buffer
		m_pDeviceRef->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // a default heap
			D3D12_HEAP_FLAG_NONE, // no flags
			&CD3DX12_RESOURCE_DESC::Buffer(iBufferSize), // resource description for a buffer
			D3D12_RESOURCE_STATE_COPY_DEST, // start in the copy destination state
			nullptr, // optimized clear value must be null for this type of resource
			IID_PPV_ARGS(&pIndexBuffer));

		// We can give resource hesaps a name so when we debug with the graphcs debugger we know what resources we are looking at
		pIndexBuffer->SetName(L"Index Buffer Resource Heap");

		// Create upload heap to upload index buffer
		ID3D12Resource* iBufferUploadHeap;
		m_pDeviceRef->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
			D3D12_HEAP_FLAG_NONE, // no flags
			&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize), // resource description for a buffer
			D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
			nullptr,
			IID_PPV_ARGS(&iBufferUploadHeap));
		iBufferUploadHeap->SetName(L"Index buffer Upload Resource Heap");

		// Store vertex buffer in upload heap
		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = reinterpret_cast<BYTE*>(iList); // pointer to our index array
		indexData.RowPitch = iBufferSize; // size of all our index buffer
		indexData.SlicePitch = iBufferSize; // also the size of our index buffer


		// We are now creating a command with the command list to copy the data from
		// the upload heal to the default
		UpdateSubresources(m_pRayTracePass_CommandList.Get(), pIndexBuffer.Get(), iBufferUploadHeap, 0, 0, 1, &indexData);

		// Transition the vertex buffer data from the copy destination state to vertex buffer state
		m_pRayTracePass_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pIndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER));
	}

}