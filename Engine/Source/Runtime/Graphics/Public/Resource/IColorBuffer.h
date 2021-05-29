#pragma once

#include <Runtime/Core.h>
#include <Runtime/Graphics/Public/GraphicsCore.h>

#include "Runtime/Graphics/Public/Resource/IPixelBuffer.h"


namespace Insight
{
	namespace Graphics
	{
		class IDevice;

		class INSIGHT_API IColorBuffer
		{
			friend class ISwapChain;
		public:
			virtual ~IColorBuffer() = default;

			virtual void CreateFromSwapChain(IDevice* pDevice, const FString& Name, void* pResource) = 0;
			virtual void Create(IDevice* pDevice, const FString& Name, UInt32 Width, UInt32 Height, UInt32 NumMips, EFormat Format) = 0;

			FORCE_INLINE void SetClearColor(Color Color) { m_ClearColor = Color; }
			FORCE_INLINE Color GetClearColor() const { return m_ClearColor; }


		protected:
			IColorBuffer()
				: m_ClearColor(0.f, 0.f, 0.f, 1.f)
				, m_NumMipMaps(0u)
				, m_FragmentCount(1u)
				, m_SampleCount(1u)
			{
			}

			virtual void CreateDerivedViews(IDevice* pDevice, EFormat Format, UInt32 ArraySize, UInt32 NumMips) = 0;

			EResourceFlags CombineResourceFlags() const
			{
				EResourceFlags Flags = RF_None;

				if (Flags == RF_None && m_FragmentCount == 1)
					Flags |= RF_AllowUnorderedAccess;

				return RF_AllowRenderTarget | Flags;
			}

		protected:
			Color m_ClearColor;
			UInt32 m_NumMipMaps;
			UInt32 m_FragmentCount;
			UInt32 m_SampleCount;
		};
	}
}
