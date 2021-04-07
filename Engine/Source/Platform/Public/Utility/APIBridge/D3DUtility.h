#pragma once

#include <Runtime/Core.h>

#include "Platform/Public/Utility/COMException.h"
#include "Runtime/Graphics/Public/CommonEnums.h"

namespace Insight
{
	namespace PlatformUtils
	{
#if IE_PLATFORM_WINDOWS
		DXGI_FORMAT IETextureFormatToDXGIFormat(const Graphics::ETextureFormat& Format);
		Graphics::ETextureFormat DXGIFormatToIETextureFormat(DXGI_FORMAT Format);

		D3D12_COMMAND_LIST_TYPE IECommandListTypeToD3DCommandListType(const Graphics::ECommandListType& Type);
		Graphics::ECommandListType D3DCommandListTypeToIECommandListType(D3D12_COMMAND_LIST_TYPE Type);


#endif
	}
}
