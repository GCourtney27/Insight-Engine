#pragma once

#include <Runtime/Core.h>
#include <DirectXMath.h>

namespace Insight 
{
	/*
		Per-component matricies
	*/
	using ieFloat3x3 = DirectX::XMFLOAT3X3;
	using ieFloat4x4 = DirectX::XMFLOAT4X4;

	/*
		SIMD Matricies
	*/
	using ieMatrix = DirectX::XMMATRIX;
	using ieMatrix2x2 = DirectX::XMMATRIX;
	using ieMatrix3x3 = DirectX::XMMATRIX;
	using ieMatrix4x4 = DirectX::XMMATRIX;

}