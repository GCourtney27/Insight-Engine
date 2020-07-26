#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Geometry/Vertex_Buffer.h"

namespace Insight {

	class INSIGHT_API D3D12VertexBuffer : public ieVertexBuffer
	{
	public:
		D3D12VertexBuffer(Verticies Verticies);
		virtual ~D3D12VertexBuffer()
		{
			Destroy();
		}
		virtual void Destroy() override;

		ID3D12Resource& GetVertexBuffer() { return *m_pVertexBuffer; }
		ID3D12Resource& GetUploadHeap() { return *m_pVertexBufferUploadHeap; }
		D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &m_VertexBufferView; }

	protected:
		virtual bool CreateResources() override;

	private:
		ID3D12Resource*				m_pVertexBufferUploadHeap = 0;
		ID3D12Resource*				m_pVertexBuffer = 0;
		D3D12_VERTEX_BUFFER_VIEW	m_VertexBufferView = {};
	};
}
