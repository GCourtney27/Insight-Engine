#pragma once

#include "EngineDefines.h"

#include "Graphics/Public/GraphicsCore.h"
#include "Math/Public/Matricies.h"
#include "Graphics/Public/IGeometryBufferManager.h"



namespace Insight
{
	struct DrawArgs
	{
		UInt32 NumVerts;
		UInt32 NumIndices;
		Graphics::VertexBufferUID VertexBufferHandle;
		Graphics::IndexBufferUID IndexBufferHandle;
	};

	IE_ALIGN(16) struct MeshWorldCBData
	{
		FMatrix WorldMat;
	};

	struct GeometryVertex3D
	{
		FVector3 Position;
		FVector3 Normal;
		FVector3 Tangent;
		FVector3 BiTangent;
		FVector4 Color;
		FVector2 UV0;
	};

	
	class INSIGHT_API StaticMeshGeometry
	{
		friend class StaticGeometryManager;
	public:
		inline StaticMeshGeometry();
		inline virtual ~StaticMeshGeometry();
		
		//
		// Getters and Setters
		//
		inline Graphics::IVertexBuffer& GetVertexBuffer();
		inline Graphics::IIndexBuffer& GetIndexBuffer();
		inline UInt32 GetNumVerticies() const;
		inline UInt32 GetNumIndices() const;
		inline UInt64 GetUID() const;

		void Create(void* pVertexData, UInt32 VertexDataSize, UInt32 NumVerticies, UInt32 VertexSize, void* pIndexData, UInt32 IndexDataSize, UInt32 NumIndices);

	private:
		inline void Initialize();
		inline void UnInitialize();

		inline void SetHashName(UInt64 NewUID);


	protected:
		DrawArgs m_DrawArgs;

		UInt64 m_Uid;
		// TODO AABB for culling
	};


	//
	// Inline function definitions
	//

	inline StaticMeshGeometry::StaticMeshGeometry()
	{
		Initialize();
	}

	inline StaticMeshGeometry::~StaticMeshGeometry()
	{
		UnInitialize();
	}

	inline Graphics::IVertexBuffer& StaticMeshGeometry::GetVertexBuffer()
	{
		return Graphics::g_pGeometryManager->GetVertexBufferByUID(m_DrawArgs.VertexBufferHandle);
	}

	inline Graphics::IIndexBuffer& StaticMeshGeometry::GetIndexBuffer()
	{
		return Graphics::g_pGeometryManager->GetIndexBufferByUID(m_DrawArgs.IndexBufferHandle);
	}

	inline UInt32 StaticMeshGeometry::GetNumVerticies() const
	{
		return m_DrawArgs.NumVerts;
	}
	
	inline UInt32 StaticMeshGeometry::GetNumIndices() const
	{
		return m_DrawArgs.NumIndices;
	}

	inline UInt64 StaticMeshGeometry::GetUID() const
	{
		return m_Uid;
	}

	inline void StaticMeshGeometry::Initialize()
	{
		m_DrawArgs.VertexBufferHandle = Graphics::g_pGeometryManager->AllocateVertexBuffer();
		m_DrawArgs.IndexBufferHandle = Graphics::g_pGeometryManager->AllocateIndexBuffer();
	}

	inline void StaticMeshGeometry::UnInitialize()
	{
		Graphics::g_pGeometryManager->DeAllocateVertexBuffer(m_DrawArgs.VertexBufferHandle);
		Graphics::g_pGeometryManager->DeAllocateIndexBuffer(m_DrawArgs.IndexBufferHandle);
	}
	
	inline void StaticMeshGeometry::SetHashName(UInt64 NewGUID)
	{
		m_Uid = NewGUID;
	}
}
