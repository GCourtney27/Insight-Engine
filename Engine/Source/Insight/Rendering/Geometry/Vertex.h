#pragma once
#include <DirectXMath.h>

namespace Insight {


	struct Vertex
	{
		Vertex() {}
		Vertex(float x, float y, float z, float u, float v, float nx = 0.0f, float ny = 0.0f, float nz = 0.0f)
			: Position(x, y, z), TexCoords(u, v), Normal(nx, ny, nz) { }

		DirectX::XMFLOAT3 Position = {};
		DirectX::XMFLOAT2 TexCoords = {};
		DirectX::XMFLOAT3 Normal = {};
		DirectX::XMFLOAT3 Tangent = {};
		DirectX::XMFLOAT3 BiTangent = {};
	};
}
