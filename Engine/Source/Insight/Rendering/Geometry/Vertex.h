#pragma once
#include <DirectXMath.h>

namespace Insight {

	using DirectX::XMFLOAT3;
	using DirectX::XMFLOAT2;

	struct Vertex3D
	{
		Vertex3D() {}
		virtual ~Vertex3D() {}

		Vertex3D(XMFLOAT3 position, XMFLOAT3 normal, XMFLOAT2 texCoords)
			: Position(position), Normal(normal), TexCoords(texCoords) { }

		Vertex3D(float x, float y, float z, float u, float v, float nx = 0.0f, float ny = 0.0f, float nz = 0.0f)
			: Position(x, y, z), TexCoords(u, v), Normal(nx, ny, nz) { }

		XMFLOAT3 Position = {};
		XMFLOAT2 TexCoords = {};
		XMFLOAT3 Normal = {};
		XMFLOAT3 Tangent = {};
		XMFLOAT3 BiTangent = {};
	};

	struct ScreenSpaceVertex
	{
		/*ScreenSpaceVertex() {}
		virtual ~ScreenSpaceVertex() {}

		ScreenSpaceVertex(XMFLOAT3 position, XMFLOAT2 texCoords)
			: Position(position), TexCoords(texCoords) { }

		ScreenSpaceVertex(float x, float y, float z, float u, float v)
			: Position(x, y, z), TexCoords(u, v) { }*/

		XMFLOAT3 Position = {};
		XMFLOAT2 TexCoords = {};
	};

}
