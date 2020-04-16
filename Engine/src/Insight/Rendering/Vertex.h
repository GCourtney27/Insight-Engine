#pragma once
#include <DirectXMath.h>

namespace Insight {


	struct Vertex
	{
		Vertex() {}
		Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v)
			: Position(x, y, z), /*Normal(nx, ny, nz),*/ TexCoords(u, v) { }

		DirectX::XMFLOAT3 Position;
		//DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT2 TexCoords;
		/*DirectX::XMFLOAT3 Tangent;
		DirectX::XMFLOAT3 BiTangent;*/
	};
}
