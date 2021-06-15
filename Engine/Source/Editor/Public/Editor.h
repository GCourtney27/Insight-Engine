#pragma once

#include "EngineDefines.h"

namespace Insight
{
	namespace Graphics
	{
		class IRenderContext;
	}
	namespace Editor
	{
		class INSIGHT_API Editor
		{
		public:
			Editor();
			~Editor();
			
			void Initialize(Graphics::IRenderContext* pRenderContext, shared_ptr<Window> pWindow);

		protected:
			bool m_bIsInitialized;

			
		};
	}
}
