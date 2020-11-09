#include <Engine_pch.h>


#include "Insight/Rendering/Geometry/Mesh.h"
#include "Mesh_Node.h"
#include "imgui.h"

namespace Insight {
	


	void MeshNode::PreRender(XMMATRIX& parentMat, UINT32& gpuAddressOffset)
	{
		auto worldMat = XMMatrixMultiply(m_Transform.GetLocalMatrix(), parentMat);

		int numChildren = (int)m_Children.size();
		for (int i = 0; i < numChildren; ++i) {
			m_Children[i]->PreRender(worldMat, gpuAddressOffset);
		}

		int numMeshChildren = (int)m_MeshChildren.size();
		for (int i = 0; i < numMeshChildren; i++) {
			m_MeshChildren[i]->GetTransformRef().SetWorldMatrix(m_MeshChildren[i]->GetTransformRef().GetLocalMatrix() * worldMat);
		}
	}

	void MeshNode::Render()
	{
		int numChildren = (int)m_MeshChildren.size();
		for (int i = 0; i < numChildren; ++i) {
			m_Children[i]->Render();
		}
	}

	void MeshNode::RenderSceneHeirarchy()
	{

		ImGuiTreeNodeFlags treeFlags = m_Children.empty() ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		const bool isExpanded = ImGui::TreeNodeEx(m_DisplayName.c_str(), treeFlags);

		if (ImGui::IsItemClicked()) {
			// TODO: Open material editor window
			IE_DEBUG_LOG(LogSeverity::Verbose, "Model node clicked");
		}

		if (isExpanded) {
			int numChildren = (int)m_Children.size();
			for (int i = 0; i < numChildren; ++i) {
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