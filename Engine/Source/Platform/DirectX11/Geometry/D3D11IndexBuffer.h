#pragma once

#include <Runtime/Core.h>

#include "Runtime/Rendering/Geometry/IndexBuffer.h"

namespace Insight {

	class INSIGHT_API D3D11IndexBuffer : public ieIndexBuffer
	{
	public:
		D3D11IndexBuffer(Indices Indices);
		~D3D11IndexBuffer()
		{
			Destroy();
		}
		virtual void Destroy() override;

		inline UINT GetBufferOffset() { return m_BufferOffset; }
		inline ID3D11Buffer* const* GetBufferPtrConst() { return &m_pIndexBuffer; }
		inline ID3D11Buffer* GetBufferPtr() { return m_pIndexBuffer; }
		inline DXGI_FORMAT GetFormat() { return m_Format; }
	protected:
		virtual bool CreateResources() override;

	private:
		ID3D11Buffer* m_pIndexBuffer = 0;
		UINT m_Offset;
		UINT m_BufferOffset;
		DXGI_FORMAT m_Format;
	};

}
