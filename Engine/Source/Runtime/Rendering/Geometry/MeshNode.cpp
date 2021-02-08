#include <Engine_pch.h>


#include "Runtime/Rendering/Geometry/Mesh.h"
#include "MeshNode.h"

#include "Runtime/UI/UILib.h"

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

		UI::NodeFlags treeFlags = m_Children.empty() ? UI::TreeNode_Leaf : UI::TreeNode_OpenArrow | UI::TreeNode_OpenDoubleClick;
		const bool isExpanded = UI::TreeNodeEx(m_DisplayName.c_str(), treeFlags);

		if (UI::IsItemClicked()) {
			// TODO: Open material editor window
			IE_LOG(Verbose, "Model node clicked");
		}

		if (isExpanded) {
			int numChildren = (int)m_Children.size();
			for (int i = 0; i < numChildren; ++i) {
				m_Children[i]->RenderSceneHeirarchy();
			}
			UI::TreePopNode();
			UI::Spacing();
		}
	}
	
	void MeshNode::AddChild(unique_ptr<MeshNode> child)
	{
		IE_ASSERT(child, "Trying to add null node to children in Mesh Node!");
		m_Children.push_back(std::move(child));
	}

}