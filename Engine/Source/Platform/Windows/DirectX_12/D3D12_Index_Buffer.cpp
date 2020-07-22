#include <ie_pch.h>

#include "D3D12_Index_Buffer.h"

namespace Insight {


	void D3D12IndexBuffer::Init()
	{

	}

	void D3D12IndexBuffer::Destroy()
	{
		COM_SAFE_RELEASE(m_pIndexBuffer);
		COM_SAFE_RELEASE(m_pIndexBufferUploadHeap);
	}

}
