#pragma once

#include <Runtime/Core.h>

namespace Insight
{
	namespace Graphics
	{
		struct RootSignatureDesc;

		class INSIGHT_API IRootSignature
		{
		public:
			virtual void* GetNativeSignature() = 0;

			virtual void Initialize(const RootSignatureDesc& Desc) = 0;

		protected:
			IRootSignature() {}
			virtual ~IRootSignature() {}

		};
	}
}