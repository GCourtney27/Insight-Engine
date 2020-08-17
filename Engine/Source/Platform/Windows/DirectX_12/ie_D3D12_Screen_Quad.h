#pragma once

#include <Insight/Core.h>

using Microsoft::WRL::ComPtr;

namespace Insight {


	class INSIGHT_API ieD3D12ScreenQuad
	{
	public:
		void Init();
		void OnRender(ComPtr<ID3D12GraphicsCommandList> commandList);

	private:
		ComPtr<ID3D12Resource> m_VertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
		UINT m_NumVerticies = 0u;
	};

}
