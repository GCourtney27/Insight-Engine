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
			virtual ResourceDesc DescribeTex2D(UInt32 Width, UInt32 Height, UInt32 DepthOrArraySize, UInt32 NumMips, EFormat Format, UInt32 Flags) = 0;
			virtual void AssociateWithResource(IDevice* Device, const EString& Name, void* Resource, EResourceState CurrentState) = 0;
			virtual void CreateTextureResource(IDevice* Device, const EString& Name, const ResourceDesc& ResourceDesc, const ClearValue& ClearValue) = 0;

			static FORCE_INLINE EFormat GetBaseFormat(EFormat BaseFormat);
			static FORCE_INLINE EFormat GetUAVFormat(EFormat BaseFormat);

			// Compute the number of texture levels needed to reduce to 1x1.  This uses
			// _BitScanReverse to find the highest set bit.  Each dimension reduces by
			// half and truncates bits.  The dimension 256 (0x100) has 9 mip levels, same
			// as the dimension 511 (0x1FF).
			static inline UInt32 ComputeNumMips(UInt32 Width, UInt32 Height)
			{
				UInt32 HighBit;
				_BitScanReverse((unsigned long*)&HighBit, Width | Height);
				return HighBit + 1;
			}

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
			EFormat m_Format;
		};

		//
		// Inline function implementations
		//

		EFormat IPixelBuffer::GetBaseFormat(EFormat BaseFormat)
		{
			switch (BaseFormat)
			{
			case F_R8G8B8A8_UNORM:
				return F_R8G8B8A8_TYPELESS;

			case F_R32G8X24_TYPELESS:
			case F_R32_FLOAT_X8X24_TYPELESS:
				return F_R32G8X24_TYPELESS;

			default:
				return F_R8G8B8A8_UNORM;
			}
		}

		EFormat IPixelBuffer::GetUAVFormat(EFormat BaseFormat)
		{
			switch (BaseFormat)
			{
			case F_R8G8B8A8_UNORM:
			case F_R8G8B8A8_TYPELESS:
			case F_R8G8B8A8_UNORM_SRGB:
				return F_R8G8B8A8_UNORM;

			default:
				return BaseFormat;
			}
		}
	}
}
