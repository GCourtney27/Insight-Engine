#include <Engine_pch.h>

#include "Platform/DirectX12/Public/D3D12PipelineState.h"
#include "Platform/Public/Utility/COMException.h"

#include "Runtime/Graphics/Public/GraphicsCore.h"
#include "Runtime/Graphics/Public/IDevice.h"
#include "Runtime/Graphics/Public/IRootSignature.h"

namespace Insight
{
	namespace Graphics
	{
		namespace DX12
		{
			D3D12PipelineState::D3D12PipelineState()
				: m_pID3D12PipelineState(NULL)
			{
			}

			D3D12PipelineState::~D3D12PipelineState()
			{
				COM_SAFE_RELEASE(m_pID3D12PipelineState);
			}
			
			void D3D12PipelineState::Initialize(const PipelineStateDesc& Desc)
			{
				ID3D12Device* pID3D12Device = RCast<ID3D12Device*>(g_pDevice->GetNativeDevice());
				
				D3D12_GRAPHICS_PIPELINE_STATE_DESC PSODesc = {};
				PSODesc.NodeMask = Desc.NodeMask;

				PSODesc.PS.pShaderBytecode = Desc.PixelShader.pShaderByteCode;
				PSODesc.PS.BytecodeLength = Desc.PixelShader.ByteCodeLength;
				PSODesc.VS.pShaderBytecode = Desc.VertexShader.pShaderByteCode;
				PSODesc.VS.BytecodeLength = Desc.VertexShader.ByteCodeLength;

				PSODesc.BlendState.AlphaToCoverageEnable = Desc.BlendState.AlphaToCoverageEnable;
				PSODesc.BlendState.IndependentBlendEnable = Desc.BlendState.IndependentBlendEnable;
				for (UInt32 i = 0; i < 8; ++i)
				{
					auto& D3Drtv = PSODesc.BlendState.RenderTarget[i];
					auto& rtv = Desc.BlendState.RenderTarget[i];
					D3Drtv.BlendEnable = rtv.BlendEnable;
					D3Drtv.LogicOpEnable = rtv.LogicOpEnable;
					D3Drtv.SrcBlend = (D3D12_BLEND)rtv.SourceBlend;
					D3Drtv.DestBlend = (D3D12_BLEND)rtv.DestBlend;
					D3Drtv.BlendOp = (D3D12_BLEND_OP)rtv.BlendOp;
					D3Drtv.SrcBlendAlpha = (D3D12_BLEND)rtv.SourceBlendAlpha;
					D3Drtv.DestBlendAlpha = (D3D12_BLEND)rtv.DestBlendAlpha;
					D3Drtv.BlendOpAlpha = (D3D12_BLEND_OP)rtv.BlendOpAlpha;
					D3Drtv.LogicOp = (D3D12_LOGIC_OP)rtv.LocigOp;
					D3Drtv.RenderTargetWriteMask = rtv.RenderTargetWriteMask;
				}
				
				std::vector<D3D12_INPUT_ELEMENT_DESC> InputElementDescs;
				PSODesc.InputLayout.NumElements = Desc.InputLayout.NumElements;
				for (UInt32 i = 0; i < Desc.InputLayout.NumElements; ++i)
				{
					D3D12_INPUT_ELEMENT_DESC desc = {
						(LPCSTR)Desc.InputLayout.pInputElementDescs[i].SemanticName,
						Desc.InputLayout.pInputElementDescs[i].SemanticIndex,
						(DXGI_FORMAT)Desc.InputLayout.pInputElementDescs[i].Format,
						Desc.InputLayout.pInputElementDescs[i].InputSlot,
						Desc.InputLayout.pInputElementDescs[i].AlignedByteOffset,
						(D3D12_INPUT_CLASSIFICATION)Desc.InputLayout.pInputElementDescs[i].InputSlotClass,
						Desc.InputLayout.pInputElementDescs[i].InstanceDataStepRate
					};
					InputElementDescs.push_back(desc);
				}

				PSODesc.InputLayout.pInputElementDescs = RCast<const D3D12_INPUT_ELEMENT_DESC*>(InputElementDescs.data());
				PSODesc.pRootSignature = RCast<ID3D12RootSignature*>(Desc.pRootSignature->GetNativeSignature());
				
				auto& D3Ddss = PSODesc.DepthStencilState;
				auto& dss = Desc.DepthStencilState;
				D3Ddss.DepthEnable = dss.DepthEnable;
				D3Ddss.DepthWriteMask = (D3D12_DEPTH_WRITE_MASK)dss.DepthWriteMask;
				D3Ddss.DepthFunc = (D3D12_COMPARISON_FUNC)dss.DepthFunc;
				D3Ddss.StencilEnable = dss.StencilEnabled;
				D3Ddss.StencilReadMask = dss.StencilReadMask;
				D3Ddss.StencilWriteMask = dss.StencilWriteMask;
				
				D3Ddss.BackFace.StencilDepthFailOp = (D3D12_STENCIL_OP)dss.BackFace.StencilDepthFailOp;
				D3Ddss.BackFace.StencilFailOp = (D3D12_STENCIL_OP)dss.BackFace.StencilFailOp;
				D3Ddss.BackFace.StencilFunc = (D3D12_COMPARISON_FUNC)dss.BackFace.StencilFunc;
				D3Ddss.BackFace.StencilPassOp = (D3D12_STENCIL_OP)dss.BackFace.StencilPassOp;

				D3Ddss.FrontFace.StencilDepthFailOp = (D3D12_STENCIL_OP)dss.FrontFace.StencilDepthFailOp;
				D3Ddss.FrontFace.StencilFailOp = (D3D12_STENCIL_OP)dss.FrontFace.StencilFailOp;
				D3Ddss.FrontFace.StencilFunc = (D3D12_COMPARISON_FUNC)dss.FrontFace.StencilFunc;
				D3Ddss.FrontFace.StencilPassOp = (D3D12_STENCIL_OP)dss.FrontFace.StencilPassOp;

				PSODesc.RasterizerState.AntialiasedLineEnable = Desc.RasterizerDesc.AntialiazedLineEnabled;
				PSODesc.RasterizerState.ConservativeRaster = (D3D12_CONSERVATIVE_RASTERIZATION_MODE)Desc.RasterizerDesc.ConservativeRaster;
				PSODesc.RasterizerState.CullMode = (D3D12_CULL_MODE)Desc.RasterizerDesc.CullMode;
				PSODesc.RasterizerState.DepthBias = Desc.RasterizerDesc.DepthBias;
				PSODesc.RasterizerState.DepthBiasClamp = Desc.RasterizerDesc.DepthBiasClamp;
				PSODesc.RasterizerState.DepthClipEnable = Desc.RasterizerDesc.DepthClipEnabled;
				PSODesc.RasterizerState.FillMode = (D3D12_FILL_MODE)Desc.RasterizerDesc.FillMode;
				PSODesc.RasterizerState.ForcedSampleCount = Desc.RasterizerDesc.ForcedSampleCount;
				PSODesc.RasterizerState.FrontCounterClockwise = Desc.RasterizerDesc.FrontCounterClockwise;
				PSODesc.RasterizerState.MultisampleEnable = Desc.RasterizerDesc.MultiSampleEnable;
				PSODesc.RasterizerState.SlopeScaledDepthBias = Desc.RasterizerDesc.SlopeScaledDepthBias;

				PSODesc.SampleMask = Desc.SampleMask;
				PSODesc.PrimitiveTopologyType = (D3D12_PRIMITIVE_TOPOLOGY_TYPE)Desc.PrimitiveTopologyType;
				PSODesc.NumRenderTargets = Desc.NumRenderTargets;
				for (UInt32 i = 0; i < Desc.NumRenderTargets; ++i)
					PSODesc.RTVFormats[i] = (DXGI_FORMAT)Desc.RTVFormats[i];
				PSODesc.DSVFormat = (DXGI_FORMAT)Desc.DSVFormat;
				PSODesc.SampleDesc.Count = Desc.SampleDesc.Count;
				PSODesc.SampleDesc.Quality = Desc.SampleDesc.Quality;

				HRESULT hr = pID3D12Device->CreateGraphicsPipelineState(&PSODesc, IID_PPV_ARGS(&m_pID3D12PipelineState));
				ThrowIfFailed(hr, TEXT("Failed to create D3D12 pipeline state!"));
			}
		}
	}
}
