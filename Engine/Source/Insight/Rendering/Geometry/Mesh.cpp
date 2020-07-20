#include <ie_pch.h>

#include "Mesh.h"

#include "Insight/Core/Application.h"
#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"
#include "Insight/Runtime/APlayer_Character.h"
#include "imgui.h"

namespace Insight {


	Mesh::Mesh(Verticies verticies, Indices indices)
		: m_Verticies(std::move(verticies)), m_Indices(std::move(indices))
	{
		SetupMesh();
	}

	Mesh::Mesh(Mesh&& mesh) noexcept
	{
		IE_CORE_WARN("Mesh moved in memory");

		m_pVertexBufferUploadHeap = mesh.m_pVertexBufferUploadHeap;
		m_pVertexBuffer = mesh.m_pVertexBuffer;
		m_VertexBufferView = mesh.m_VertexBufferView;

		m_pIndexBufferUploadHeap = mesh.m_pIndexBufferUploadHeap;
		m_pIndexBuffer = mesh.m_pIndexBuffer;
		m_IndexBufferView = mesh.m_IndexBufferView;

		m_NumIndices = mesh.m_NumVerticies;
		m_NumIndices = mesh.m_NumIndices;
		m_VBufferSize = mesh.m_VBufferSize;
		m_IBufferSize = mesh.m_IBufferSize;

		m_Verticies = std::move(mesh.m_Verticies);
		m_Indices = std::move(mesh.m_Indices);

		m_Transform = mesh.m_Transform;
		m_ConstantBufferPerObject = mesh.m_ConstantBufferPerObject;

		mesh.m_pVertexBufferUploadHeap = nullptr;
		mesh.m_pVertexBuffer = nullptr;
		mesh.m_VertexBufferView = {};

		mesh.m_pIndexBufferUploadHeap = nullptr;
		mesh.m_pIndexBuffer = nullptr;
		mesh.m_IndexBufferView = {};

		mesh.m_NumIndices = 0;
		mesh.m_NumIndices = 0;
		mesh.m_VBufferSize = 0;
		mesh.m_IBufferSize = 0;

		mesh.m_ConstantBufferPerObject = {};
	}

	Mesh::~Mesh()
	{
		Destroy();
	}

	void Mesh::Destroy()
	{
		COM_SAFE_RELEASE(m_pIndexBuffer);
		COM_SAFE_RELEASE(m_pVertexBuffer);
		COM_SAFE_RELEASE(m_pVertexBufferUploadHeap);
		COM_SAFE_RELEASE(m_pIndexBufferUploadHeap);

		m_pDeviceContext = nullptr;
	}

	void Mesh::Init(Verticies verticies, Indices indices)
	{
		m_Verticies = std::move(verticies);
		m_Indices = std::move(indices);
		SetupMesh();
	}

	void Mesh::InitializeVariables()
	{
		m_NumIndices = static_cast<uint32_t>(m_Verticies.size());
		m_NumVerticies = static_cast<uint32_t>(m_Indices.size());

		m_IBufferSize = m_NumIndices * sizeof(uint32_t);
		m_VBufferSize = m_NumVerticies * sizeof(Vertex3D);

		m_pDeviceContext = &Direct3D12Context::Get().GetDeviceContext();
	}

	void Mesh::PreRender(const XMMATRIX& parentMat)
	{
		m_Transform.SetWorldMatrix(XMMatrixMultiply(parentMat, m_Transform.GetLocalMatrix()));

		XMMATRIX worldMatTransposed = XMMatrixTranspose(m_Transform.GetWorldMatrixRef());
		XMFLOAT4X4 worldFloat;
		XMStoreFloat4x4(&worldFloat, worldMatTransposed);

		m_ConstantBufferPerObject.world = worldFloat;
	}

	CB_VS_PerObject Mesh::GetConstantBuffer()
	{
		return m_ConstantBufferPerObject;
	}

	void Mesh::Render(ID3D12GraphicsCommandList*& pCommandList)
	{
		pCommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
		pCommandList->IASetIndexBuffer(&m_IndexBufferView);
		pCommandList->DrawIndexedInstanced(m_NumIndices, 1, 0, 0, 0);
	}

	void Mesh::OnImGuiRender()
	{
		m_Material.OnImGuiRender();
	}

	void Mesh::SetupMesh()
	{
		InitializeVariables();

		if (!InitializeVertexDataForD3D12()) {
			IE_CORE_TRACE("Failed to setup vertex data for D3D12");
			Destroy();
			return;
		}
		if (!InitializeIndexDataForD3D12()) {
			IE_CORE_TRACE("Failed to setup index data for D3D12");
			Destroy();
			return;
		}
	}
	bool Mesh::InitializeVertexDataForD3D12()
	{
		HRESULT hr;

		hr = m_pDeviceContext->CreateCommittedResource(
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

		hr = m_pDeviceContext->CreateCommittedResource(
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

		// TODO: This will fail in multithread mode because UpdateSubresources
		// modifies the command list and the command list is used elsewhere during this time
		// recording draw commands still, initializing other assets etc., causing a corruption.
		UpdateSubresources(&Direct3D12Context::Get().GetScenePassCommandList(), m_pVertexBuffer, m_pVertexBufferUploadHeap, 0, 0, 1, &vertexData);

		m_VertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.StrideInBytes = sizeof(Vertex3D);
		m_VertexBufferView.SizeInBytes = m_VBufferSize;

		Direct3D12Context::Get().GetScenePassCommandList().ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

		return true;
	}

	bool Mesh::InitializeIndexDataForD3D12()
	{
		HRESULT hr;

		hr = m_pDeviceContext->CreateCommittedResource(
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

		hr = m_pDeviceContext->CreateCommittedResource(
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

		UpdateSubresources(&Direct3D12Context::Get().GetScenePassCommandList(), m_pIndexBuffer, m_pIndexBufferUploadHeap, 0, 0, 1, &indexData);

		m_IndexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_IndexBufferView.SizeInBytes = m_IBufferSize;

		Direct3D12Context::Get().GetScenePassCommandList().ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER));

		return true;
	}
}

