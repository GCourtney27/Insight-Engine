#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Geometry/Index_Buffer.h"

namespace Insight {

	class INSIGHT_API D3D12IndexBuffer : public IndexBuffer
	{
	public:
		D3D12IndexBuffer() = default;
		~D3D12IndexBuffer()
		{
			Destroy();
		}
		virtual void Init() override;
		virtual void Destroy() override;

	private:
		ID3D12Resource* m_pIndexBufferUploadHeap = 0;
		ID3D12Resource* m_pIndexBuffer = 0;
		D3D12_INDEX_BUFFER_VIEW		m_IndexBufferView = {};
	};
}
