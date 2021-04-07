#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/Resource/IGPUResource.h"

namespace Insight
{
	namespace Graphics
	{
		class INSIGHT_API IPixelBuffer
		{
		public:
			virtual ResourceDesc DescribeTex2D(UInt32 Width, UInt32 Height, UInt32 DepthOrArraySize, UInt32 NumMips, ETextureFormat Format, UInt32 Flags) = 0;
			virtual void AssociateWithResource(IDevice* Device, const EString& Name, void* Resource, EResourceState CurrentState) = 0;
			virtual void CreateTextureResource(IDevice* Device, const EString& Name, const ResourceDesc& ResourceDesc, const ClearValue& ClearValue) = 0;

			static FORCE_INLINE ETextureFormat GetBaseFormat(ETextureFormat BaseFormat);
		protected:
			IPixelBuffer()
				: m_Width(0u)
				, m_Height(0u)
				, m_ArraySize(0u)
			{
			}

			virtual ~IPixelBuffer()
			{
			}

			UInt32 m_Width;
			UInt32 m_Height;
			UInt32 m_ArraySize;
			ETextureFormat m_Format;
		};

		ETextureFormat IPixelBuffer::GetBaseFormat(ETextureFormat BaseFormat)
		{
			switch (BaseFormat)
			{
			case TF_R8G8B8A8_UNORM:
				return TF_R8G8B8A8_TYPELESS;

			case TF_R32G8X24_TYPELESS:
			case TF_R32_FLOAT_X8X24_TYPELESS:
				return TF_R32G8X24_TYPELESS;

			default:
				return TF_R8G8B8A8_UNORM;
			}
		}
	}
}
