#pragma once

#include <Runtime/CoreMacros.h>

#include "Runtime/Graphics/Geometry/VertexBuffer.h"

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

		Microsoft::WRL::ComPtr<ID3D12Resource> GetVertexBuffer() { return m_pVertexBuffer; }
		Microsoft::WRL::ComPtr<ID3D12Resource> GetUploadHeap() { return m_pVertexBufferUploadHeap; }
		D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &m_VertexBufferView; }

	protected:
		virtual bool CreateResources() override;

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource>		m_pVertexBufferUploadHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource>		m_pVertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW	m_VertexBufferView = {};
	};
}
