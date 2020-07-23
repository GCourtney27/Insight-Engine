#include <ie_pch.h>

#include "D3D12_Vertex_Buffer.h"

#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"

namespace Insight {


	D3D12VertexBuffer::D3D12VertexBuffer(Verticies Verticies)
		: ieVertexBuffer(Verticies) 
	{
		m_NumVerticies = static_cast<uint32_t>(Verticies.size());
		m_BufferSize = m_NumVerticies * sizeof(Vertex3D);
		CreateResources();
	}

	void D3D12VertexBuffer::Destroy()
	{
		COM_SAFE_RELEASE(m_pVertexBuffer);
		COM_SAFE_RELEASE(m_pVertexBufferUploadHeap);
	}

	bool D3D12VertexBuffer::CreateResources()
	{
		HRESULT hr;
		Direct3D12Context* D3D12Context = reinterpret_cast<Direct3D12Context*>(&Renderer::Get());

		hr = D3D12Context->GetDeviceContext().CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(m_BufferSize),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&m_pVertexBuffer));
		if (FAILED(hr)) {
			IE_CORE_ERROR("Failed to upload vertex buffer resource heap");
			return false;
		}
		m_pVertexBuffer->SetName(L"Vertex Buffer Resource Heap");

		hr = D3D12Context->GetDeviceContext().CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(m_BufferSize),
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
		vertexData.RowPitch = m_BufferSize;
		vertexData.SlicePitch = m_BufferSize;

		// TODO: This will fail in multithread mode because UpdateSubresources
		// modifies the command list and the command list is used elsewhere during this time
		// recording draw commands still, initializing other assets etc., causing a corruption.
		UpdateSubresources(&D3D12Context->GetScenePassCommandList(), m_pVertexBuffer, m_pVertexBufferUploadHeap, 0, 0, 1, &vertexData);

		m_VertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.StrideInBytes = sizeof(Vertex3D);
		m_VertexBufferView.SizeInBytes = m_BufferSize;

		D3D12Context->GetScenePassCommandList().ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

		return true;
	}

}
