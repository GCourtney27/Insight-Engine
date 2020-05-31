#pragma once
#include <DirectXMath.h>

namespace Insight {

	using DirectX::XMFLOAT3;
	using DirectX::XMFLOAT2;

	struct Vertex3D
	{
		XMFLOAT3 Position	 = {};
		XMFLOAT2 TexCoords	 = {};
		XMFLOAT3 Normal		 = {};
		XMFLOAT3 Tangent	 = {};
		XMFLOAT3 BiTangent	 = {};
	};

	struct ScreenSpaceVertex
	{
		XMFLOAT3 Position	 = {};
		XMFLOAT2 TexCoords	 = {};
	};

}
