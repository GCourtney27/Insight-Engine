#include <Engine_pch.h>

#include "Platform/Public/Utility/APIBridge/D3DUtility.h"

namespace Insight
{
	namespace PlatformUtils
	{


		// ----------------------------------
		//		Texture Type conversions
		// ----------------------------------

		DXGI_FORMAT IETextureFormatToDXGIFormat(const Graphics::ETextureFormat& Format)
		{
			switch (Format)
			{
			case Graphics::ETextureFormat::TF_R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
			default:
				IE_LOG(Error, TEXT("Invalid format trying to convert ETextureFormat to DXGI_FORMAT!"));
				return DXGI_FORMAT_B8G8R8A8_TYPELESS;
			}

		}

		Graphics::ETextureFormat DXGIFormatToIETextureFormat(DXGI_FORMAT Format)
		{
			switch (Format)
			{
			case DXGI_FORMAT_R8G8B8A8_UNORM: return Graphics::ETextureFormat::TF_R8G8B8A8_UNORM;
			default:
				IE_LOG(Error, TEXT("Invalid format trying to convert DXGI_FORMAT to ETextureFormat!"));
				return Graphics::ETextureFormat::TF_R8G8B8A8_UNORM;
			}
		}


		// -------------------------------------
		//	D3D12 Command List Type Conversions
		// -------------------------------------

		D3D12_COMMAND_LIST_TYPE IECommandListTypeToD3DCommandListType(const Graphics::ECommandListType& Type)
		{
			switch (Type)
			{
			case Graphics::ECommandListType::CLT_Direct: return D3D12_COMMAND_LIST_TYPE_DIRECT;
			case Graphics::ECommandListType::CLT_Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
			default:
				IE_LOG(Error, TEXT("Failed to convert ECommandListType to D3D12_COMMAND_LIST_TYPE with specified enum value!"));
				return D3D12_COMMAND_LIST_TYPE_DIRECT;
			}
		}

		Graphics::ECommandListType D3DCommandListTypeToIECommandListType(D3D12_COMMAND_LIST_TYPE Type)
		{
			switch (Type)
			{
			case D3D12_COMMAND_LIST_TYPE_DIRECT: return Graphics::ECommandListType::CLT_Direct;
			case D3D12_COMMAND_LIST_TYPE_COMPUTE: return Graphics::ECommandListType::CLT_Compute;
			default:
				IE_LOG(Error, TEXT("Failed to convert D3D12_COMMAND_LIST_TYPE to ECommandListType with specified enum value!"));
				return Graphics::ECommandListType::CLT_Direct;
			}
		}
	}
}
