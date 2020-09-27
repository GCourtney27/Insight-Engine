#pragma once

#include <Retina/Core.h>

#include "Retina/Rendering/Geometry/Index_Buffer.h"

namespace Retina {

	using Microsoft::WRL::ComPtr;

	class RETINA_API D3D12IndexBuffer : public ieIndexBuffer
	{
	public:
		D3D12IndexBuffer(Indices Indices);
		~D3D12IndexBuffer()
		{
			Destroy();
		}
		virtual void Destroy() override;

		ComPtr<ID3D12Resource> GetIndexBuffer() { return m_pIndexBuffer; }
		ComPtr<ID3D12Resource> GetUploadHeap() { return m_pIndexBufferUploadHeap; }
		D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return m_IndexBufferView; }

	protected:
		virtual bool CreateResources() override;

	private:
		ComPtr<ID3D12Resource>		m_pIndexBufferUploadHeap;
		ComPtr<ID3D12Resource>		m_pIndexBuffer;
		D3D12_INDEX_BUFFER_VIEW		m_IndexBufferView = {};
	};
}
