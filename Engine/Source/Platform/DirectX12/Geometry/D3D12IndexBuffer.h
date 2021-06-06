#pragma once

#include "EngineDefines.h"

#include "Graphics/Geometry/IndexBuffer.h"

namespace Insight {

	

	class INSIGHT_API D3D12IndexBuffer : public ieIndexBuffer
	{
	public:
		D3D12IndexBuffer(Indices Indices);
		~D3D12IndexBuffer()
		{
			Destroy();
		}
		virtual void Destroy() override;

		Microsoft::WRL::ComPtr<ID3D12Resource> GetIndexBuffer() { return m_pIndexBuffer; }
		Microsoft::WRL::ComPtr<ID3D12Resource> GetUploadHeap() { return m_pIndexBufferUploadHeap; }
		D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return m_IndexBufferView; }

	protected:
		virtual bool CreateResources() override;

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource>		m_pIndexBufferUploadHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource>		m_pIndexBuffer;
		D3D12_INDEX_BUFFER_VIEW		m_IndexBufferView = {};
	};
}
