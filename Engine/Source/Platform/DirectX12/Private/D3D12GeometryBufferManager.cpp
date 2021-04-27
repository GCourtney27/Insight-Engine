#include <Engine_pch.h>

#include "Platform/DirectX12/Public/D3D12GeometryBufferManager.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			VertexBufferUID D3D12GeometryBufferManager::AllocateVertexBuffer()
			{
				VertexBufferUID NewUID = s_NextVertexBufferID++;

				auto InsertResult = m_VertexBufferLUT.try_emplace(NewUID, D3D12VertexBuffer{});
				IE_ASSERT(InsertResult.second == true); // Trying to create a vertex buffer with an already existing ID! This is not allowed.

				m_VertexBufferLUT[NewUID].SetUID(NewUID);

				return NewUID;
			}

			IndexBufferUID D3D12GeometryBufferManager::AllocateIndexBuffer()
			{
				IndexBufferUID NewUID = s_NextIndexBufferID++;
				auto InsertResult = m_IndexBufferLUT.try_emplace(NewUID, D3D12IndexBuffer{});
				IE_ASSERT(InsertResult.second == true); // Trying to create a index buffer with an already existing ID! This is not allowed.

				m_IndexBufferLUT[NewUID].SetUID(NewUID);

				return NewUID;
			}
		}
	}
}