#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Geometry/Index_Buffer.h"

namespace Insight {

	class INSIGHT_API D3D12IndexBuffer : public IndexBuffer
	{
	public:
		D3D12IndexBuffer(Indices Indices);
		~D3D12IndexBuffer()
		{
			Destroy();
		}
		virtual void Destroy() override;

		ID3D12Resource& GetIndexBuffer() { return *m_pIndexBuffer; }
		ID3D12Resource& GetUploadHeap() { return *m_pIndexBufferUploadHeap; }
		D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return m_IndexBufferView; }

	protected:
		virtual bool CreateResources() override;

	private:
		ID3D12Resource* m_pIndexBufferUploadHeap = 0;
		ID3D12Resource* m_pIndexBuffer = 0;
		D3D12_INDEX_BUFFER_VIEW		m_IndexBufferView = {};
	};
}
