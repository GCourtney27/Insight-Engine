#pragma once

#include "EngineDefines.h"

namespace Insight
{
	namespace Graphics
	{
		enum EFormat;
		struct SampleDesc;
		struct PipelineStateDesc;

		class INSIGHT_API IPipelineState
		{
		public:
			virtual ~IPipelineState() 
			{
			}
			virtual void* GetNativePSO() = 0;

			virtual void Initialize(const PipelineStateDesc& Desc) = 0;

		protected:
			IPipelineState() {}
			
		};
	}
}
