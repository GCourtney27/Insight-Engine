#include <Renderer_pch.h>

#include "D3D11_Index_Buffer.h"

#include "Platform/Windows/DirectX_11/Direct3D11_Context.h"

namespace Retina {

	D3D11IndexBuffer::D3D11IndexBuffer(Indices Indices)
		: ieIndexBuffer(Indices)
	{
		m_NumIndices = static_cast<uint32_t>(Indices.size());
		m_BufferSize = m_NumIndices * sizeof(uint32_t);
		m_BufferOffset = 0U;
		m_Format = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;

		CreateResources();
	}

	void D3D11IndexBuffer::Destroy()
	{
		COM_SAFE_RELEASE(m_pIndexBuffer);
	}

	bool D3D11IndexBuffer::CreateResources()
	{
		Direct3D11Context* D3D11Context = dynamic_cast<Direct3D11Context*>(&Renderer::Get());

		if (m_pIndexBuffer != nullptr) {
			m_pIndexBuffer->Release();
		}

		//Load Index Data
		D3D11_BUFFER_DESC IndexBufferDesc;
		ZeroMemory(&IndexBufferDesc, sizeof(IndexBufferDesc));
		IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		IndexBufferDesc.ByteWidth = sizeof(DWORD) * m_NumIndices;
		IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		IndexBufferDesc.CPUAccessFlags = 0;
		IndexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexBufferData;
		indexBufferData.pSysMem = reinterpret_cast<void*>(m_Indices.data());
		HRESULT hr = D3D11Context->GetDevice().CreateBuffer(&IndexBufferDesc, &indexBufferData, &m_pIndexBuffer);
		ThrowIfFailed(hr, "Failed to create index buffer for D3D 11 context.");

		return true;
	}

}
