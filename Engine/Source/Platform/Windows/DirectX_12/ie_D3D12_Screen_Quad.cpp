#include <ie_pch.h>

#include "ie_D3D12_Screen_Quad.h"

#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"

namespace Insight {

	void ieD3D12ScreenQuad::Init()
	{
		HRESULT hr;
		Direct3D12Context* GraphicsContext = reinterpret_cast<Direct3D12Context*>(&Renderer::Get());

		// Vertex Buffer
		ScreenSpaceVertex quadVerts[] =
		{
			{ { -1.0f, 1.0f, 0.0f }, { 0.0f,0.0f } }, // Top Left
			{ {  1.0f, 1.0f, 0.0f }, { 1.0f,0.0f } }, // Top Right
			{ { -1.0f,-1.0f, 0.0f }, { 0.0f,1.0f } }, // Bottom Left
			{ {  1.0f,-1.0f, 0.0f }, { 1.0f,1.0f } }, // Bottom Right
		};
		int vBufferSize = sizeof(quadVerts);
		m_NumVerticies = vBufferSize / sizeof(ScreenSpaceVertex);

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.Width = vBufferSize;
		resourceDesc.Height = 1;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		hr = GraphicsContext->GetDeviceContext().CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&m_VertexBuffer)
		);
		m_VertexBuffer->SetName(L"Screen Quad Default Resource Heap");
		ThrowIfFailed(hr, "Failed to create default heap resource for screen qauad");

		ID3D12Resource* vBufferUploadHeap;
		hr = GraphicsContext->GetDeviceContext().CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vBufferUploadHeap));
		vBufferUploadHeap->SetName(L"Screen Quad Upload Resource Heap");
		ThrowIfFailed(hr, "Failed to create upload heap resource for screen qauad");

		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = reinterpret_cast<BYTE*>(quadVerts);
		vertexData.RowPitch = vBufferSize;
		vertexData.SlicePitch = vBufferSize;

		UpdateSubresources(&GraphicsContext->GetScenePassCommandList(), m_VertexBuffer.Get(), vBufferUploadHeap, 0, 0, 1, &vertexData);

		GraphicsContext->GetScenePassCommandList().ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_VertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

		m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.StrideInBytes = sizeof(ScreenSpaceVertex);
		m_VertexBufferView.SizeInBytes = vBufferSize;

		// Index Buffer

		uint32_t QuadIndices[] =
		{
			0, 1, 3,
			0, 3, 2
		};
		uint32_t IndexBufferSize = 6 * sizeof(uint32_t);
		ComPtr<ID3D12Resource>		pIndexBufferUploadHeap;

		hr = GraphicsContext->GetDeviceContext().CreateCommittedResource(
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

		hr = GraphicsContext->GetDeviceContext().CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(IndexBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&pIndexBufferUploadHeap));
		if (FAILED(hr)) {
			IE_CORE_ERROR("Failed to create Committed Resource for Index Buffer to the Upload Heap");
		}
		pIndexBufferUploadHeap->SetName(L"Index Buffer Upload Resource Heap");

		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = reinterpret_cast<BYTE*>(QuadIndices);
		indexData.RowPitch = IndexBufferSize;
		indexData.SlicePitch = IndexBufferSize;

		UpdateSubresources(&GraphicsContext->GetScenePassCommandList(), m_pIndexBuffer.Get(), pIndexBufferUploadHeap.Get(), 0, 0, 1, &indexData);

		m_IndexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_IndexBufferView.SizeInBytes = IndexBufferSize;

		GraphicsContext->GetScenePassCommandList().ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER));
	}

	void ieD3D12ScreenQuad::OnRender(ComPtr<ID3D12GraphicsCommandList> pCommandList)
	{
		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		pCommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
		pCommandList->IASetIndexBuffer(&m_IndexBufferView);
		pCommandList->DrawInstanced(m_NumVerticies, 1, 0, 0);
	}

}
