#pragma once

#include <Runtime/CoreMacros.h>

#include "Runtime/Rendering/Geometry/VertexBuffer.h"

namespace Insight {

	class INSIGHT_API D3D11VertexBuffer : public ieVertexBuffer
	{
	public:
		D3D11VertexBuffer(Verticies Verticies);
		virtual ~D3D11VertexBuffer()
		{
			Destroy();
		}
		virtual void Destroy() override;

		inline const UINT* GetBufferOffset() { return &m_BufferOffset; }
		inline const UINT GetStride() const { return m_Stride; }
		inline const UINT* GetStridePtr() const { return &m_Stride; }
		inline ID3D11Buffer* const* GetBufferPtr() { return &m_pVertexBuffer; }

	protected:
		virtual bool CreateResources() override;

	private:
		ID3D11Buffer* m_pVertexBuffer = 0;
		UINT m_Stride;
		UINT m_BufferOffset;
	};

}
