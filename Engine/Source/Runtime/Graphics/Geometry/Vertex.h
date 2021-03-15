#pragma once

#include <Runtime/Core.h>

namespace Insight {

	struct Vertex3D
	{
		FVector3 Position	 = {};
		FVector2 TexCoords	 = {};
		FVector3 Normal		 = {};
		FVector3 Tangent	 = {};
		FVector3 BiTangent	 = {};
	};

	struct ScreenSpaceVertex
	{
		FVector3 Position	 = {};
		FVector2 TexCoords	 = {};
	};


}
