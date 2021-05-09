#pragma once

#include <Runtime/Core.h>
#include <Runtime/Graphics/Public/GraphicsCore.h>


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

			virtual void CreatePipelineState(const PipelineStateDesc& PSODesc, IPipelineState** ppOutPSO) = 0;
			virtual void CreateRootSignature(const RootSignatureDesc& RSDesc, IRootSignature** ppOutSignature) = 0;
			virtual void CreateDescriptorHeap(const EString&& DebugHeapName, EResourceHeapType&& Type, UInt32&& MaxCount, IDescriptorHeap** ppOutHeap) = 0;
			virtual void CreateColorBuffer(const EString& Name, UInt32 Width, UInt32 Height, UInt32 NumMips, EFormat Format, IColorBuffer** ppOutColorBuffer) = 0;
			virtual void CreateDepthBuffer(const EString& Name, UInt32 Width, UInt32 Height, EFormat Format, IDepthBuffer** ppOutDepthBuffer) = 0;
			virtual void CopyDescriptors(
				UInt32 NumDestDescriptorRanges,
				const CpuDescriptorHandle* pDestDescriptorRangeStarts,
				const UInt32* pDestDescriptorRangeSizes,
				UInt32 NumSrcDescriptorRanges,
				const ITexture** pSrcDescriptorRangeStarts,
				const UInt32* pSrcDescriptorRangeSizes,
				EResourceHeapType DescriptorHeapsType
			) = 0;

		protected:
			IDevice() {}
			virtual ~IDevice() {}

			virtual void Initialize(const IEDeviceInitParams& InitParams, IEDeviceQueryResult& OutDeviceQueryResult, void** ppFactoryContext) = 0;
			virtual void UnInitialize() = 0;
		};
	}
}

