#pragma once

#include "EngineDefines.h"

namespace Insight {

	

	class INSIGHT_API ieD3D12SphereRenderer
	{
	public:
		struct SimpleVertex
		{
			DirectX::XMFLOAT4 position;
		};
	public:
		void Init(float radius, int slices, int segments);
		void ResourceSetup();
		void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);

		Microsoft::WRL::ComPtr<ID3D12Resource> GetVertexBuffer() { return m_VertexBuffer; }
		Microsoft::WRL::ComPtr<ID3D12Resource> GetIndexBuffer() { return m_IndexBuffer; }

		int GetVertexCount() { return m_NumTriangles; }
		int GetIndexBufferCount() { return m_NumIndices; }

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_DefaultBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_VertexView;
		D3D12_INDEX_BUFFER_VIEW m_IndexView;
		int m_Slices;
		int m_Segments;
		int m_NumTriangles;
		int m_NumIndices;
		float m_Radius;
		
	};

}
