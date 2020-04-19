#include "ie_pch.h"

#include "Mesh.h"
#include "Insight/Core/Application.h"
#include "Platform/DirectX12/Direct3D12_Context.h"

namespace Insight {


	Mesh::Mesh(std::vector<Vertex> verticies, std::vector<UINT> indices)
	{
		m_Verticies = verticies;
		m_Indices = indices;
		// m_Textures = textures
		m_NumIndices = m_Verticies.size();
		m_NumVerticies = m_Indices.size();

		m_IBufferSize = m_NumIndices * sizeof(UINT);
		m_VBufferSize = m_NumVerticies * sizeof(Vertex);

		RenderingContext* renderContext = &Application::Get().GetWindow().GetRenderContext();
		Direct3D12Context* graphicsContext = reinterpret_cast<Direct3D12Context*>(renderContext);

		m_pLogicalDevice = &graphicsContext->GetDeviceContext();
		m_pCommandList = &graphicsContext->GetCommandList();

		SetupMesh();
	}

	Mesh::~Mesh()
	{
		//Destroy();
	}

	void Mesh::Draw()
	{
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
		m_VertexBufferView.StrideInBytes = sizeof(Vertex);
		m_VertexBufferView.SizeInBytes = m_VBufferSize;

		m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

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

