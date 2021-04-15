#include <Engine_pch.h>

#include "Platform/DirectX12/Public/D3D12RootSignature.h"

#include "Runtime/Graphics/Public/GraphicsCore.h"
#include "Runtime/Graphics/Private/IDevice.h"

#include "Platform/Public/Utility/COMException.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			D3D12RootSignature::D3D12RootSignature()
				: m_pID3D12RootSignature(NULL)
			{
			}

			D3D12RootSignature::~D3D12RootSignature()
			{
				COM_SAFE_RELEASE(m_pID3D12RootSignature);
			}
			
			void D3D12RootSignature::Initialize(const RootSignatureDesc& Desc)
			{
				ID3D12Device* pID3D12Device = RCast<ID3D12Device*>(g_pDevice->GetNativeDevice());

				CD3DX12_ROOT_SIGNATURE_DESC RSDesc;
				RSDesc.Init(Desc.NumParams, RCast<const D3D12_ROOT_PARAMETER*>(Desc.pParameters), Desc.NumStaticSamplers, RCast<const D3D12_STATIC_SAMPLER_DESC*>(Desc.pStaticSamplers), (D3D12_ROOT_SIGNATURE_FLAGS)Desc.Flags);

				ID3DBlob* pSignature;
				ID3DBlob* pErrorBuffer;
				ThrowIfFailed(D3D12SerializeRootSignature(&RSDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignature, &pErrorBuffer)
					, TEXT("Failed to deserialize root signature!"));
				ThrowIfFailed(pID3D12Device->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&m_pID3D12RootSignature))
					, TEXT("Failed to create root signature!"));

				COM_SAFE_RELEASE(pErrorBuffer);
				COM_SAFE_RELEASE(pSignature);
			}
		}
	}
}
