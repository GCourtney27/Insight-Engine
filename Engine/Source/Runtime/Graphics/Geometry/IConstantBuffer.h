#pragma once

#include "EngineDefines.h"

namespace Insight
{
	class INSIGHT_API IConstantBuffer
	{
	public:
		virtual void Bind() = 0;

		virtual void Create() = 0;

	protected:
		IConstantBuffer() = default;
		~IConstantBuffer() = default;
	};
}
