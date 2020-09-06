#include <Engine_pch.h>

#include "Renderer.h"
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

	void SceneNode::RemoveChild(SceneNode* ChildNode)
	{
		auto iter = std::find(m_Children.begin(), m_Children.end(), ChildNode);
		if (iter != m_Children.end()) {

			(*iter)->Destroy();
			m_Children.erase(iter);
		}
	}

	bool SceneNode::WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer)
	{
		size_t numChildrenObjects = m_Children.size();
		for (size_t i = 0; i < numChildrenObjects; ++i) {
			m_Children[i]->WriteToJson(Writer);
		}
		return false;
	}

	bool SceneNode::LoadFromJson(const rapidjson::Value& jsonActor)
	{
		return false;
	}

	void SceneNode::RenderSceneHeirarchy()
	{
		size_t numChildrenObjects = m_Children.size();
		for (size_t i = 0; i < numChildrenObjects; ++i) {
			m_Children[i]->RenderSceneHeirarchy();
		}
	}

	bool SceneNode::OnInit()
	{
		m_RootTransform.EditorInit();
		return true;
	}

	bool SceneNode::OnPostInit()
	{
		return true;
	}

	void SceneNode::OnUpdate(const float DeltaMs)
	{
		for (auto i = m_Children.begin(); i != m_Children.end(); ++i) {
			(*i)->OnUpdate(DeltaMs);
		}
	}

	void SceneNode::CalculateParent(XMMATRIX parentMat)
	{
		GetTransformRef().SetWorldMatrix(XMMatrixMultiply(parentMat, GetTransformRef().GetLocalMatrixRef()));
		for (auto i = m_Children.begin(); i != m_Children.end(); ++i) {
			(*i)->CalculateParent(GetTransformRef().GetWorldMatrixRef());
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
		IE_CORE_INFO("{0}", m_DisplayName);
		for (auto i = m_Children.begin(); i != m_Children.end(); ++i) {
			(*i)->BeginPlay();
		}
	}

	void SceneNode::Tick(const float DeltaMs)
	{
		for (auto i = m_Children.begin(); i != m_Children.end(); ++i) {
			(*i)->Tick(DeltaMs);
		}
	}

	void SceneNode::Exit()
	{
	}

	void SceneNode::EditorEndPlay()
	{
		m_RootTransform.EditorEndPlay();
		for (auto i = m_Children.begin(); i != m_Children.end(); ++i) {
			(*i)->EditorEndPlay();
		}
	}

	void SceneNode::Destroy()
	{
		size_t numChildrenObjects = m_Children.size();
		for (size_t i = 0; i < numChildrenObjects; ++i) {
			m_Children[i]->Destroy();
			delete m_Children[i];
		}
	}

}
