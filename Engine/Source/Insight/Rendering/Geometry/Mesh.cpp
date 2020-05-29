#include <ie_pch.h>

#include "Mesh.h"

#include "Insight/Core/Application.h"
#include "Platform/DirectX12/Direct3D12_Context.h"
#include "Insight/Runtime/APlayer_Character.h"
#include "imgui.h"

namespace Insight {


	Mesh::Mesh(Verticies verticies, Indices indices, Material material)
		: m_Verticies(verticies), m_Indices(indices), m_Material(material)
	{
		InitializeLocalVariables();
	}

	Mesh::~Mesh()
	{
		Destroy();
	}

	void Mesh::Init(Verticies verticies, Indices indices)
	{
		m_Verticies = verticies;
		m_Indices = indices;
		InitializeLocalVariables();
	}

	void Mesh::InitializeLocalVariables()
	{
		m_NumIndices   = static_cast<UINT>(m_Verticies.size());
		m_NumVerticies = static_cast<UINT>(m_Indices.size());

		m_IBufferSize = m_NumIndices * sizeof(UINT);
		m_VBufferSize = m_NumVerticies * sizeof(Vertex3D);

		m_pLogicalDevice = &Direct3D12Context::Get().GetDeviceContext();
		m_pCommandList	 = &Direct3D12Context::Get().GetCommandList();

		SetupMesh();
	}

	void Mesh::PreRender(const XMMATRIX& parentMat)
	{
		m_Transform.SetWorldMatrix(XMMatrixMultiply(parentMat, m_Transform.GetLocalMatrix()));
		
		XMMATRIX worldMatTransposed = XMMatrixTranspose(m_Transform.GetLocalMatrixRef());
		XMFLOAT4X4 worldFloat;
		XMStoreFloat4x4(&worldFloat, worldMatTransposed);

		m_ConstantBufferPerObject.world = worldFloat;
	}

	CB_VS_PerObject Mesh::GetConstantBuffer()
	{
		return m_ConstantBufferPerObject;
	}

	void Mesh::Render()
	{
		//m_Material.BindResources();
		m_pCommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
		m_pCommandList->IASetIndexBuffer(&m_IndexBufferView);

		m_pCommandList->DrawIndexedInstanced(m_NumIndices, 1, 0, 0, 0);
	}

	void Mesh::Destroy()
	{
		COM_SAFE_RELEASE(m_pVertexBuffer);
		COM_SAFE_RELEASE(m_pIndexBuffer);
		COM_SAFE_RELEASE(m_pVertexBufferUploadHeap);
		COM_SAFE_RELEASE(m_pIndexBufferUploadHeap);
	}

	void Mesh::SetupMesh()
	{
		if (!InitializeVertexDataForD3D12())
			IE_CORE_TRACE("Failed to setup vertex data for D3D12");

		//if (!InitializeInstanceBufferD3D12())
		//	IE_CORE_TRACE("Failed to setup instance data for D3D12");

		if (!InitializeIndexDataForD3D12())
			IE_CORE_TRACE("Failed to setup index data for D3D12");

	}
	bool Mesh::InitializeVertexDataForD3D12()
	{
		HRESULT hr;

		hr = m_pLogicalDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(m_VBufferSize),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&m_pVertexBuffer));
		if (FAILED(hr)) {
			IE_CORE_ERROR("Failed to upload vertex buffer resource heap");
			return false;
		}
		m_pVertexBuffer->SetName(L"Vertex Buffer Resource Heap");

		hr = m_pLogicalDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(m_VBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_pVertexBufferUploadHeap));
		if (FAILED(hr)) {
			IE_CORE_ERROR("Failed to upload vertex buffer heap");
			return false;
		}
		m_pVertexBufferUploadHeap->SetName(L"Vertex Buffer Upload Resource Heap");

		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = reinterpret_cast<BYTE*>(m_Verticies.data());
		vertexData.RowPitch = m_VBufferSize;
		vertexData.SlicePitch = m_VBufferSize;

		UpdateSubresources(m_pCommandList, m_pVertexBuffer, m_pVertexBufferUploadHeap, 0, 0, 1, &vertexData);

		m_VertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.StrideInBytes = sizeof(Vertex3D);
		m_VertexBufferView.SizeInBytes = m_VBufferSize;

		m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

		return true;
	}

	bool Mesh::InitializeInstanceBufferD3D12()
	{
		// Instance Data
		XMFLOAT3 instancePositions[] =
		{
			XMFLOAT3(0.0f, 0.0f, 7.0f),
			XMFLOAT3(0.0f, 0.0f, -7.0f),
			XMFLOAT3(7.0f, 0.0f, 0.0f),
			XMFLOAT3(-7.0f, 0.0f, 0.0f),
		};

		const UINT instanceBufferSize = sizeof(instancePositions);

		// Create the instance buffer resource in the GPU's default heap and copy instance data into it using the upload heap.
		// The upload resource must not be released until after the GPU has finished using it.

		HRESULT hr = m_pLogicalDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(instanceBufferSize),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&m_pInstanceBuffer));
		if (FAILED(hr)) {
			IE_CORE_ERROR("Failed to create commited resource for instance buffer.");
		}
		hr = m_pInstanceBuffer->SetName(L"Instance Buffer");
		if (FAILED(hr)) {
			IE_CORE_ERROR("Failed to set GPU name for instance buffer.");
		}
		hr = m_pLogicalDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(instanceBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_pInstanceBufferUploadHeap));
		if (FAILED(hr)) {
			IE_CORE_ERROR("Failed to create upload commited resource for instance buffer.");
		}
		hr = m_pInstanceBufferUploadHeap->SetName(L"Instance Buffer");
		if (FAILED(hr)) {
			IE_CORE_ERROR("Failed to set GPU name for instance buffer.");
		}
		// Upload the instance buffer to the GPU.
		D3D12_SUBRESOURCE_DATA instanceData = {};
		instanceData.pData = reinterpret_cast<BYTE*>(instancePositions);
		instanceData.RowPitch = instanceBufferSize;
		instanceData.SlicePitch = instanceBufferSize;

		UpdateSubresources(m_pCommandList, m_pInstanceBuffer, m_pInstanceBufferUploadHeap, 0, 0, 1, &instanceData);

		// Create vertex/index buffer views.
		m_InstanceBufferView.BufferLocation = m_pInstanceBuffer->GetGPUVirtualAddress();
		m_InstanceBufferView.StrideInBytes = sizeof(XMFLOAT3);
		m_InstanceBufferView.SizeInBytes = instanceBufferSize;

		m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pInstanceBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));


		return true;
	}

	bool Mesh::InitializeIndexDataForD3D12()
	{
		HRESULT hr;

		hr = m_pLogicalDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(m_IBufferSize),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&m_pIndexBuffer));
		if (FAILED(hr)) {
			IE_CORE_ERROR("Failed to create Committed Resource for Index Buffer to the Deafault Heap");
			return false;
		}
		m_pIndexBuffer->SetName(L"Index Buffer Resource Heap");

		hr = m_pLogicalDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(m_IBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_pIndexBufferUploadHeap));
		if (FAILED(hr)) {
			IE_CORE_ERROR("Failed to create Committed Resource for Index Buffer to the Upload Heap");
			return false;
		}
		m_pIndexBufferUploadHeap->SetName(L"Index Buffer Upload Resource Heap");

		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = reinterpret_cast<BYTE*>(m_Indices.data());
		indexData.RowPitch = m_IBufferSize;
		indexData.SlicePitch = m_IBufferSize;

		UpdateSubresources(m_pCommandList, m_pIndexBuffer, m_pIndexBufferUploadHeap, 0, 0, 1, &indexData);

		m_IndexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_IndexBufferView.SizeInBytes = m_IBufferSize;

		m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER));

		return true;
	}
}

