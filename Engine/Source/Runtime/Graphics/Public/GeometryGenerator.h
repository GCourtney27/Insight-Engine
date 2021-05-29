#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/ResourceManagement/ModelManager.h"

namespace Insight
{
	namespace GeometryGenerator
	{
		/*
			Generates a 2D screen space quad and returns a reference to it.
		*/
		StaticMeshGeometryRef GenerateScreenAlignedQuadMesh();

		//StaticMeshGeometryRef GenerateCubeMesh();
	}
}
