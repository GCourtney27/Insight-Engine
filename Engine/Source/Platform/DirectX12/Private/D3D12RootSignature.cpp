#include <Engine_pch.h>

#include "Platform/DirectX12/Public/D3D12RootSignature.h"

#include "Runtime/Graphics/Public/IDevice.h"
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

/*

				std::vector<CD3DX12_ROOT_PARAMETER> RootParams;
				RootParams.reserve(Desc.NumParams);
				for (size_t i = 0; i < Desc.NumParams; ++i)
				{
					const RootParameter& Param = Desc.pParameters[i];
					CD3DX12_ROOT_PARAMETER CRootParam;
					switch (Param.ParameterType)
					{
					case RPT_ConstantBufferView:
						CRootParam.InitAsConstantBufferView(Param.Descriptor.ShaderRegister, Param.Descriptor.RegisterSpace, (D3D12_SHADER_VISIBILITY)Param.ShaderVisibility);
						break;
					case RPT_DescriptorTable:
					{
						std::vector<CD3DX12_DESCRIPTOR_RANGE> Ranges;
						Ranges.reserve(Param.DescriptorTable.NumDescriptors);
						for (size_t i = 0; i < Param.DescriptorTable.NumDescriptors; ++i)
						{
							CD3DX12_DESCRIPTOR_RANGE CRange;
							CRange.Init(
								(D3D12_DESCRIPTOR_RANGE_TYPE)Param.DescriptorTable.pDescriptorRanges[i].Type,
								Param.DescriptorTable.pDescriptorRanges[i].NumDescriptors,
								Param.DescriptorTable.pDescriptorRanges[i].BaseShaderRegister,
								Param.DescriptorTable.pDescriptorRanges[i].OffsetInDescriptorsFromTableStart);
							Ranges.push_back(CRange);
						}
						CRootParam.InitAsDescriptorTable(Param.DescriptorTable.NumDescriptors, Ranges.data(), (D3D12_SHADER_VISIBILITY)Param.ShaderVisibility);
					}
					}
					RootParams.push_back(CRootParam);
				}
*/
