#include <Engine_pch.h>

#include "Platform/DirectX12/Public/D3D12Device.h"

#include "Platform/Public/Utility/COMException.h"
#include "Platform/DirectX12/Public/D3D12RootSignature.h"
#include "Platform/DirectX12/Public/D3D12PipelineState.h"
#include "Platform/DirectX12/Public/D3D12Descriptorheap.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{

			D3D12Device::D3D12Device()
				: m_pD3DDevice(NULL)
			{
			}

			D3D12Device::~D3D12Device()
			{
				UnInitialize();
			}
			
			void D3D12Device::Initialize(const IEDeviceInitParams& InitParams, IEDeviceQueryResult& OutDeviceQueryResult, void** ppFactoryContext)
			{
				IDXGIFactory6** ppDXGIFactory = RCast<IDXGIFactory6**>(ppFactoryContext);
				IE_ASSERT(ppDXGIFactory);

				const IED3D12DeviceInitParams& D3D12InitParams = SCast<const IED3D12DeviceInitParams&>(InitParams);
				IED3D12DeviceQueryResult& D3D12QueryResult = SCast<IED3D12DeviceQueryResult&>(OutDeviceQueryResult);

				// Create the Device
				//
				if (InitParams.ForceWarpAdapter)
				{
					Microsoft::WRL::ComPtr<IDXGIAdapter> pWarpAdapter;
					ThrowIfFailed((*ppDXGIFactory)->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)), TEXT("Failed to enumerate warp adapter!"));

					ThrowIfFailed(D3D12CreateDevice(
						pWarpAdapter.Get(),
						D3D12InitParams.MinFeatureLevel,
						IID_PPV_ARGS(&m_pD3DDevice)
					), TEXT("Failed to create D3D12 warp device!"));
				}
				else
				{
					Microsoft::WRL::ComPtr<IDXGIAdapter1> hardwareAdapter;
					GetHardwareAdapter((*ppDXGIFactory), &hardwareAdapter, D3D12InitParams, D3D12QueryResult);

					ThrowIfFailed(D3D12CreateDevice(
						hardwareAdapter.Get(),
						D3D12InitParams.MinFeatureLevel,
						IID_PPV_ARGS(&m_pD3DDevice)
					), TEXT("Failed to create D3D12 device!"));
				}
			}

			void D3D12Device::CreatePipelineState(const PipelineStateDesc& PSODesc, IPipelineState** ppOutPSO)
			{
				D3D12PipelineState* pD3D12PSO = CreateRenderComponentObject< D3D12PipelineState>(ppOutPSO);
				pD3D12PSO->Initialize(PSODesc);
			}

			void D3D12Device::CreateRootSignature(const RootSignatureDesc& RSDesc, IRootSignature** ppOutSignature)
			{
				D3D12RootSignature* pD3D12RS = CreateRenderComponentObject<D3D12RootSignature>(ppOutSignature);
				pD3D12RS->Initialize(RSDesc);
			}
			void D3D12Device::CreateDescriptorHeap(const EString&& DebugHeapName, EResourceHeapType&& Type, UInt32&& MaxCount, IDescriptorHeap** ppOutHeap)
			{
				D3D12DescriptorHeap* pD3D12Heap = CreateRenderComponentObject<D3D12DescriptorHeap>(ppOutHeap);
				pD3D12Heap->Create(DebugHeapName, Type, MaxCount);
			}

			void D3D12Device::GetHardwareAdapter(IDXGIFactory6* pFactory, IDXGIAdapter1** ppAdapter, const IED3D12DeviceInitParams& InitParams, IED3D12DeviceQueryResult& OutDeviceQueryResult)
			{
				Microsoft::WRL::ComPtr<IDXGIAdapter1> pAdapter;
				*ppAdapter = NULL;
				UInt32 CurrentMemory = 0;
				DXGI_ADAPTER_DESC1 Desc;

				for (UInt32 AdapterIndex = 0;
					DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapterByGpuPreference(AdapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&pAdapter));
					++AdapterIndex)
				{
					Desc = { 0 };
					pAdapter->GetDesc1(&Desc);

					// Make sure we get the video card that is not a software adapter
					// and it has the most video memory.
					//
					if (Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE || Desc.DedicatedVideoMemory < CurrentMemory) continue;

#				if IE_WITH_DXR
					// Check if we can support ray tracing with the device.
					if (InitParams.CheckForDXRSupport)
					{
						// Pass in a temp device to poll feature support later.
						Microsoft::WRL::ComPtr<ID3D12Device5> TempDevice;
						if (SUCCEEDED(D3D12CreateDevice(pAdapter.Get(), InitParams.MinDXRFeatureLevel, __uuidof(ID3D12Device5), &TempDevice)))
						{
							// If the device supports the feature level check to see if it can support DXR.
							//
							if (CheckDXRSupport(TempDevice.Get()))
							{
								CurrentMemory = SCast<UInt32>(Desc.DedicatedVideoMemory);
								if (*ppAdapter != NULL)
									(*ppAdapter)->Release();

								*ppAdapter = pAdapter.Detach();
								OutDeviceQueryResult.IsDXRSupported = true;

								IE_LOG(Log, TEXT("Found suitable D3D 12 hardware that can support DXR: %s"), Desc.Description);
								continue;
							}
						}
					}
#				endif // IE_WITH_DXR

					// If we cannot support ray tracing, see if D3D12 is supported on the adapter.
					//
					if (SUCCEEDED(D3D12CreateDevice(pAdapter.Get(), InitParams.MinFeatureLevel, __uuidof(ID3D12Device), NULL)))
					{
						CurrentMemory = static_cast<UINT>(Desc.DedicatedVideoMemory);
						if (*ppAdapter != NULL) (*ppAdapter)->Release();

						*ppAdapter = pAdapter.Detach();
						IE_LOG(Log, TEXT("Found suitable D3D 12 hardware: %s"), Desc.Description);
					}
				}
				IE_ASSERT((*ppAdapter)); // Could not locate a proper adapter that supports D3D12.

				Desc = { 0 };
				(*ppAdapter)->GetDesc1(&Desc);
				OutDeviceQueryResult.DeviceName = Desc.Description;
				IE_LOG(Warning, TEXT("\"%s\" selected as D3D 12 graphics hardware."), OutDeviceQueryResult.DeviceName.c_str());
			}

			void D3D12Device::UnInitialize()
			{
				COM_SAFE_RELEASE(m_pD3DDevice);
			}



			// 
			// Utility Functions
			// 

			bool D3D12Device::CheckSM6Support(ID3D12Device* pDevice)
			{
				IE_ASSERT(pDevice);
				D3D12_FEATURE_DATA_SHADER_MODEL sm6_0{ D3D_SHADER_MODEL_6_0 };
				ThrowIfFailed(pDevice->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &sm6_0, sizeof(sm6_0))
					, TEXT("Failed to query feature support for shader model 6 with device."));
				return (sm6_0.HighestShaderModel >= D3D_SHADER_MODEL_6_0);
			}

			bool D3D12Device::CheckDXRSupport(ID3D12Device* pDevice)
			{
				IE_ASSERT(pDevice);
				D3D12_FEATURE_DATA_D3D12_OPTIONS5 Options5 = {};
				ThrowIfFailed(pDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &Options5, sizeof(Options5))
					, TEXT("Failed to query feature support for ray trace with device."));
				return (Options5.RaytracingTier > D3D12_RAYTRACING_TIER_1_0);
			}

		}
	}
}

