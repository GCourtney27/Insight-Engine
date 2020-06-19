#include <ie_pch.h>

#include "CSharp_Scirpt_Component.h"

#include "Insight/Systems/Managers/Resource_Manager.h"
#include "Insight/Runtime/AActor.h"
#include "Insight/Core/Application.h"

#include "imgui.h"

namespace Insight {

	CSharpScriptComponent::CSharpScriptComponent(StrongActorPtr pOwner)
		: ActorComponent("C-Sharp Script Component", pOwner)
	{
		m_pMonoScriptManager = &ResourceManager::Get().GetMonoScriptManager();

	}

	CSharpScriptComponent::~CSharpScriptComponent()
	{
	}

	void CSharpScriptComponent::OnAttach()
	{
		// Register the class with mono runtime
		if (!m_pMonoScriptManager->CreateClass(m_pClass, m_pObject, m_ModuleName.c_str())) {
			IE_CORE_ERROR("Failed to create C# class for \"{0}\"", m_ModuleName);
			return;
		}
		// Register the engine methods for the object
		if (!m_pMonoScriptManager->CreateMethod(m_pClass, m_pBeginPlayMethod, m_ModuleName.c_str(), "BeginPlay")) {
			IE_CORE_ERROR("Failed to find method \"OnUpdate\" in \"{0}\"", m_ModuleName);
		}
		if (!m_pMonoScriptManager->CreateMethod(m_pClass, m_pUpdateMethod, m_ModuleName.c_str(), "Tick")) {
			IE_CORE_ERROR("Failed to find method \"OnUpdate\" in \"{0}\"", m_ModuleName);
		}
	}

	void CSharpScriptComponent::OnDetach()
	{

	}

	bool CSharpScriptComponent::LoadFromJson(const rapidjson::Value& jsonStaticMeshComponent)
	{
		return true;
	}

	void CSharpScriptComponent::OnInit()
	{
	}

	void CSharpScriptComponent::OnPostInit()
	{
	}

	void CSharpScriptComponent::OnDestroy()
	{
	}

	void CSharpScriptComponent::OnUpdate(const float& deltaTime)
	{

	}

	void CSharpScriptComponent::OnChanged()
	{
	}

	void CSharpScriptComponent::OnImGuiRender()
	{
		if (ImGui::CollapsingHeader(m_ComponentName, ImGuiTreeNodeFlags_DefaultOpen)) {
		
			ImGui::Text("Module Name"); ImGui::SameLine();
			ImGui::Text(m_ModuleName.c_str());
		}
	}

	void CSharpScriptComponent::BeginPlay()
	{
		if (!m_CanBeCalledOnBeginPlay) { return; }

	}

	void CSharpScriptComponent::Tick(const float& deltaMs)
	{
		if (!m_CanBeTicked) { return; }
		void* args[1];
		args[0] = &const_cast<float&>(deltaMs);
		m_pMonoScriptManager->InvokeMethod(m_pUpdateMethod, m_pObject, args);
	}

	void CSharpScriptComponent::Interop_Translate(float x, float y, float z)
	{
		Transform& transform = m_pOwner->GetTransformRef();
		transform.Translate(x, y, z);
	}

	void CSharpScriptComponent::Interop_Rotate(float x, float y, float z)
	{
		Transform& transform = m_pOwner->GetTransformRef();
		transform.Rotate(x, y, z);
	}

	void CSharpScriptComponent::Interop_Scale(float x, float y, float z)
	{
		Transform& transform = m_pOwner->GetTransformRef();
		transform.Scale(x, y, z);
	}

	void CSharpScriptComponent::Interop_SetPosition(float x, float y, float z)
	{
		Transform& transform = m_pOwner->GetTransformRef();
		transform.SetPosition(Vector3(x, y, z));
	}

	void CSharpScriptComponent::Interop_SetRotation(float x, float y, float z)
	{
		Transform& transform = m_pOwner->GetTransformRef();
		transform.SetRotation(Vector3(x, y, z));
	}

	void CSharpScriptComponent::Interop_SetScale(float x, float y, float z)
	{
		Transform& transform = m_pOwner->GetTransformRef();
		transform.SetScale(Vector3(x, y, z));
	}

}
