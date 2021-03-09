#include <Engine_pch.h>

#include "Mesh.h"

#include "Runtime/Core/Application.h"
#include "Runtime/Rendering/Renderer.h"

#include "Platform/DirectX11/Geometry/D3D11IndexBuffer.h"
#include "Platform/DirectX11/Geometry/D3D11VertexBuffer.h"
#include "Platform/DirectX12/Geometry/D3D12IndexBuffer.h"
#include "Platform/DirectX12/Geometry/D3D12VertexBuffer.h"
#include "Platform/DirectX12/Direct3D12Context.h"



namespace Insight {


	Mesh::Mesh(const Verticies& Verticies, const Indices& Indices)
	{
		Init(Verticies, Indices);
	}

	Mesh::Mesh(Mesh&& mesh) noexcept
	{
		IE_LOG(Warning, TEXT("Mesh being moved in memory."));
		
		m_pIndexBuffer = mesh.m_pIndexBuffer;
		m_pVertexBuffer = mesh.m_pVertexBuffer;

		m_Transform = std::move(mesh.m_Transform);
		m_ConstantBufferPerObject = mesh.m_ConstantBufferPerObject;
	}

	Mesh::~Mesh()
	{
		Destroy();
	}

	void Mesh::Destroy()
	{
		delete m_pVertexBuffer;
		delete m_pIndexBuffer;
	}

	void Mesh::Init(const Verticies& Verticies, const Indices& Indices)
	{
		CreateBuffers(Verticies, Indices);
	}

	void Mesh::PreRender(const XMMATRIX& parentMat)
	{
		m_Transform.SetWorldMatrix(XMMatrixMultiply(parentMat, m_Transform.GetLocalMatrix()));
		m_ConstantBufferPerObject.World = m_Transform.GetWorldMatrixRef();

		if (m_ShouldUpdateAS) UpdateAccelerationStructures();
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

	void Mesh::CreateBuffers(const Verticies& Verticies, const Indices& Indices)
	{
		switch (Renderer::GetAPI()) {
		case Renderer::ETargetRenderAPI::Direct3D_11:
		{
			m_pVertexBuffer = new D3D11VertexBuffer(Verticies);
			m_pIndexBuffer = new D3D11IndexBuffer(Indices);
			break;
		}
		case Renderer::ETargetRenderAPI::Direct3D_12:
		{
			m_pVertexBuffer = new D3D12VertexBuffer(Verticies);
			m_pIndexBuffer = new D3D12IndexBuffer(Indices);

			if (Renderer::GetIsRayTraceEnabled()) {

				m_ShouldUpdateAS = true;
				
				m_RTInstanceIndex = Renderer::GetAs<Direct3D12Context>()
					.RegisterGeometryWithRTAccelerationStucture(
						reinterpret_cast<D3D12VertexBuffer*>(m_pVertexBuffer)->GetVertexBuffer(),
						reinterpret_cast<D3D12IndexBuffer*>(m_pIndexBuffer)->GetIndexBuffer(),
						reinterpret_cast<D3D12VertexBuffer*>(m_pVertexBuffer)->GetNumVerticies(),
						reinterpret_cast<D3D12IndexBuffer*>(m_pIndexBuffer)->GetNumIndices(),
						m_Transform.GetWorldMatrix()
					);
			}

			break;
		}
		case Renderer::ETargetRenderAPI::Invalid:
		{
			assert(false); // Mesh trying to be created before the renderer has been initialized.
			break;
		}
		default:
		{
			IE_LOG(Error, TEXT("Failed to determine vertex buffer type for mesh."));
			break;
		}
		}
	}
	
	void Mesh::UpdateAccelerationStructures()
	{
		Renderer::GetAs<Direct3D12Context>().UpdateRTAccelerationStructureMatrix(m_RTInstanceIndex, m_Transform.GetWorldMatrix());
	}

}

