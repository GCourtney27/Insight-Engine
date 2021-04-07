#include <Engine_pch.h>

#include "Platform/DirectX12/Public/Resource/D3D12ColorBuffer.h"

#include "Platform/DirectX12/Public/D3D12Device.h"
#include "Platform/DirectX12/Private/D3D12CommonGlobals.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{

			void D3D12ColorBuffer::CreateFromSwapChain(IDevice* pDevice, const EString& Name, void* pResource)
			{
				ID3D12Device* pID3D12Device = RCast<ID3D12Device*>(pDevice->GetNativeDevice());
				AssociateWithResource(pDevice, Name, pResource, RS_Present);

				m_RTVHandle = AllocateDescriptor(pID3D12Device, (D3D12_DESCRIPTOR_HEAP_TYPE)RHT_RTV);
				pID3D12Device->CreateRenderTargetView(RCast<ID3D12Resource*>(pResource), NULL, m_RTVHandle);
			}
			
			void D3D12ColorBuffer::Create(IDevice* pDevice, const EString& Name, UInt32 Width, UInt32 Height, UInt32 NumMips, ETextureFormat Format)
			{

			}
		}
	}
}
