#include <Engine_pch.h>

#include "Platform/DirectX12/Public/Resource/D3D12PixelBuffer.h"

#include "Platform/DirectX12/Public/D3D12Device.h"
#include "Platform/Public/Utility/COMException.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			ResourceDesc D3D12PixelBuffer::DescribeTex2D(UInt32 Width, UInt32 Height, UInt32 DepthOrArraySize, UInt32 NumMips, EFormat Format, UInt32 Flags)
			{
				m_Width = Width;
				m_Height = Height;
				m_ArraySize = DepthOrArraySize;
				m_Format = Format;

				ResourceDesc Desc = {};
				Desc.Alignment = 0;
				Desc.DepthOrArraySize = (UInt16)DepthOrArraySize;
				Desc.Dimension = RD_Texture_2D;
				Desc.Flags = (EResourceFlags)Flags;
				Desc.Format = GetBaseFormat(Format);
				Desc.Height = (UInt32)Height;
				Desc.Layout = TL_Unknown;
				Desc.MipLevels = (UInt16)NumMips;
				Desc.SampleDesc.Count = 1;
				Desc.SampleDesc.Quality = 0;
				Desc.Width = (UInt64)Width;

				return Desc;
			}

			void D3D12PixelBuffer::AssociateWithResource(IDevice* pDevice, const FString& Name, void* pResource, EResourceState CurrentState)
			{
				IE_ASSERT(pResource != NULL);

				ID3D12Resource* pD3D12Resource = RCast<ID3D12Resource*>(pResource);
				m_pID3D12Resource.Attach(pD3D12Resource);

				D3D12_RESOURCE_DESC Desc = pD3D12Resource->GetDesc();
				
				m_UsageState = CurrentState;

				m_Width = (UInt32)Desc.Width;
				m_Height = Desc.Height;
				m_ArraySize = Desc.DepthOrArraySize;
				m_Format = (EFormat)Desc.Format;

#		if IE_DEBUG
				pD3D12Resource->SetName(Name.c_str());
#		endif // IE_DEBUG
			}
			
			void D3D12PixelBuffer::CreateTextureResource(IDevice* pDevice, const FString& Name, const ResourceDesc& ResourceDesc, const ClearValue& ClearValue)
			{
				IE_ASSERT(pDevice != NULL);
				
				D3D12Device* pD3D12Device = DCast<D3D12Device*>(pDevice);
				IE_ASSERT(pD3D12Device != NULL);

				ID3D12Device* pID3D12Device = RCast<ID3D12Device*>(pD3D12Device->GetNativeDevice());
				IE_ASSERT(pID3D12Device != NULL);

				D3D12_CLEAR_VALUE D3D12ClearVal;
				ZeroMemRanged(&D3D12ClearVal, sizeof(D3D12_CLEAR_VALUE));
				memcpy(D3D12ClearVal.Color, ClearValue.Color, 4 * sizeof(float));
				D3D12ClearVal.DepthStencil.Depth = ClearValue.DepthStencil.Depth;
				D3D12ClearVal.DepthStencil.Stencil = ClearValue.DepthStencil.Stencil;
				D3D12ClearVal.Format = (DXGI_FORMAT)ClearValue.Format;

				{
					CD3DX12_HEAP_PROPERTIES HeapProps(D3D12_HEAP_TYPE_DEFAULT);
					HRESULT hr = pID3D12Device->CreateCommittedResource(
						&HeapProps, 
						D3D12_HEAP_FLAG_NONE, 
						RCast<const D3D12_RESOURCE_DESC*>(&ResourceDesc), 
						(D3D12_RESOURCE_STATES)RS_Common, 
						&D3D12ClearVal,
						IID_PPV_ARGS(&m_pID3D12Resource)
					);
					ThrowIfFailed(hr, TEXT("Failed to create committed GPU resource!"));
				}

				m_UsageState = RS_Common;
				m_GpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;

#		if IE_DEBUG
				m_pID3D12Resource->SetName(Name.c_str());
#		endif // IE_DEBUG
			}
			
		}
	}
}
