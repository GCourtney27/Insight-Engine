#pragma once

#include <Insight/Core.h>

using Microsoft::WRL::ComPtr;

namespace Insight {


	class INSIGHT_API D3D12ScreenQuad
	{
	public:
		void Init(ScreenSpaceVertex Verticies[], uint32_t VertexBufferSize, uint32_t Indices[], uint32_t IndexBufferSize);
		void OnRender(ComPtr<ID3D12GraphicsCommandList> commandList);

	private:
		ComPtr<ID3D12Resource> m_VertexBuffer;
		ComPtr<ID3D12Resource>		m_pIndexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
		UINT m_NumVerticies = 0U;
		UINT m_NumIndices = 0U;
	};

}
