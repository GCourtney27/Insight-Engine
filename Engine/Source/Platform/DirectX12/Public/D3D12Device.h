#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Private/IDevice.h"


namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			struct IED3D12DeviceInitParams : public IEDeviceInitParams
			{
				bool CheckForDXRSupport;
				D3D_FEATURE_LEVEL MinFeatureLevel;
				D3D_FEATURE_LEVEL MinDXRFeatureLevel;
			};

			struct IED3D12DeviceQueryResult : public IEDeviceQueryResult
			{
				bool IsDXRSupported;
			};

			class INSIGHT_API D3D12Device final : public IDevice
			{
				friend class D3D12RenderContextFactory;

			public:
				D3D12Device();
				virtual ~D3D12Device();

				virtual void* GetNativeDevice() const override { return RCast<void*>(m_pD3DDevice); }



			protected:
				void GetHardwareAdapter(IDXGIFactory6* pFactory, IDXGIAdapter1** ppAdapter, const IED3D12DeviceInitParams& InitParams, IED3D12DeviceQueryResult& OutDeviceQueryResult);
				
				//
				// Virtual Functions
				//
				virtual void UnInitialize() override;
				virtual void Initialize(const IEDeviceInitParams& InitParams, IEDeviceQueryResult& OutDeviceQueryResult, void** ppFactoryContext) override;

				//
				// Feature Check Functions
				// 
				bool CheckSM6Support(ID3D12Device* pDevice);
				bool CheckDXRSupport(ID3D12Device* pDevice);

			private:
				ID3D12Device* m_pD3DDevice;

			};
		}
	}
}
