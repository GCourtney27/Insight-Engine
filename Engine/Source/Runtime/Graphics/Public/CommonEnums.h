#pragma once

namespace Insight
{
	namespace Graphics
	{
		enum ECommandListType
		{
			CLT_Direct = 0x00,
			CLT_Compute = 0x01,
			CLT_Copy = 0x02,
			CLT_Bundle = 0x04,
		};

		enum EFormat
		{
			F_Unknown = 0,
			F_R32G32B32A32_Float = 2,
			F_R32G32B32_Float = 6,
			F_R32G8X24_Typeless = 19,
			F_R32G32_Float = 16,
			F_D32_Float_S8X24_Typeless = 20,
			F_R32_Float_X8X24_Typeless = 21,
			F_X32_Typeless_G8X24_UInt = 22,
			F_R8G8B8A8_Typeless = 27,
			F_R8G8B8A8_UNorm = 28,
			F_R8G8B8A8_UNorm_SRGB = 29,
			F_R32_Typeless = 39,
			F_D32_Float = 40,
			F_R32_Float = 41,
			F_R32_UINT = 42,

			F_R24G8_Typeless = 44,
			R_D24_UNorm_S8_UInt = 45,
			F_R24_UNorm_X8_Typeless = 46,
			F_X24_Typeless_G8_UInt = 47,

			F_R16_Typeless = 53,
			F_D16_UNorm = 55,
			F_R16_UNorm = 56,

			F_B8G8R8A8_UNorm = 87,
			F_B8G8R8X8_UNorm = 88,
			F_B8G8R8X8_UNorm_SRGB = 93,
			F_B8G8R8X8_UNorm_Typeless = 92,
			F_B8G8R8A8_UNorm_Typeless = 90,
			F_B8G8R8A8_UNorm_SRGB = 91,

		};

		enum EResourceState
		{
			RS_Common = 0,
			RS_VertexAndConstantBuffer = 0x01,
			RS_IndexBuffer = 0x02,
			RS_RenderTarget = 0x04,
			RS_UnorderedAccess = 0x08,
			RS_DepthWrite = 0x10,
			RS_DepthRead = 0x20,
			RS_NonPixelShaderResource = 0x40,
			RS_PixelShaderResource = 0x80,
			RS_CopyDestination = 0x400,

			RS_Present = 0,
		};
#		define RESOURCE_STATE_INVALID		((EResourceState)-1)

		enum EResourceHeapType
		{
			RHT_CBV_SRV_UAV = 0,
			RHT_Sampler = (RHT_CBV_SRV_UAV + 1),
			RHT_RTV = (RHT_Sampler + 1),
			RHT_DSV = (RHT_RTV + 1),
			RHT_HeapType_Count = (RHT_DSV + 1),
		};

		enum EResourceDimension
		{
			RD_Unknown = 0,
			RD_Buffer = 1,
			RD_Texture_1D = 2,
			RD_Texture_2D = 3,
			RD_Texture_3D = 4,
		};

		enum ETextureLayout
		{
			TL_Unknown = 0,
			TL_Row_Major = 1,
			TL_64k_Undefined_Swizzle = 2,
			TL_64k_Standard_Swizzle = 3,
		};

		enum EResourceFlags
		{
			RF_None = 0,
			RF_AllowRenderTarget = 0x1,
			RF_AllowDepthStencil = 0x2,
			RF_AllowUnorderedAccess = 0x4,
			RF_DenyShaderResource = 0x8,
		};
		IE_DEFINE_ENUM_FLAG_OPERATORS(EResourceFlags);


		enum EBlend
		{
			B_Zero = 1,
			B_One = 2,
			B_SourceColor = 3,
			B_InvSourceColor = 4,
			B_SourceAlpha = 5,
			B_InvSourceAlpha = 6,
			B_DestAlpha = 7,
			B_InvDestAlpha = 8,
			B_DestColor = 9,
			B_InvDestColor = 10,
			B_SrcAlphaSat = 11,
			B_BlendFactor = 14,
			B_InvBlendFactor = 15,
			B_Src1Color = 16,
			B_InvSrc1Color = 17,
			B_Src1Alpha = 18,
			B_InvSrc1Alpha = 19,

		};

		enum ECullMode
		{
			CM_None = 1,
			CM_Front = 2,
			CM_Back = 3
		};

		enum EIndexBufferStripCutValue
		{
			IBSCV_Disabled = 0,
			IBSCV_0xFFFF = 1,
			IBSCV_0xFFFFFFFF = 2,
		};

		enum EPrimitiveTopologyType
		{
			PTT_Undefined = 0,
			PTT_Point = 1,
			PTT_Line = 2,
			PTT_Triangle = 3,
			PTT_Patch = 4,
		};

		enum EPrimitiveTopology
		{
			PT_Undefined = 0,
			PT_Pointlist = 1,
			PT_Linelist = 2,
			PT_Linestrip = 3,
			PT_TiangleList = 4,
		};

		enum EPipelineStateFlags
		{
			PSF_None = 0,
			PSF_ToolDebug = 0x1,
		};

		enum EInputClassification
		{
			IC_PerVertexData = 0,
			IC_PerInstanceData = 0,
		};

		enum EBlendOp
		{
			BO_Add = 1,
			BO_Subtract = 2,
			BO_ReverseSubtract = 3,
			BO_Min = 4,
			BO_Max = 5,
		};

		enum EFillMode
		{
			FM_WireFrame = 2,
			FM_Solid = 3,
		};

		enum EConseritiveRasterMode
		{
			CRM_Off = 0,
			CRM_On = 1,
		};

		enum EDepthWriteMask
		{
			DWM_Zero = 0,
			DWM_All = 1,
		};

		enum EComparisonFunc
		{
			CF_Never = 1,
			CF_Less = 2,
			CF_Equal = 3,
			CF_LessEqual = 4,
			CF_Greater = 5,
			CF_NotEqual = 6,
			CF_GreaterEqual = 7,
			CF_Allways = 8,
		};

		enum ELogicOp
		{
			LO_Clear = 0,
			LO_Set = (LO_Clear + 1),
			LO_Copy = (LO_Set + 1),
			LO_Copy_Inverted = (LO_Copy + 1),
			LO_NoOp = (LO_Copy_Inverted + 1),
			LO_Invert = (LO_NoOp + 1),
			LO_And = (LO_Invert + 1),
			LO_Nand = (LO_And + 1),
			LO_Or = (LO_Nand + 1),
			LO_NOr = (LO_Or + 1),
			LO_XOr = (LO_NOr + 1),
			LO_Equiv = (LO_XOr + 1),
			LO_AndReverse = (LO_Equiv + 1),
			LO_AndInverted = (LO_AndReverse + 1),
			LO_OrReverse = (LO_AndInverted + 1),
			LO_OR_Inverted = (LO_OrReverse + 1)
		};

		enum EColorWriteEnable
		{
			CWE_Red = 1,
			CWE_Green = 2,
			CWE_Blue = 4,
			CWE_Alpha = 8,
			CWE_All = (((CWE_Red | CWE_Green) | CWE_Blue) | CWE_Alpha)
		};

		enum EStencilOp
		{
			SO_Keep = 1,
			SO_Zero = 2,
			SO_Replace = 3,
			SO_IncSat = 4,
			SO_DecrSat = 5,
			SO_Invert = 6,
			SO_Incr = 7,
			SO_Decr = 8,
		};

		enum ERootSignatureFlags
		{
			RSF_None = 0,
			RSF_AllowInputAssemblerLayout = 0x1,
			RSF_DenyVertexShaderRootAccess = 0x2,
			RSF_DenyHullShaderRootAccess = 0x4,
			RSF_DenyDomainShaderRootAccess = 0x8,
			RSF_DenyGeometryShaderRootAccess = 0x10,
			RSF_DenyPixelShaderRootAccess = 0x20,
			RSF_AllowStreamOutput = 0x40,
			RSF_LOCALRootSignature = 0x80,
			RSF_DenyAmplificationShaderRootAccess = 0x100,
			RSF_DenyMeshShaderRootAccess = 0x200
		};
		IE_DEFINE_ENUM_FLAG_OPERATORS(ERootSignatureFlags);

		enum EFilter
		{
			F_Min_Mag_Mip_Point = 0,
			F_Min_Mag_Point_Mip_Linear = 0x1,
			F_Min_Point_Mag_Linear_Mip_Point = 0x4,
			F_Min_Point_Mag_Mip_Linear = 0x5,
			F_Min_Linear_Mag_Mip_Point = 0x10,
			F_Min_Linear_Mag_Point_Mip_Linear = 0x11,
			F_Min_Mag_Linear_Mip_Point = 0x14,
			F_Min_Mag_Mip_Linear = 0x15,
			F_Anisotropic = 0x55,
			F_Comparison_Min_Mag_Mip_Point = 0x80,
			F_Comparison_Min_Mag_Point_Mip_Linear = 0x81,
			F_Comparison_Min_Point_Mag_Linear_Mip_Point = 0x84,
			F_Comparison_Min_Point_Mag_Mip_Linear = 0x85,
			F_Comparison_Min_Linear_Mag_Mip_Point = 0x90,
			F_Comparison_Min_Linear_Mag_Point_Mip_Linear = 0x91,
			F_Comparison_Min_Mag_Linear_Mip_Point = 0x94,
			F_Comparison_Min_Mag_Mip_Linear = 0x95,
			F_Comparison_Anisotropic = 0xd5,
			F_Minimum_Min_Mag_Mip_Point = 0x100,
			F_Minimum_Min_Mag_Point_Mip_Linear = 0x101,
			F_Minimum_Min_Point_Mag_Linear_Mip_Point = 0x104,
			F_Minimum_Min_Point_Mag_Mip_Linear = 0x105,
			F_Minimum_Min_Linear_Mag_Mip_Point = 0x110,
			F_Minimum_Min_Linear_Mag_Point_Mip_Linear = 0x111,
			F_Minimum_Min_Mag_Linear_Mip_Point = 0x114,
			F_Minimum_Min_Mag_Mip_Linear = 0x115,
			F_Minimum_Anisotropic = 0x155,
			F_Maximum_Min_Mag_Mip_Point = 0x180,
			F_Maximum_Min_Mag_Point_Mip_Linear = 0x181,
			F_Maximum_Min_Point_Mag_Linear_Mip_Point = 0x184,
			F_Maximum_Min_Point_Mag_Mip_Linear = 0x185,
			F_Maximum_Min_Linear_Mag_Mip_Point = 0x190,
			F_Maximum_Min_Linear_Mag_Point_Mip_Linear = 0x191,
			F_Maximum_Min_Mag_Linear_Mip_Point = 0x194,
			F_Maximum_Min_Mag_Mip_Linear = 0x195,
			F_Maximum_Anisotropic = 0x1d5
		};

		enum ETextureAddressMode
		{
			TAM_Wrap = 1,
			TAM_Mirror = 2,
			TAM_Clamp = 3,
			TAM_Border = 4,
			TAM_MirrorOnce = 5,
		};

		enum EStaticBorderColor
		{
			SBC_Transparent_Black = 0,
			SBC_Opaque_Black = (SBC_Transparent_Black + 1),
			SBC_Opaque_White = (SBC_Opaque_Black + 1),
		};

		enum ERootParameterType
		{
			RPT_DescriptorTable = 0,
			RPT_32BitConstants = (RPT_DescriptorTable + 1),
			RPT_ConstantBufferView = (RPT_32BitConstants + 1),
			RPT_ShaderResourceView = (RPT_ConstantBufferView + 1),
			RPT_UnorderedAccessView = (RPT_ShaderResourceView + 1),
		};

		enum EShaderVisibility
		{
			SV_All = 0,
			SV_Vertex = 1,
			SV_Hull = 2,
			SV_Domain = 3,
			SV_Geometry = 4,
			SV_Pixel = 5,
			SV_Amplification = 6,
			SV_Mesh,
		};

		enum EDescriptorRangeType
		{
			DRT_ShaderResourceView = 0,
			DRT_UnorderedAccessView = (DRT_ShaderResourceView + 1),
			DRT_ConstantBufferView = (DRT_UnorderedAccessView + 1),
			DRT_Sampler = (DRT_ConstantBufferView + 1),
		};
	}
}


