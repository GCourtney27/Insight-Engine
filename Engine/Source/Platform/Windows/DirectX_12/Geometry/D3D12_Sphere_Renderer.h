#pragma once

#include <Insight/Core.h>

namespace Insight {

	using Microsoft::WRL::ComPtr;

	class ieD3D12SphereRenderer
	{
	public:
		void Init(float radius, int slices, int segments);
		void resourceSetup();
		void Render(ComPtr<ID3D12GraphicsCommandList> commandList);

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
		struct SimpleVertex
		{
			DirectX::XMFLOAT4 position;
		};
	};

}
