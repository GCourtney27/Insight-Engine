#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/CommonEnums.h"

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
			virtual void CreateFromSwapChain(IDevice* pDevice, const EString& Name, void* pResource) = 0;
			virtual void Create(IDevice* pDevice, const EString& Name, UInt32 Width, UInt32 Height, UInt32 NumMips, ETextureFormat Format) = 0;

			FORCE_INLINE void SetClearColor(Color Color) { m_ClearColor = Color; }
			FORCE_INLINE Color GetClearColor() const { return m_ClearColor; }

		protected:
			IColorBuffer() {}
			virtual ~IColorBuffer() {}

			Color m_ClearColor;
			UInt32 m_NumMips;
			UInt32 m_FragmentCount;
			UInt32 m_SampleCount;

		};
	}
}
