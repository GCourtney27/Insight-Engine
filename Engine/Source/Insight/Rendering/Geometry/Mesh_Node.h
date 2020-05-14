#pragma once

#include <Insight/Core.h>

namespace Insight {

	class INSIGHT_API MeshNode
	{
	public:
		MeshNode(std::vector<Mesh*> meshChildren, Transform transform, std::string displayName = "Default Mesh Node")
			: m_MeshChildren(meshChildren), m_Transform(transform), m_DisplayName(displayName) {}
		~MeshNode() {}

		void RenderSceneHeirarchy();

		void AddChild(unique_ptr<MeshNode> child);

	private:
		std::vector<unique_ptr<MeshNode>> m_Children;
		std::vector<Mesh*> m_MeshChildren;
		Transform m_Transform;
		std::string m_DisplayName;
	};

}
