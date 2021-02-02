#pragma once

#include <Runtime/CoreMacros.h>



namespace Insight {


	class INSIGHT_API D3D12ScreenQuad
	{
	public:
		void Init(ScreenSpaceVertex Verticies[], uint32_t VertexBufferSize, uint32_t Indices[], uint32_t IndexBufferSize);
		void OnRender(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> 		m_pVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource>		m_pIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource>		m_pVertexBufferUploadHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource>		m_pIndexBufferUploadHeap;
		
		D3D12_VERTEX_BUFFER_VIEW	m_VertexBufferView;
		D3D12_INDEX_BUFFER_VIEW		m_IndexBufferView;
		
		UINT						m_NumVerticies = 0U;
		UINT						m_NumIndices = 0U;
	};

}
