#pragma once

#include <Runtime/CoreMacros.h>

namespace Insight
{
	namespace Graphics
	{
		class INSIGHT_API IRenderContext
		{
		public:

			virtual void Initialize() = 0;
			virtual void UnInitialize() = 0;

			virtual void CreateTexture() = 0;
			virtual void CreateBuffer() = 0;
			virtual ieVertexBuffer& CreateVertexBuffer() = 0;
			virtual ieIndexBuffer& CreateIndexBuffer() = 0;

			virtual void BindVertexBuffer(ieVertexBuffer const& Vertexbuffer) = 0;
			virtual void BindIndexBuffer(ieIndexBuffer const& IndexBuffer) = 0;

			virtual void DrawMesh() = 0;

			// Debug Utilites
			//void DrawLine(FVector3 Start, FVector3 End, Color LineColor)
			//void DrawOnScreenText(FVector2 Location, Color TextColor)
			//void DrawRay(FVector3 Start, FVector3 Direction, Color LineColor, bool bIgnoreDepth)

		protected:
			IRenderContext() = default;
			virtual ~IRenderContext() = default;
		};
	}
} 
