#pragma once

#include <Runtime/Core.h>

// Insight
#include "Runtime/Graphics/Public/IRenderContext.h"
// Platform
#include "Platform/DirectX12/Public/DeviceResources.h"


namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{

			class INSIGHT_API D3D12RenderContext : public IRenderContext
			{
			public:
				D3D12RenderContext() = default;
				virtual ~D3D12RenderContext() = default;

				virtual void Initialize() override;
				virtual void UnInitialize() override;

				virtual void PreFrame() override;
				virtual void SubmitFrame() override;

				virtual void CreateTexture() override;
				virtual void CreateBuffer() override;
				virtual ieVertexBuffer& CreateVertexBuffer() override { return m_VertexBuffers[0]; }
				virtual ieIndexBuffer& CreateIndexBuffer() override { return m_IndexBuffers[0]; }

				virtual void BindVertexBuffer(ieVertexBuffer const& Vertexbuffer) override {}
				virtual void BindIndexBuffer(ieIndexBuffer const& IndexBuffer) override {}

				virtual void DrawMesh() override {}

			protected:

			};
		}
	}
}