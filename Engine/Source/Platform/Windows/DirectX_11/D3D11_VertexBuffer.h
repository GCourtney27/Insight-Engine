#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

using Microsoft::WRL::ComPtr;

template<class T>
class D3D11VertexBuffer
{
private:
	ComPtr <ID3D11Buffer> m_Buffer;
	uint32_t m_Stride = sizeof(T);
	uint32_t m_VertexCount = 0;

public:
	D3D11VertexBuffer() {}

	D3D11VertexBuffer(const D3D11VertexBuffer<T>& rhs)
	{
		m_Buffer = rhs.m_Buffer;
		m_VertexCount = rhs.m_VertexCount;
		m_Stride = rhs.m_Stride;
	}

	D3D11VertexBuffer<T>& operator =(const D3D11VertexBuffer<T>& a)
	{
		m_Buffer = a.m_Buffer;
		m_VertexCount = a.m_VertexCount;
		m_Stride = a.m_Stride;
		return *this;
	}

	ID3D11Buffer* Get()const
	{
		return m_Buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf()const
	{
		return m_Buffer.GetAddressOf();
	}

	uint32_t VertexCount() const
	{
		return m_VertexCount;
	}

	const uint32_t Stride() const
	{
		return m_Stride;
	}

	const uint32_t* StridePtr() const
	{
		return &m_Stride;
	}

	HRESULT Initialize(ID3D11Device* pDevice, T* pData, uint32_t VertexCount)
	{
		if (m_Buffer.Get() != nullptr)
		{
			m_Buffer.Reset();
		}
		m_VertexCount = VertexCount;

		D3D11_BUFFER_DESC VertexBufferDesc = {};

		VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		VertexBufferDesc.ByteWidth = m_Stride * VertexCount;
		VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		VertexBufferDesc.CPUAccessFlags = 0;
		VertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA VertexBufferData = {};
		VertexBufferData.pSysMem = pData;

		HRESULT hr = pDevice->CreateBuffer(&VertexBufferDesc, &VertexBufferData, m_Buffer.GetAddressOf());
		return hr;
	}
};
