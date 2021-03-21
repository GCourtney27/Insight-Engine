#pragma once

#include <Runtime/Core.h>

namespace Insight
{
	namespace Graphics
	{
		enum IETextureFormat
		{
			TF_RGB8_UNORM,
			TF_RGB32_UNORM,
		};

		struct IESampleDesc
		{
			UInt32 Count;
			UInt32 Quality;
		};

		class INSIGHT_API ITexture
		{
		public:

		protected:
			ITexture() {}
			virtual ~ITexture() {}
		};

		namespace PlatformHelpers
		{
#		if IE_PLATFORM_WINDOWS
			inline DXGI_FORMAT IEFormatToDXGIFormat(const IETextureFormat& Format)
			{
				switch (Format)
				{
				case IETextureFormat::TF_RGB8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
				default:
					IE_LOG(Error, TEXT("Invalid format trying to convert IETextureFormat to DXGI_FORMAT!"));
					break;
				}

				return DXGI_FORMAT_B8G8R8A8_TYPELESS;
			}
#		endif
		}
	}
}