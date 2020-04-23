#pragma once
#ifndef ConstantBuffer_h__
#define ConstantBuffer_h__
#include <d3dx12.h>
#include "ConstantBuffersPerObject_TEMP.h"
#include <wrl/client.h>

template<class T>
class ConstantBuffer
{
private:
	ConstantBuffer(const ConstantBuffer<T>& rhs);

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
	ID3D12GraphicsCommandList* commandList = nullptr;

public:
	ConstantBuffer() {}

	T data;

	ID3D12Resource* Get() const
	{
		return buffer.Get();
	}

	ID3D12Resource* const* GetAddressOf()const
	{
		return buffer.GetAddressOf();
	}

	HRESULT Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
	{
		if (buffer.Get() != nullptr)
		{
			buffer.Reset();
		}

		this->commandList = commandList;

		HRESULT hr = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)))),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&buffer)
		);
		buffer->SetName(L"Vertex Buffer Resource Heap");
		return hr;
	}

	bool ApplyChanges()
	{
		/*D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = this->deviceContext->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to map constatn buffer.");
			return false;
		}
		CopyMemory(mappedResource.pData, &data, sizeof(T));
		this->deviceContext->Unmap(buffer.Get(), 0);*/
		return true;
	}
};

#endif // ConstantBuffer_h__