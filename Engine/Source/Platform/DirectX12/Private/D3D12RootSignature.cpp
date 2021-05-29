#include <Engine_pch.h>

#include "Platform/DirectX12/Public/D3D12RootSignature.h"

#include "Runtime/Graphics/Public/IDevice.h"
#include "Runtime/Core/Public/Utility/Hash.h"
#include "Platform/Public/Utility/COMException.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			static std::map< size_t, ::Microsoft::WRL::ComPtr<ID3D12RootSignature> > s_RootSignatureHashMap;

			D3D12RootSignature::D3D12RootSignature()
				: m_pID3D12RootSignature(NULL)
			{
			}

			D3D12RootSignature::~D3D12RootSignature()
			{
				COM_SAFE_RELEASE(m_pID3D12RootSignature);
			}


            void D3D12RootSignature::DestroyAll(void)
            {
                s_RootSignatureHashMap.clear();
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

            void D3D12RootSignature::Finalize(const FString& name, ERootSignatureFlags Flags)
            {
                if (m_Finalized)
                    return;

                IE_ASSERT(m_NumInitializedStaticSamplers == m_NumSamplers);

                D3D12_ROOT_SIGNATURE_DESC RootDesc;
                RootDesc.NumParameters = m_NumParameters;
                RootDesc.pParameters = (const D3D12_ROOT_PARAMETER*)m_ParamArray.get();
                RootDesc.NumStaticSamplers = m_NumSamplers;
                RootDesc.pStaticSamplers = (const D3D12_STATIC_SAMPLER_DESC*)m_SamplerArray.get();
                RootDesc.Flags = (D3D12_ROOT_SIGNATURE_FLAGS)Flags;

                m_DescriptorTableBitMap = 0;
                m_SamplerTableBitMap = 0;

                size_t HashCode = HashState(&RootDesc.Flags);
                HashCode = HashState(RootDesc.pStaticSamplers, m_NumSamplers, HashCode);

                for (UINT Param = 0; Param < m_NumParameters; ++Param)
                {
                    const D3D12_ROOT_PARAMETER& RootParam = RootDesc.pParameters[Param];
                    m_DescriptorTableSize[Param] = 0;

                    if (RootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
                    {
                        IE_ASSERT(RootParam.DescriptorTable.pDescriptorRanges != nullptr);

                        HashCode = HashState(RootParam.DescriptorTable.pDescriptorRanges,
                            RootParam.DescriptorTable.NumDescriptorRanges, HashCode);

                        // We keep track of sampler descriptor tables separately from CBV_SRV_UAV descriptor tables
                        if (RootParam.DescriptorTable.pDescriptorRanges->RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER)
                            m_SamplerTableBitMap |= (1 << Param);
                        else
                            m_DescriptorTableBitMap |= (1 << Param);

                        for (UINT TableRange = 0; TableRange < RootParam.DescriptorTable.NumDescriptorRanges; ++TableRange)
                            m_DescriptorTableSize[Param] += RootParam.DescriptorTable.pDescriptorRanges[TableRange].NumDescriptors;
                    }
                    else
                        HashCode = HashState(&RootParam, 1, HashCode);
                }

                ID3D12RootSignature** RSRef = nullptr;
                bool firstCompile = false;
                {
                    static std::mutex s_HashMapMutex;
                    std::lock_guard<std::mutex> CS(s_HashMapMutex);
                    auto iter = s_RootSignatureHashMap.find(HashCode);

                    // Reserve space so the next inquiry will find that someone got here first.
                    if (iter == s_RootSignatureHashMap.end())
                    {
                        RSRef = s_RootSignatureHashMap[HashCode].GetAddressOf();
                        firstCompile = true;
                    }
                    else
                        RSRef = iter->second.GetAddressOf();
                }

                if (firstCompile)
                {
                    ID3D12Device* pD3D12Device = RCast<ID3D12Device*>(g_pDevice->GetNativeDevice());
                    ::Microsoft::WRL::ComPtr<ID3DBlob> pOutBlob, pErrorBlob;

                    HRESULT hr = D3D12SerializeRootSignature(&RootDesc, D3D_ROOT_SIGNATURE_VERSION_1,
                        pOutBlob.GetAddressOf(), pErrorBlob.GetAddressOf());
                    if (pErrorBlob != NULL)
                    {
                        std::string err = (char*)pErrorBlob->GetBufferPointer();
                        IE_LOG(Error, TEXT("Error while compiling RootSignature: %s"), StringHelper::StringToWide(err).c_str())
                    }

                    ASSERT_SUCCEEDED(D3D12SerializeRootSignature(&RootDesc, D3D_ROOT_SIGNATURE_VERSION_1,
                        pOutBlob.GetAddressOf(), pErrorBlob.GetAddressOf()));

                    ASSERT_SUCCEEDED(pD3D12Device->CreateRootSignature(1, pOutBlob->GetBufferPointer(), pOutBlob->GetBufferSize(),
                        IID_PPV_ARGS(&m_pID3D12RootSignature)));

                    m_pID3D12RootSignature->SetName(name.c_str());

                    s_RootSignatureHashMap[HashCode].Attach(m_pID3D12RootSignature);
                    IE_ASSERT(*RSRef == m_pID3D12RootSignature);
                }
                else
                {
                    while (*RSRef == nullptr)
                        std::this_thread::yield();
                    m_pID3D12RootSignature = *RSRef;
                }

                m_Finalized = TRUE;
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
