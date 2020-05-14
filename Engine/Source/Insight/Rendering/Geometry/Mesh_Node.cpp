#include <ie_pch.h>


#include "Insight/Rendering/Geometry/Mesh.h"
#include "Mesh_Node.h"
#include "imgui.h"

namespace Insight {
	


	void MeshNode::RenderSceneHeirarchy()
	{
		if (ImGui::TreeNode(m_DisplayName.c_str()))
		{
			int numChildren = m_Children.size();
			for (int i = 0; i < numChildren; ++i)
			{
				m_Children[i]->RenderSceneHeirarchy();
			}
			ImGui::TreePop();
			ImGui::Spacing();
		}
	}
	
	void MeshNode::AddChild(unique_ptr<MeshNode> child)
	{
		IE_ASSERT(child, "Trying to add null node to children in Mesh Node!");
		m_Children.push_back(std::move(child));
	}

}