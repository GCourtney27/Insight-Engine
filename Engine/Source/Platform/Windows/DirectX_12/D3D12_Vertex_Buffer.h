#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Geometry/Vertex_Buffer.h"

namespace Insight {

	class INSIGHT_API D3D12VertexBuffer : public VertexBuffer
	{
	public:
		D3D12VertexBuffer() = default;
		virtual ~D3D12VertexBuffer()
		{
			Destroy();
		}
		virtual void Init() override;
		virtual void Destroy() override;

	private:
		ID3D12Resource* m_pVertexBufferUploadHeap = 0;
		ID3D12Resource* m_pVertexBuffer = 0;
		D3D12_VERTEX_BUFFER_VIEW	m_VertexBufferView = {};
	};
}
