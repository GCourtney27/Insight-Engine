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

		enum ETextureFormat
		{
			TF_R32G8X24_TYPELESS = 19,
			TF_R32_FLOAT_X8X24_TYPELESS = 21,

			TF_R8G8B8A8_TYPELESS = 27,
			TF_R8G8B8A8_UNORM = 28,
			TF_R32G32B32A32_UNORM,

		};//DXGI_FORMAT_R32G8X24_TYPELESS

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
	}
}


