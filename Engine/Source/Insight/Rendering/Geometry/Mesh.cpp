#include <ie_pch.h>

#include "Mesh.h"

#include "Insight/Core/Application.h"
#include "Insight/Rendering/Renderer.h"

#include "Platform/Windows/DirectX_11/Geometry/D3D11_Index_Buffer.h"
#include "Platform/Windows/DirectX_11/Geometry/D3D11_Vertex_Buffer.h"
#include "Platform/Windows/DirectX_12/Geometry/D3D12_Index_Buffer.h"
#include "Platform/Windows/DirectX_12/Geometry/D3D12_Vertex_Buffer.h"
#include "Platform/Windows/DirectX_12/Direct3D12_Context.h"

#include "imgui.h"

namespace Insight {


	Mesh::Mesh(Verticies Verticies, Indices Indices)
	{
		Init(Verticies, Indices);
	}

	Mesh::~Mesh()
	{
		//Destroy();
	}

	void Mesh::Destroy()
	{
		delete m_pVertexBuffer;
		delete m_pIndexBuffer;
	}

	void Mesh::Init(Verticies& Verticies, Indices& Indices)
	{
		CreateBuffers(Verticies, Indices);
	}

	void Mesh::PreRender(const XMMATRIX& parentMat)
	{
		m_Transform.SetWorldMatrix(XMMatrixMultiply(parentMat, m_Transform.GetLocalMatrix()));

		XMMATRIX worldMatTransposed = XMMatrixTranspose(m_Transform.GetWorldMatrixRef());
		XMFLOAT4X4 worldFloat;
		XMStoreFloat4x4(&worldFloat, worldMatTransposed);

		m_ConstantBufferPerObject.world = worldFloat;

		// TODO check if d3d12 is active
		reinterpret_cast<Direct3D12Context*>(&Renderer::Get())->UpdateRTAccelerationStructureMatrix(m_RTInstanceIndex, m_Transform.GetWorldMatrix());
	}

	uint32_t Mesh::GetVertexCount()
	{
		return m_pVertexBuffer->GetNumVerticies();
	}

	uint32_t Mesh::GetVertexBufferSize()
	{
		return m_pVertexBuffer->GetBufferSize();
	}

	uint32_t Mesh::GetIndexCount()
	{
		return m_pIndexBuffer->GetBufferSize();
	}

	uint32_t Mesh::GetIndexBufferSize()
	{
		return m_pIndexBuffer->GetBufferSize();
	}

	void Mesh::Render()
	{
		Renderer::SetVertexBuffers(0, 1, m_pVertexBuffer);
		Renderer::SetIndexBuffer(m_pIndexBuffer);
		Renderer::DrawIndexedInstanced(m_pIndexBuffer->GetNumIndices(), 1, 0, 0, 0);
	}

	void Mesh::OnImGuiRender()
	{
	}

	void Mesh::CreateBuffers(Verticies& Verticies, Indices& Indices)
	{
		switch (Renderer::GetAPI()) {
		case Renderer::eTargetRenderAPI::D3D_11:
		{
			m_pVertexBuffer = new D3D11VertexBuffer(Verticies);
			m_pIndexBuffer = new D3D11IndexBuffer(Indices);
			break;
		}
		case Renderer::eTargetRenderAPI::D3D_12:
		{
			m_pVertexBuffer = new D3D12VertexBuffer(Verticies);
			m_pIndexBuffer = new D3D12IndexBuffer(Indices);

			// TODO Check if ray tracing is available first before we register the buffers
			m_RTInstanceIndex  = reinterpret_cast<Direct3D12Context*>(&Renderer::Get())
				->RegisterGeometryWithRTAccelerationStucture(
				reinterpret_cast<D3D12VertexBuffer*>(m_pVertexBuffer)->GetVertexBuffer(),
				reinterpret_cast<D3D12IndexBuffer*>(m_pIndexBuffer)->GetIndexBuffer(),
				reinterpret_cast<D3D12VertexBuffer*>(m_pVertexBuffer)->GetNumVerticies(),
				reinterpret_cast<D3D12IndexBuffer*>(m_pIndexBuffer)->GetNumIndices(),
				m_Transform.GetWorldMatrix()
			);

			break;
		}
		case Renderer::eTargetRenderAPI::INVALID:
		{
			IE_CORE_FATAL(L"Mesh trying to be created before the renderer has been initialized.");
			break;
		}
		default:
		{
			IE_CORE_ERROR("Failed to determine vertex buffer type for mesh.");
			break;
		}
		}
	}
}

