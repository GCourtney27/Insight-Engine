#include <Renderer_pch.h>

#include "D3D11_Vertex_Buffer.h"

#include "Platform/Windows/DirectX_11/Direct3D11_Context.h"


namespace Insight {



	D3D11VertexBuffer::D3D11VertexBuffer(Verticies Verticies)
	{
		m_NumVerticies = static_cast<uint32_t>(Verticies.size());
		m_BufferSize = m_NumVerticies * sizeof(Vertex3D);
		m_Stride = sizeof(Vertex3D);
		m_BufferOffset = 0U;
		m_Verticies = std::move(Verticies);

		CreateResources();
	}

	void D3D11VertexBuffer::Destroy()
	{
		COM_SAFE_RELEASE(m_pVertexBuffer);
	}

	bool D3D11VertexBuffer::CreateResources()
	{
		if (m_pVertexBuffer != nullptr) {
			m_pVertexBuffer->Release();
		}

		Direct3D11Context& RenderContext = Renderer::GetAs<Direct3D11Context>();
		
		D3D11_BUFFER_DESC VertexBufferDesc = {};
		VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		VertexBufferDesc.ByteWidth = m_Stride * m_NumVerticies;
		VertexBufferDesc.CPUAccessFlags = 0;
		VertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA VertexBufferData;
		VertexBufferData.pSysMem = m_Verticies.data();

		HRESULT hr = RenderContext.GetDevice().CreateBuffer(&VertexBufferDesc, &VertexBufferData, &m_pVertexBuffer);
		ThrowIfFailed(hr, "Failed to create vertex buffer for D3D 11 context.");

		return true;
	}

}
