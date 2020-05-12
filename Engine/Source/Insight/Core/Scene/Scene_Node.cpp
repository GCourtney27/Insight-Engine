#include "ie_pch.h"

#include "Insight/Rendering/Rendering_Context.h"
#include "Insight/Runtime/AActor.h"
#include "Scene_Node.h"

namespace Insight {



	SceneNode::SceneNode(AActor* actor)
		: m_Actor(actor)
	{
	}

	SceneNode::~SceneNode()
	{
		size_t numChildrenObjects = m_Children.size();
		for (size_t i = 0; i < numChildrenObjects; ++i)
		{
			delete m_Children[i];
		}
	}

	void SceneNode::AddChild(SceneNode* childNode)
	{
		m_Children.push_back(childNode);
		childNode->SetParent(this);
	}

	void SceneNode::OnUpdate(const float& deltaMs)
	{
		if (m_Parent)
		{
			m_Transform.GetWorldMatrixRef() = m_Parent->GetTransform().GetWorldMatrix() * m_Transform.GetLocalMatrix();
		}
		else
		{
			m_Transform.SetWorldMatrix(m_Transform.GetLocalMatrix());
		}
		for (auto i = m_Children.begin(); i != m_Children.end(); ++i)
		{
			(*i)->OnUpdate(deltaMs);
		}
	}

	void SceneNode::OnRender()
	{
		if (m_Actor)
		{
			m_Actor->OnRender();
		}
	}

}
