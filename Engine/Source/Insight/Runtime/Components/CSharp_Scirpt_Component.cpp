#include <ie_pch.h>

#include "CSharp_Scirpt_Component.h"

#include "Insight/Systems/Managers/Resource_Manager.h"
#include "Insight/Runtime/AActor.h"
#include "Insight/Core/Application.h"

#include "imgui.h"

namespace Insight {


	CSharpScriptComponent::CSharpScriptComponent(AActor* pOwner)
		: ActorComponent("C-Sharp Script Component", pOwner)
	{
		m_pMonoScriptManager = &ResourceManager::Get().GetMonoScriptManager();
	}

	CSharpScriptComponent::~CSharpScriptComponent()
	{
		Cleanup();
	}

	void TestRotate(float x, float y, float z)
	{
		AActor* actor = Application::Get().GetScene().GetSelectedActor();
		if (actor) {
			Transform& transform = actor->GetTransformRef();
			transform.Rotate(x, y, z);
		}
	}

	void CSharpScriptComponent::OnAttach()
	{
		
	}

	void CSharpScriptComponent::OnDetach()
	{

	}

	void CSharpScriptComponent::RegisterScript()
	{
		// Register the class with mono runtime
		if (!m_pMonoScriptManager->CreateClass(m_pClass, m_pObject, m_ModuleName.c_str())) {
			IE_CORE_ERROR("Failed to create C# class for \"{0}\"", m_ModuleName);
			return;
		}

		// Register the engine methods for the object
		if (!m_pMonoScriptManager->CreateMethod(m_pClass, m_pBeginPlayMethod, m_ModuleName.c_str(), "BeginPlay()")) {
			IE_CORE_ERROR("Failed to find method \"BeginPlay\" in \"{0}\"", m_ModuleName);
		}
		if (!m_pMonoScriptManager->CreateMethod(m_pClass, m_pUpdateMethod, m_ModuleName.c_str(), "Tick(double)")) {
			IE_CORE_ERROR("Failed to find method \"Tick\" in \"{0}\"", m_ModuleName);
		}

		// TEMP
		mono_add_internal_call("InsightEngine.Interop::TestRotate", reinterpret_cast<const void*>(TestRotate));
	}

	void CSharpScriptComponent::Cleanup()
	{
	}

	bool CSharpScriptComponent::LoadFromJson(const rapidjson::Value& jsonCSScriptComponent)
	{
		json::get_string(jsonCSScriptComponent[0], "ModuleName", m_ModuleName);
		json::get_bool(jsonCSScriptComponent[0], "Enabled", ActorComponent::m_Enabled);

		RegisterScript();
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
		Cleanup();
	}

	void CSharpScriptComponent::OnPreRender(const DirectX::XMMATRIX& matrix)
	{
	}

	void CSharpScriptComponent::PrepScriptedValues()
	{
		MonoClassField* rotOffset;
		float currentRotY = m_pOwner->GetTransformRef().GetRotation().y;
		rotOffset = mono_class_get_field_from_name(m_pClass, "m_YRotationOffset");
		//IE_CORE_INFO("Rotation: {0}", currentRotY);
		mono_field_set_value(m_pObject, rotOffset, &currentRotY);
	}

	void CSharpScriptComponent::OnUpdate(const float& deltaMs)
	{
		PrepScriptedValues();
		void* args[1];
		double doubleDt = (double)deltaMs;
		args[0] = &doubleDt;
		m_pMonoScriptManager->InvokeMethod(m_pUpdateMethod, m_pObject, args);
		MonoProperty* rot = mono_class_get_property_from_name(m_pClass, "RotationOffset");
		MonoMethod* method = mono_property_get_get_method(rot);
		MonoObject* result = mono_runtime_invoke(method, m_pObject, nullptr, nullptr);
		float y = *(float*)mono_object_unbox(result);
		
		ActorComponent::m_pOwner->GetTransformRef().Rotate(0.0f, y, 0.0f);
	}

	void CSharpScriptComponent::OnRender()
	{
	}

	void CSharpScriptComponent::OnChanged()
	{
	}

	void CSharpScriptComponent::OnImGuiRender()
	{
		if (ImGui::CollapsingHeader(m_ComponentName, ImGuiTreeNodeFlags_DefaultOpen)) {
			
			ImGui::Text("Module Name: "); ImGui::SameLine();
			ImGui::Text(m_ModuleName.c_str());
		}
	}

	void CSharpScriptComponent::RenderSceneHeirarchy()
	{
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
