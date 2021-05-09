#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/IDevice.h"


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


				virtual void CreatePipelineState(const PipelineStateDesc& PSODesc, IPipelineState** ppOutPSO) override;
				virtual void CreateRootSignature(const RootSignatureDesc& RSDesc, IRootSignature** ppOutSignature) override;
				virtual void CreateDescriptorHeap(const EString&& DebugHeapName, EResourceHeapType&& Type, UInt32&& MaxCount, IDescriptorHeap** ppOutHeap) override;
				virtual void CreateColorBuffer(const EString& Name, UInt32 Width, UInt32 Height, UInt32 NumMips, EFormat Format, IColorBuffer** ppOutColorBuffer) override;
				virtual void CreateDepthBuffer(const EString& Name, UInt32 Width, UInt32 Height, EFormat Format, IDepthBuffer** ppOutDepthBuffer) override;
				virtual void CopyDescriptors(
					UInt32 NumDestDescriptorRanges,
					const CpuDescriptorHandle* pDestDescriptorRangeStarts,
					const UInt32* pDestDescriptorRangeSizes,
					UInt32 NumSrcDescriptorRanges,
					const ITexture** pSrcDescriptorRangeStarts,
					const UInt32* pSrcDescriptorRangeSizes,
					EResourceHeapType DescriptorHeapsType
				) override;

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
