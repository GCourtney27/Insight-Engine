#pragma once

#include <Runtime/Core.h>

namespace Insight
{
	namespace Graphics
	{
		
		struct IEDeviceInitParams
		{
			bool ForceWarpAdapter;
		};

		struct IEDeviceQueryResult
		{
			EString DeviceName;
		};

		class INSIGHT_API IDevice
		{
			friend class IRenderContext;
		public:
			virtual void* GetNativeDevice() const = 0;


		protected:
			IDevice() {}
			virtual ~IDevice() {}

			virtual void Initialize(const IEDeviceInitParams& InitParams, IEDeviceQueryResult& OutDeviceQueryResult, void** ppFactoryContext) = 0;
			virtual void UnInitialize() = 0;
		};
	}
}

