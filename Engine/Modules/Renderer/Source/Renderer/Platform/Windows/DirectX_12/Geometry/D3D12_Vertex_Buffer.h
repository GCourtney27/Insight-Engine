#pragma once

#include <Retina/Core.h>

#include "Retina/Rendering/Geometry/Vertex_Buffer.h"

namespace Retina {

	using Microsoft::WRL::ComPtr;

	class RETINA_API D3D12VertexBuffer : public ieVertexBuffer
	{
	public:
		D3D12VertexBuffer(Verticies Verticies);
		virtual ~D3D12VertexBuffer()
		{
			Destroy();
		}
		virtual void Destroy() override;

		ComPtr<ID3D12Resource> GetVertexBuffer() { return m_pVertexBuffer; }
		ComPtr<ID3D12Resource> GetUploadHeap() { return m_pVertexBufferUploadHeap; }
		D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &m_VertexBufferView; }

	protected:
		virtual bool CreateResources() override;

	private:
		ComPtr<ID3D12Resource>		m_pVertexBufferUploadHeap;
		ComPtr<ID3D12Resource>		m_pVertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW	m_VertexBufferView = {};
	};
}
