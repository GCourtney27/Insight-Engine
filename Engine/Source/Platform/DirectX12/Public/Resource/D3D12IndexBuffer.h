#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/Resource/IIndexBuffer.h"
#include "Platform/DirectX12/Public/Resource/D3D12GPUResource.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			class INSIGHT_API D3D12IndexBuffer : public IIndexBuffer, public D3D12GPUResource
			{
				friend class D3D12GeometryBufferManager;
			public:
				D3D12IndexBuffer() {}
				~D3D12IndexBuffer() {}
				
				virtual void* GetNativeBufferView() override { return RCast<void*>(&m_D3D12IndexBufferView); }


			protected:
				D3D12_INDEX_BUFFER_VIEW m_D3D12IndexBufferView;

			};
		}
	}
}