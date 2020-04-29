#pragma once

#include "Insight/Core.h"

#include "Insight/Rendering/Geometry/Mesh.h"

namespace Insight {

	class MeshManager
	{
	public:
		MeshManager() {}
		~MeshManager() { }

		bool LoadMeshFromFile(const std::string& filePath);


	private:
		std::vector< std::shared_ptr<Mesh> > m_Meshes;

	};

}
