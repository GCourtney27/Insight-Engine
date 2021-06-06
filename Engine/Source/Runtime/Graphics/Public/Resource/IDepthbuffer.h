#pragma once

#include "EngineDefines.h"


namespace Insight
{
	namespace Graphics
	{
		enum EFormat;
		class INSIGHT_API IDepthBuffer
		{
		public:
			virtual ~IDepthBuffer() {}

			virtual void Create(const FString& Name, UInt32 Width, UInt32 Height, EFormat Format) = 0;

			FORCEINLINE float GetClearDepth() const { return m_ClearDepth; }
			FORCEINLINE UInt8 GetClearStencil() const { return m_ClearStencil; }

		protected:
			IDepthBuffer(float ClearDepth, UInt8 ClearStencil)
				: m_ClearDepth(ClearDepth)
				, m_ClearStencil(ClearStencil)
			{
			}

			float m_ClearDepth;
			UInt8 m_ClearStencil;
		};
	}
}
