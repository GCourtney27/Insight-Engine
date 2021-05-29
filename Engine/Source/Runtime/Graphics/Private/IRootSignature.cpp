#include <Engine_pch.h>

#include "Runtime/Graphics/Public/IRootSignature.h"

namespace Insight
{
	namespace Graphics
	{
        void IRootSignature::InitStaticSampler(
            UInt32 Register,
            const SamplerDesc& NonStaticSamplerDesc,
            EShaderVisibility Visibility)
        {
            IE_ASSERT(m_NumInitializedStaticSamplers < m_NumSamplers);
            StaticSamplerDesc& StaticSamplerDesc = m_SamplerArray[m_NumInitializedStaticSamplers++];

            StaticSamplerDesc.Filter = NonStaticSamplerDesc.Filter;
            StaticSamplerDesc.AddressU = NonStaticSamplerDesc.AddressU;
            StaticSamplerDesc.AddressV = NonStaticSamplerDesc.AddressV;
            StaticSamplerDesc.AddressW = NonStaticSamplerDesc.AddressW;
            StaticSamplerDesc.MipLODBias = NonStaticSamplerDesc.MipLODBias;
            StaticSamplerDesc.MaxAnisotropy = NonStaticSamplerDesc.MaxAnisotropy;
            StaticSamplerDesc.ComparisonFunc = NonStaticSamplerDesc.ComparisonFunc;
            StaticSamplerDesc.BorderColor = SBC_Opaque_White;
            StaticSamplerDesc.MinLOD = NonStaticSamplerDesc.MinLOD;
            StaticSamplerDesc.MaxLOD = NonStaticSamplerDesc.MaxLOD;
            StaticSamplerDesc.ShaderRegister = Register;
            StaticSamplerDesc.RegisterSpace = 0;
            StaticSamplerDesc.ShaderVisibility = Visibility;

            if (StaticSamplerDesc.AddressU == D3D12_TEXTURE_ADDRESS_MODE_BORDER ||
                StaticSamplerDesc.AddressV == D3D12_TEXTURE_ADDRESS_MODE_BORDER ||
                StaticSamplerDesc.AddressW == D3D12_TEXTURE_ADDRESS_MODE_BORDER)
            {
                if (
                    // Transparent Black
                    NonStaticSamplerDesc.BorderColor[0] == 0.0f &&
                    NonStaticSamplerDesc.BorderColor[1] == 0.0f &&
                    NonStaticSamplerDesc.BorderColor[2] == 0.0f &&
                    NonStaticSamplerDesc.BorderColor[3] == 0.0f ||
                    // Opaque Black
                    NonStaticSamplerDesc.BorderColor[0] == 0.0f &&
                    NonStaticSamplerDesc.BorderColor[1] == 0.0f &&
                    NonStaticSamplerDesc.BorderColor[2] == 0.0f &&
                    NonStaticSamplerDesc.BorderColor[3] == 1.0f ||
                    // Opaque White
                    NonStaticSamplerDesc.BorderColor[0] == 1.0f &&
                    NonStaticSamplerDesc.BorderColor[1] == 1.0f &&
                    NonStaticSamplerDesc.BorderColor[2] == 1.0f &&
                    NonStaticSamplerDesc.BorderColor[3] == 1.0f
                    )
                {
                    IE_LOG(Warning, TEXT("Sampler border color does not match static sampler limitations."));
                }

                if (NonStaticSamplerDesc.BorderColor[3] == 1.0f)
                {
                    if (NonStaticSamplerDesc.BorderColor[0] == 1.0f)
                        StaticSamplerDesc.BorderColor = SBC_Opaque_White;
                    else
                        StaticSamplerDesc.BorderColor = SBC_Opaque_Black;
                }
                else
                    StaticSamplerDesc.BorderColor = SBC_Transparent_Black;
            }
        }


	}
}
