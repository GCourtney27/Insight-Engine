#pragma once

#include <Retina/Core.h>

namespace Retina {

	using Microsoft::WRL::ComPtr;

	class ieD3D12SphereRenderer
	{
	public:
		struct SimpleVertex
		{
			DirectX::XMFLOAT4 position;
		};
	public:
		void Init(float radius, int slices, int segments);
		void resourceSetup();
		void Render(ComPtr<ID3D12GraphicsCommandList> commandList);

		ComPtr<ID3D12Resource> GetVertexBuffer() { return m_VertexBuffer; }
		ComPtr<ID3D12Resource> GetIndexBuffer() { return m_IndexBuffer; }

		int GetVertexCount() { return m_TriangleSize; }
		int GetIndexBufferCount() { return m_IndexSize; }

	private:
		ComPtr<ID3D12Resource> m_VertexBuffer;
		ComPtr<ID3D12Resource> m_DefaultBuffer;
		ComPtr<ID3D12Resource> m_IndexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_VertexView;
		D3D12_INDEX_BUFFER_VIEW m_IndexView;
		int m_Slices;
		int m_Segments;
		int m_TriangleSize;
		int m_IndexSize;
		float m_Radius;
		
	};

}
