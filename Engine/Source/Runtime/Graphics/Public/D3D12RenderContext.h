#pragma once

#include <Runtime/CoreMacros.h>

// Insight
#include "Runtime/Graphics/Public/IRenderContext.h"
// Platform
#include "Platform/DirectX12/Public/DeviceResources.h"

using namespace Insight;
using namespace Insight::Graphics;

namespace DX12
{

	class INSIGHT_API D3D12RenderContext : public IRenderContext
	{
	public:
		D3D12RenderContext() = default;
		virtual ~D3D12RenderContext() = default;

		virtual void Initialize() override;
		virtual void UnInitialize() override;

		virtual void CreateTexture() override;
		virtual void CreateBuffer() override;
		virtual ieVertexBuffer& CreateVertexBuffer() override;
		virtual ieIndexBuffer& CreateIndexBuffer() override;

		virtual void BindVertexBuffer(ieVertexBuffer const& Vertexbuffer) override;
		virtual void BindIndexBuffer(ieIndexBuffer const& IndexBuffer) override;

		virtual void DrawMesh() override;

	protected:
		//DeviceResources m_DeviceResources;

	private:
		//std::vector<D3D12_VERTEX_BUFFER_VIEW> m_VertexBuffers;
		//std::vector<D3D12_INDEX_BUFFER_VIEW> m_VertexBuffers;
	};
}