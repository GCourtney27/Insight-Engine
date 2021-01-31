#pragma once
#include <Insight/Core.h>

#include "Platform/DirectXShared/ConstantBufferTypes.h"



template <class ConstantBufferType>
class INSIGHT_API ConstantBuffer
{
private:
	ConstantBuffer(const ConstantBuffer<ConstantBufferType>& rhs);

private:

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_Buffer;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;

public:
	ConstantBuffer() {}

	ConstantBufferType Data;

	ID3D11Buffer* Get() const
	{
		return m_Buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf()const
	{
		return m_Buffer.GetAddressOf();
	}

	HRESULT Init(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	{
		if (m_Buffer.Get() != nullptr) {
			m_Buffer.Reset();
		}

		this->m_pDeviceContext = pDeviceContext;

		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.ByteWidth = static_cast<UINT>(sizeof(ConstantBufferType) + (16 - (sizeof(ConstantBufferType) % 16)));
		desc.StructureByteStride = 0;

		HRESULT hr = pDevice->CreateBuffer(&desc, 0, m_Buffer.GetAddressOf());
		return hr;
	}

	bool SubmitToGPU()
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		this->m_pDeviceContext->Map(m_Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &Data, sizeof(ConstantBufferType));
		this->m_pDeviceContext->Unmap(m_Buffer.Get(), 0);
		return true;
	}
};

