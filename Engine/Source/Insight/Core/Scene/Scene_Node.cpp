#include <ie_pch.h>

#include "Insight/Rendering/Rendering_Context.h"
#include "Insight/Runtime/AActor.h"
#include "imgui.h"
#include "Scene_Node.h"
#include "Insight/Core/Scene/Scene.h"

namespace Insight {



	SceneNode::SceneNode(std::string displayName)
		: m_DisplayName(displayName)
	{
	}

	SceneNode::~SceneNode()
	{
		Destroy();
	}

	void SceneNode::AddChild(SceneNode* childNode)
	{
		m_Children.push_back(childNode);
		childNode->SetParent(this);
	}

	void SceneNode::RenderSceneHeirarchy()
	{
		size_t numChildrenObjects = m_Children.size();
		if (numChildrenObjects == 0) {

		}
		else {
			for (size_t i = 0; i < numChildrenObjects; ++i) {
				m_Children[i]->RenderSceneHeirarchy();
			}
		}
	}

	bool SceneNode::OnInit()
	{
		return false;
	}

	bool SceneNode::OnPostInit()
	{
		return false;
	}

	void SceneNode::OnUpdate(const float& deltaMs)
	{
		for (auto i = m_Children.begin(); i != m_Children.end(); ++i) {
			(*i)->OnUpdate(deltaMs);
		}
	}

	void SceneNode::OnPreRender(XMMATRIX parentMat)
	{
		GetTransformRef().SetWorldMatrix(XMMatrixMultiply(parentMat, GetTransformRef().GetLocalMatrixRef()));
		for (auto i = m_Children.begin(); i != m_Children.end(); ++i) {
			(*i)->OnPreRender(GetTransformRef().GetWorldMatrixRef());
		}
	}

	void SceneNode::OnRender()
	{
		for (auto i = m_Children.begin(); i != m_Children.end(); ++i) {
			(*i)->OnRender();
		}
	}

	void SceneNode::BeginPlay()
	{
		for (auto i = m_Children.begin(); i != m_Children.end(); ++i) {
			(*i)->BeginPlay();
		}
	}

	void SceneNode::Tick(const float& deltaMs)
	{
	}

	void SceneNode::Exit()
	{
	}

	void SceneNode::Destroy()
	{
		size_t numChildrenObjects = m_Children.size();
		for (size_t i = 0; i < numChildrenObjects; ++i) {
			delete m_Children[i];
		}
	}

}
