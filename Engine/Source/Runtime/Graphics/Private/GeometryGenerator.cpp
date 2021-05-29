#include <Engine_pch.h>

#include "Runtime/Graphics/Public/GeometryGenerator.h"

namespace Insight
{
	namespace GeometryGenerator
	{
		struct Vertex2D
		{
			FVector2 Position;
			FVector2 UVs;
		};

		StaticMeshGeometryRef GenerateScreenAlignedQuadMesh()
		{
			static const FString QuadGeometryName = TEXT("ScrenSpaceQuad");
			if (Graphics::g_StaticGeometryManager.MeshExists(QuadGeometryName))
			{
				return Graphics::g_StaticGeometryManager.GetStaticMeshByName(QuadGeometryName);
			}

			UInt32 Indices[] =
			{
				0, 1, 3,
				0, 3, 2
			};
			const UInt32 NumIndices = IE_ARRAYSIZE(Indices);
			const UInt32 IndexBufferSize = NumIndices * sizeof(UInt32);

			Vertex2D Verts[] =
			{
				{ { -1.0f, 1.0f }, { 0.0f, 0.0f } }, // Top Left
				{ {  1.0f, 1.0f }, { 1.0f, 0.0f } }, // Top Right
				{ { -1.0f,-1.0f }, { 0.0f, 1.0f } }, // Bottom Left
				{ {  1.0f,-1.0f }, { 1.0f, 1.0f } }, // Bottom Right
			};
			const UInt32 NumVerts = IE_ARRAYSIZE(Verts);
			const UInt32 VertexBufferSize = NumVerts * sizeof(Vertex2D);

			return Graphics::g_StaticGeometryManager.RegisterGeometry(
				QuadGeometryName,
				Verts, VertexBufferSize, NumVerts, sizeof(Vertex2D),
				Indices, IndexBufferSize, NumIndices
			);
		}
	}
}
