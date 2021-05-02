#pragma once

#include <Runtime/Core.h>
#include <Runtime/Graphics/Public/GraphicsCore.h>


namespace Insight
{
	namespace Graphics
	{
		namespace CommonStructHelpers
		{
			struct CDepthStencilStateDesc : public DepthStencilStateDesc
			{
				CDepthStencilStateDesc()
				{
					DepthEnable = true;
					DepthWriteMask = DWM_All;
					DepthFunc = CF_Less;
					StencilEnabled = false;
					StencilReadMask = IE_DEFAULT_STENCIL_READ_MASK;
					StencilWriteMask = IE_DEFAULT_STENCIL_WRITE_MASK;
					const StencilOpDesc DefaultStencilOp =
					{ SO_Keep, SO_Keep, SO_Keep, CF_Always };
					FrontFace = DefaultStencilOp;
					BackFace = DefaultStencilOp;
				}
			};

			struct CBlendDesc : public BlendDesc
			{
				CBlendDesc()
				{
					AlphaToCoverageEnable = false;
					IndependentBlendEnable = false;
					
					RenderTargetBlendDesc DefaultRenderTargetBlendDesc =
					{
						false, false,
						B_One, B_Zero, BO_Add,
						B_One, B_Zero, BO_Add,
						LO_NoOp,
						CWE_All,
					};
					for (UInt32 i = 0; i < IE_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
						RenderTarget[i] = DefaultRenderTargetBlendDesc;
				}
			};

			struct CRasterizerDesc : public RasterizerDesc
			{
				CRasterizerDesc()
				{
					FillMode = FM_Solid;
					CullMode = CM_Back;
					FrontCounterClockwise = false;
					DepthBias = IE_DEFAULT_DEPTH_BIAS;
					DepthBiasClamp = IE_DEFAULT_DEPTH_BIAS_CLAMP;
					SlopeScaledDepthBias = IE_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
					DepthClipEnabled = true;
					MultiSampleEnable = false;
					AntialiazedLineEnabled = false;
					ForcedSampleCount = 0;
					ConservativeRaster = CRM_Off;
				}
			};
		}
	}
}
