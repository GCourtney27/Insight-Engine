#include <Renderer_pch.h>

#include "D3D12_Screen_Quad.h"

#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"

namespace Insight {

	void D3D12ScreenQuad::Init(ScreenSpaceVertex Verticies[], uint32_t VertexBufferSize, uint32_t Indices[], uint32_t IndexBufferSize)
	{
		HRESULT hr;
		Direct3D12Context& RenderContext = Renderer::GetAs<Direct3D12Context>();

		// Vertex Buffer
		// -------------
		m_NumVerticies = VertexBufferSize / sizeof(ScreenSpaceVertex);

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.Width = VertexBufferSize;
		resourceDesc.Height = 1;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		hr = RenderContext.GetDeviceContext().CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&m_pVertexBuffer)
		);
		m_pVertexBuffer->SetName(L"Screen Quad Default Resource Heap");
		ThrowIfFailed(hr, "Failed to create default heap resource for screen qauad");

		hr = RenderContext.GetDeviceContext().CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(VertexBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_pVertexBufferUploadHeap));
		m_pVertexBufferUploadHeap->SetName(L"Screen Quad Upload Resource Heap");
		ThrowIfFailed(hr, "Failed to create upload heap resource for screen qauad");

		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = reinterpret_cast<BYTE*>(Verticies);
		vertexData.RowPitch = VertexBufferSize;
		vertexData.SlicePitch = VertexBufferSize;

		UpdateSubresources(&RenderContext.GetScenePassCommandList(), m_pVertexBuffer.Get(), m_pVertexBufferUploadHeap.Get(), 0, 0, 1, &vertexData);

		RenderContext.GetScenePassCommandList().ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

		m_VertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.StrideInBytes = sizeof(ScreenSpaceVertex);
		m_VertexBufferView.SizeInBytes = VertexBufferSize;

		// Index Buffer
		// ------------

		hr = RenderContext.GetDeviceContext().CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(IndexBufferSize),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&m_pIndexBuffer));
		if (FAILED(hr)) {
			IE_CORE_ERROR("Failed to create Committed Resource for Index Buffer to the Deafault Heap");
		}
		m_pIndexBuffer->SetName(L"Index Buffer Resource Heap");

		hr = RenderContext.GetDeviceContext().CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(IndexBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_pIndexBufferUploadHeap));
		if (FAILED(hr)) {
			IE_CORE_ERROR("Failed to create Committed Resource for Index Buffer to the Upload Heap");
		}
		m_pIndexBufferUploadHeap->SetName(L"Index Buffer Upload Resource Heap");

		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = reinterpret_cast<BYTE*>(Indices);
		indexData.RowPitch = IndexBufferSize;
		indexData.SlicePitch = IndexBufferSize;

		UpdateSubresources(&RenderContext.GetScenePassCommandList(), m_pIndexBuffer.Get(), m_pIndexBufferUploadHeap.Get(), 0, 0, 1, &indexData);

		m_IndexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_IndexBufferView.SizeInBytes = IndexBufferSize;

		RenderContext.GetScenePassCommandList().ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER));
	}

	void D3D12ScreenQuad::OnRender(ComPtr<ID3D12GraphicsCommandList> pCommandList)
	{
		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		pCommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
		pCommandList->IASetIndexBuffer(&m_IndexBufferView);
		pCommandList->DrawInstanced(m_NumVerticies, 1, 0, 0);
	}

}
