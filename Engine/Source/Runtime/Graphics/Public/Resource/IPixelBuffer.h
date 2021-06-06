#pragma once

#include "EngineDefines.h"

#include "Graphics/Public/Resource/IGPUResource.h"

namespace Insight
{
	namespace Graphics
	{
		class INSIGHT_API IPixelBuffer
		{
		public:
			virtual ResourceDesc DescribeTex2D(UInt32 Width, UInt32 Height, UInt32 DepthOrArraySize, UInt32 NumMips, EFormat Format, UInt32 Flags) = 0;
			virtual void AssociateWithResource(IDevice* Device, const FString& Name, void* Resource, EResourceState CurrentState) = 0;
			virtual void CreateTextureResource(IDevice* Device, const FString& Name, const ResourceDesc& ResourceDesc, const ClearValue& ClearValue) = 0;

			FORCEINLINE EFormat GetFormat() const { return m_Format; }

			static FORCEINLINE EFormat GetBaseFormat(EFormat BaseFormat);
			static FORCEINLINE EFormat GetUAVFormat(EFormat BaseFormat);
			static FORCEINLINE EFormat GetDSVFormat(EFormat DefaultFormat);
			static FORCEINLINE EFormat GetStencilFormat(EFormat DefaultFormat);
			static FORCEINLINE EFormat GetDepthFormat(EFormat DefaultFormat);

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
			case F_R8G8B8A8_UNorm:
			case F_R8G8B8A8_UNorm_SRGB:
				return F_R8G8B8A8_Typeless;

			case F_B8G8R8A8_UNorm:
			case F_B8G8R8A8_UNorm_SRGB:
				return F_B8G8R8A8_UNorm_Typeless;

			case F_B8G8R8X8_UNorm:
			case F_B8G8R8X8_UNorm_SRGB:
				return F_B8G8R8X8_UNorm_Typeless;

			// 32-bit Z w/ Stencil
			case F_R32G8X24_Typeless:
			case F_D32_Float_S8X24_Typeless:
			case F_R32_Float_X8X24_Typeless:
			case F_X32_Typeless_G8X24_UInt:
				return F_R32G8X24_Typeless;

			// No Stencil
			case F_R32_Typeless:
			case F_D32_Float:
			case F_R32_Float:
				return F_R32_Typeless;

			// 24-bit Z
			case F_R24G8_Typeless:
			case R_D24_UNorm_S8_UInt:
			case F_R24_UNorm_X8_Typeless:
			case F_X24_Typeless_G8_UInt:
				return F_R24G8_Typeless;

			// 16-bit Z w/o Stencil
			case F_R16_Typeless:
			case F_D16_UNorm:
			case F_R16_UNorm:
				return F_R16_Typeless;

			default:
				return BaseFormat;
			}
		}

		EFormat IPixelBuffer::GetUAVFormat(EFormat BaseFormat)
		{
			switch (BaseFormat)
			{
			case F_R8G8B8A8_UNorm:
			case F_R8G8B8A8_Typeless:
			case F_R8G8B8A8_UNorm_SRGB:
				return F_R8G8B8A8_UNorm;

			default:
				return BaseFormat;
			}
		}

		EFormat IPixelBuffer::GetDSVFormat(EFormat DefaultFormat)
		{
			switch (DefaultFormat)
			{
				// 32-Bit Z with Stencil
			case F_R32G8X24_Typeless:
			case F_D32_Float_S8X24_Typeless:
			case F_R32_Float_X8X24_Typeless:
			case F_X32_Typeless_G8X24_UInt:
				return F_D32_Float_S8X24_Typeless;

				// No Stencil
			case F_R32_Typeless:
			case F_D32_Float:
			case F_R32_Float:
				return F_D32_Float;

				// 24-Bit Z
			case F_R24G8_Typeless:
			case R_D24_UNorm_S8_UInt:
			case F_R24_UNorm_X8_Typeless:
			case F_X24_Typeless_G8_UInt:
				return R_D24_UNorm_S8_UInt;

				// 16-Bit Z no Stencil
			case F_R16_Typeless:
			case F_D16_UNorm:
			case F_R16_UNorm:
				return F_D16_UNorm;

			default:
				return DefaultFormat;
			}
		}
		
		EFormat IPixelBuffer::GetStencilFormat(EFormat DefaultFormat)
		{
			switch (DefaultFormat)
			{
			// 32-bit Z w/ Stencil
			case F_R32G8X24_Typeless:
			case F_D32_Float_S8X24_Typeless:
			case F_R32_Float_X8X24_Typeless:
			case F_X32_Typeless_G8X24_UInt:
				return F_X32_Typeless_G8X24_UInt;

			// 24-Bit Depth
			case F_R24G8_Typeless:
			case R_D24_UNorm_S8_UInt:
			case F_R24_UNorm_X8_Typeless:
			case F_X24_Typeless_G8_UInt:
				return F_X24_Typeless_G8_UInt;

			default:
				return F_Unknown;
			}
		}

		EFormat IPixelBuffer::GetDepthFormat(EFormat DefaultFormat)
		{
			switch (DefaultFormat)
			{
			// 32-bit Z w/ Stencil
			case F_R32G8X24_Typeless:
			case F_D32_Float_S8X24_Typeless:
			case F_R32_Float_X8X24_Typeless:
			case F_X32_Typeless_G8X24_UInt:
				return F_R32_Float_X8X24_Typeless;

			// No Stencil
			case F_R32_Typeless:
			case F_D32_Float:
			case F_R32_Float:
				return F_R32_Float;

			// 24-bit Z
			case R_D24_UNorm_S8_UInt:
			case F_R24_UNorm_X8_Typeless:
			case F_X24_Typeless_G8_UInt:
				return F_R24_UNorm_X8_Typeless;

			// 16-bit Z w/o Stencil
			case F_R16_Typeless:
			case F_D16_UNorm:
			case F_R16_UNorm:
				return F_R16_UNorm;

			default:
				return F_Unknown;
			}
		}
	}
}
