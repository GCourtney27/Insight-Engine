#pragma once

#include <Runtime/Core.h>
#include "DirectX12/TK/Inc/SimpleMath.h"


namespace Insight 
{
	/*
		Per-component matricies
	*/
	using Float3x3 = DirectX::XMFLOAT3X3;
	using Float4x4 = DirectX::XMFLOAT4X4;

	using FMatrix = ::DirectX::SimpleMath::Matrix;


}