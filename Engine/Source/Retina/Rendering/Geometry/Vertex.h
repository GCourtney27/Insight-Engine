#pragma once
#include "Retina/Math/ie_Vectors.h"

namespace Retina {

	using Math::ieFloat2;
	using Math::ieFloat3;

	struct Vertex3D
	{
		ieFloat3 Position	 = {};
		ieFloat2 TexCoords	 = {};
		ieFloat3 Normal		 = {};
		ieFloat3 Tangent	 = {};
		ieFloat3 BiTangent	 = {};
	};

	struct ScreenSpaceVertex
	{
		ieFloat3 Position	 = {};
		ieFloat2 TexCoords	 = {};
	};


}
