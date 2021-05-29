#include <Engine_pch.h>

#include "Runtime/Graphics/Public/WorldRenderer/StaticMeshGeometry.h"

#include "Runtime/Graphics/Public/ResourceManagement/IConstantBufferManager.h"
#include "Runtime/Graphics/Public/Resource/IVertexBuffer.h"
#include "Runtime/Graphics/Public/Resource/IIndexBuffer.h"
#include "Runtime/Graphics/Public/ICommandContext.h"
#include "Runtime/Graphics/Public/WorldRenderer/Common.h"


namespace Insight
{
	void StaticMeshGeometry::Create(void* pVertexData, UInt32 VertexDataSize, UInt32 NumVerticies, UInt32 VertexSize, void* pIndexData, UInt32 IndexDataSize, UInt32 NumIndices)
	{
		// Create the vertex buffer
		m_DrawArgs.NumVerts = NumVerticies;

		// Init Vertex buffer.
		IE_ASSERT(m_DrawArgs.VertexBufferHandle != IE_INVALID_VERTEX_BUFFER_HANDLE); // Vertex buffer was not registered properly with geometry buffer manager.
		Graphics::IVertexBuffer& Buffer = Graphics::g_pGeometryManager->GetVertexBufferByUID(m_DrawArgs.VertexBufferHandle);
		Buffer.Create(TEXT("Vertex Buffer"), VertexDataSize, VertexSize, pVertexData);

		// Create the index buffer
		m_DrawArgs.NumIndices = NumIndices;

		// Init Index buffer
		IE_ASSERT(m_DrawArgs.IndexBufferHandle != IE_INVALID_INDEX_BUFFER_HANDLE); // Index buffer was not registered properly with geometry buffer manager.
		Graphics::IIndexBuffer& IndexBuffer = Graphics::g_pGeometryManager->GetIndexBufferByUID(m_DrawArgs.IndexBufferHandle);
		IndexBuffer.Create(TEXT("Index Buffer"), IndexDataSize, pIndexData);
	}
}
