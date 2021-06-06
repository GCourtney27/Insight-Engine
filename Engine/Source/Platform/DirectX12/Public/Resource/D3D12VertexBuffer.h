#pragma once

#include "EngineDefines.h"

#include "Graphics/Public/Resource/IVertexBuffer.h"
#include "Platform/DirectX12/Public/Resource/D3D12GPUResource.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			class INSIGHT_API D3D12VertexBuffer : public IVertexBuffer, public D3D12GPUResource
			{
				friend class D3D12GeometryBufferManager;
			public:
				D3D12VertexBuffer() {}
				virtual ~D3D12VertexBuffer() {}

				virtual void* GetNativeBufferView() override { return RCast<void*>(&m_D3D12VertexBufferView); }

				virtual void Create(const FString& Name, UInt32 VertexDataSize, UInt32 VertexStrideSize, void* pVerticies) override;

			protected:
				D3D12_VERTEX_BUFFER_VIEW m_D3D12VertexBufferView;
			};
		}
	}
}