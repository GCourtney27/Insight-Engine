#include <ie_pch.h>


#include "Insight/Rendering/Geometry/Mesh.h"
#include "Mesh_Node.h"
#include "Platform/DirectX12/Direct3D12_Context.h"
#include "imgui.h"

namespace Insight {
	


	void MeshNode::PreRender(XMMATRIX& parentMat, UINT32& gpuAddressOffset)
	{
		//UINT8* cbvGPUAddress = &Direct3D12Context::Get().GetConstantBufferViewGPUHeapAddress();
		auto worldMat = m_Transform.GetLocalMatrix() * parentMat;

		int numChildren = (int)m_Children.size();
		for (int i = 0; i < numChildren; ++i) {
			m_Children[i]->PreRender(worldMat, gpuAddressOffset);
		}

		int numMeshChildren = (int)m_MeshChildren.size();
		for (int i = 0; i < numMeshChildren; i++) {
			m_MeshChildren[i]->GetTransformRef().SetWorldMatrix(m_MeshChildren[i]->GetTransformRef().GetLocalMatrix() * worldMat);
		}

		//XMMATRIX viewMat = Direct3D12Context::Get().GetCamera().GetViewMatrix();
		//XMFLOAT4X4 viewFloatMat;
		//XMStoreFloat4x4(&viewFloatMat, XMMatrixTranspose(viewMat));
		//XMMATRIX projectionMat = Direct3D12Context::Get().GetCamera().GetProjectionMatrix();
		//XMFLOAT4X4 projectionFloatMat;
		//XMStoreFloat4x4(&projectionFloatMat, XMMatrixTranspose(projectionMat));

		//for (int i = 0; i < m_MeshChildren.size(); i++)
		//{
		//	m_MeshChildren[i]->GetTransformRef().GetLocalMatrixRef() = m_MeshChildren[i]->GetTransformRef().GetLocalMatrixRef() * worldMat;
		//	//m_MeshChildren[i]->ApplyWorldTransform(worldMat);
		//	CB_VS_PerObject cbPerObject = m_MeshChildren[i]->GetConstantBuffer();
		//	cbPerObject.view = viewFloatMat;
		//	cbPerObject.projection = projectionFloatMat;
		//	memcpy(cbvGPUAddress + (ConstantBufferPerObjectAlignedSize * gpuAddressOffset++), &cbPerObject, sizeof(cbPerObject));
		//}
	}

	void MeshNode::Render()
	{
		int numChildren = (int)m_MeshChildren.size();
		for (int i = 0; i < numChildren; ++i) {
			m_Children[i]->Render();
		}

		/*D3D12_GPU_VIRTUAL_ADDRESS cbvHandle(Direct3D12Context::Get().GetConstantBufferUploadHeap().GetGPUVirtualAddress());
		
		int numMeshChildren = m_MeshChildren.size();
		for (int i = 0; i < numMeshChildren; ++i) {
			Direct3D12Context::Get().GetCommandList().SetGraphicsRootConstantBufferView(0, cbvHandle + (ConstantBufferPerObjectAlignedSize * i));
			m_MeshChildren[i]->Render();
		}*/
	}

	void MeshNode::RenderSceneHeirarchy()
	{

		ImGuiTreeNodeFlags treeFlags = m_Children.empty() ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		const bool isExpanded = ImGui::TreeNodeEx(m_DisplayName.c_str(), treeFlags);

		if (ImGui::IsItemClicked()) {
			// TODO: Open material editor window
			IE_CORE_TRACE("Model node clicked");
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