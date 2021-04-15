#pragma once

#include <Runtime/Core.h>
#include <Runtime/Graphics/Public/GraphicsCore.h>

#include "Runtime/Graphics/Public/Resource/IPixelBuffer.h"
#include "Platform/DirectX12/Public/Resource/D3D12GPUResource.h"

#include "Platform/DirectX12/Public/Common/D3D12Utility.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			class INSIGHT_API D3D12PixelBuffer : public IPixelBuffer, public D3D12GPUResource
			{
			public:
				D3D12PixelBuffer() {}
				virtual ~D3D12PixelBuffer() {}

				virtual ResourceDesc DescribeTex2D(UInt32 Width, UInt32 Height, UInt32 DepthOrArraySize, UInt32 NumMips, EFormat Format, UInt32 Flags) override;
				virtual void AssociateWithResource(IDevice* Device, const EString& Name, void* Resource, EResourceState CurrentState) override;
				virtual void CreateTextureResource(IDevice* Device, const EString& Name, const ResourceDesc& ResourceDesc, const ClearValue& ClearValue) override;

			};
		}
	}
}
