#include <ie_pch.h>

#include "D3D12_Vertex_Buffer.h"

namespace Insight {



	void D3D12VertexBuffer::Init()
	{
	}

	void D3D12VertexBuffer::Destroy()
	{
		COM_SAFE_RELEASE(m_pVertexBuffer);
		COM_SAFE_RELEASE(m_pVertexBufferUploadHeap);
	}

}
