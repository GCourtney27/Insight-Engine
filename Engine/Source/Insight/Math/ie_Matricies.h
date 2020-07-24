#pragma once

#include <Insight/Core.h>
#include <DirectXMath.h>

namespace Insight {

	namespace Math {

		/*
			Per-component matricies
		*/
#if defined IE_PLATFORM_WINDOWS
		using ieFloat3x3 = DirectX::XMFLOAT3X3;
		using ieFloat4x4 = DirectX::XMFLOAT4X4;
#elif defined IE_PLATFORM_MAC
		using ieFloat2x2 = glm::mat2x2;
		using ieFloat3x3 = glm::mat3x3;
		using ieFloat4x4 = glm::mat4x4;
#endif
	}

	namespace Math {

		/*
			SIMD Matricies
		*/
#if defined IE_PLATFORM_WINDOWS
		using ieMatrix = DirectX::XMMATRIX;
		using ieMatrix2x2 = DirectX::XMMATRIX;
		using ieMatrix3x3 = DirectX::XMMATRIX;
		using ieMatrix4x4 = DirectX::XMMATRIX;
#elif defined IE_PLATFORM_MAC
		using ieMatrix2x2 = glm::mat2x2;
		using ieMatrix3x3 = glm::mat3x3;
		using ieMatrix4x4 = glm::mat4x4;
#endif

	}

}