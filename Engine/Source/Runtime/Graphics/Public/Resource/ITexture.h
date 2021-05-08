#pragma once

#include <Runtime/Core.h>
#include <Runtime/Graphics/Public/GraphicsCore.h>

namespace Insight
{
	namespace Graphics
	{
		class INSIGHT_API ITexture
		{
		public:
			virtual void Create2D(UInt64 RowPitchBytes, UInt64 Width, UInt64 Height, EFormat Format, const void* InitData) = 0;
			virtual void CreateCube(UInt64 RowPitchBytes, UInt64 Width, UInt64 Height, EFormat Format, const void* InitialData) = 0;

			virtual void Destroy() = 0;

			FORCE_INLINE UInt32 GetWidth() const { return m_Width; }
			FORCE_INLINE UInt32 GetHeight() const { return m_Height; }
			FORCE_INLINE UInt32 GetDepth() const { return m_Depth; }

		protected:
			ITexture() 
				: m_Width(0)
				, m_Height(0)
				, m_Depth(0)
			{
			}
			virtual ~ITexture() {}

			UInt32 m_Width;
			UInt32 m_Height;
			UInt32 m_Depth;
		};
	}
}