#pragma once

#include "EngineDefines.h"

#include "Graphics/Public/ResourceManagement/ModelManager.h"

namespace Insight
{
	namespace GeometryGenerator
	{
		/*
			Generates a 2D screen space quad and returns a reference to it.
		*/
		StaticMeshGeometryRef GenerateScreenAlignedQuadMesh();
		StaticMeshGeometryRef GenerateSphere(UInt32 Radius, UInt32 Slices, UInt32 Segments);
		//StaticMeshGeometryRef Generate1x1x1CubeMesh();
	}
}
