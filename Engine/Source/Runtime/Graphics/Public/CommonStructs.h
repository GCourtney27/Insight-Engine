#pragma once

#include <Runtime/Core.h>

namespace Insight
{
	namespace Graphics
	{
		struct ViewPort
		{
			float TopLeftX;
			float TopLeftY;
			float Width;
			float Height;
			float MinDepth;
			float MaxDepth;
		};

		struct Rect
		{
			Int32 Left;
			Int32 Top;
			Int32 Right;
			Int32 Bottom;
		};

		struct SampleDesc
		{
			UInt32 Count;
			UInt32 Quality;
		};

		struct ResourceDesc
		{
			EResourceDimension Dimension;
			UInt64 Alignment;
			UInt64 Width;
			UInt32 Height;
			UInt16 DepthOrArraySize;
			UInt16 MipLevels;
			ETextureFormat Format;
			SampleDesc SampleDesc;
			ETextureLayout Layout;
			UInt32 Flags;
		};

		struct DepthStencilValue
		{
			float Depth;
			UInt8 Stencil;
		};
		
		struct ClearValue
		{
			float Color[4];
			DepthStencilValue DepthStencil;
		};
	}
}
