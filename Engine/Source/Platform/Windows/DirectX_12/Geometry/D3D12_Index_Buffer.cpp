#include <ie_pch.h>

#include "D3D12_Index_Buffer.h"

#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"

namespace Insight {



	D3D12IndexBuffer::D3D12IndexBuffer(Indices Indices)
		: ieIndexBuffer(Indices)
	{
		m_NumIndices = static_cast<uint32_t>(Indices.size());
		m_BufferSize = m_NumIndices * sizeof(uint32_t);
		CreateResources();
	}

	void D3D12IndexBuffer::Destroy()
	{
		COM_SAFE_RELEASE(m_pIndexBuffer);
		COM_SAFE_RELEASE(m_pIndexBufferUploadHeap);
	}

	bool D3D12IndexBuffer::CreateResources()
	{
		HRESULT hr;
		Direct3D12Context* D3D12Context = reinterpret_cast<Direct3D12Context*>(&Renderer::Get());

		hr = D3D12Context->GetDeviceContext().CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(m_BufferSize),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&m_pIndexBuffer));
		if (FAILED(hr)) {
			IE_CORE_ERROR("Failed to create Committed Resource for Index Buffer to the Deafault Heap");
			return false;
		}
		m_pIndexBuffer->SetName(L"Index Buffer Resource Heap");

		hr = D3D12Context->GetDeviceContext().CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(m_BufferSize),
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
		indexData.RowPitch = m_BufferSize;
		indexData.SlicePitch = m_BufferSize;

		Direct3D12Context* graphicsContext = reinterpret_cast<Direct3D12Context*>(&Renderer::Get());
		UpdateSubresources(&graphicsContext->GetScenePassCommandList(), m_pIndexBuffer.Get(), m_pIndexBufferUploadHeap.Get(), 0, 0, 1, &indexData);

		m_IndexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_IndexBufferView.SizeInBytes = m_BufferSize;

		D3D12Context->GetScenePassCommandList().ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER));

		return true;
	}

}
