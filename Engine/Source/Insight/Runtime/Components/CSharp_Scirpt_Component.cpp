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

		GetTransformFields();
		UpdateScriptFields();
	}

	void CSharpScriptComponent::Cleanup()
	{
		m_pMonoScriptManager = nullptr;
	}

	bool CSharpScriptComponent::LoadFromJson(const rapidjson::Value& jsonCSScriptComponent)
	{
		json::get_string(jsonCSScriptComponent[0], "ModuleName", m_ModuleName);
		json::get_bool(jsonCSScriptComponent[0], "Enabled", ActorComponent::m_Enabled);

		RegisterScript();
		return true;
	}

	bool CSharpScriptComponent::WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer)
	{
		Writer.Key("CSharpScript");
		Writer.StartArray(); // Start CSScript Write
		{
			Writer.StartObject();
			Writer.Key("ModuleName");
			Writer.String(m_ModuleName.c_str());

			Writer.Key("Enabled");
			Writer.Bool(ActorComponent::m_Enabled);
			Writer.EndObject();
		}
		Writer.EndArray(); // End CSScript Write

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

	void CSharpScriptComponent::CalculateParent(const DirectX::XMMATRIX& matrix)
	{
	}

	void CSharpScriptComponent::UpdateScriptFields()
	{
		ieVector3 currentPos = m_pOwner->GetTransformRef().GetPosition();
		mono_field_set_value(m_PositionObj, m_XPositionField, &currentPos.x);
		mono_field_set_value(m_PositionObj, m_YPositionField, &currentPos.y);
		mono_field_set_value(m_PositionObj, m_ZPositionField, &currentPos.z);

		ieVector3 currentRot = m_pOwner->GetTransformRef().GetRotation();
		mono_field_set_value(m_RotationObj, m_XRotationField, &currentRot.x);
		mono_field_set_value(m_RotationObj, m_YRotationField, &currentRot.y);
		mono_field_set_value(m_RotationObj, m_ZRotationField, &currentRot.z);

		ieVector3 currentSca = m_pOwner->GetTransformRef().GetScale();
		mono_field_set_value(m_ScaleObj, m_XScaleField, &currentSca.x);
		mono_field_set_value(m_ScaleObj, m_YScaleField, &currentSca.y);
		mono_field_set_value(m_ScaleObj, m_ZScaleField, &currentSca.z);
	}

	void CSharpScriptComponent::ProcessScriptTransformChanges()
	{
		float pos[3];
		float rot[3];
		float sca[3];

		mono_field_get_value(m_PositionObj, m_XPositionField, &pos[0]);
		mono_field_get_value(m_PositionObj, m_YPositionField, &pos[1]);
		mono_field_get_value(m_PositionObj, m_ZPositionField, &pos[2]);
		ActorComponent::m_pOwner->GetTransformRef().SetPosition(pos[0], pos[1], pos[2]);

		mono_field_get_value(m_RotationObj, m_XRotationField, &rot[0]);
		mono_field_get_value(m_RotationObj, m_YRotationField, &rot[1]);
		mono_field_get_value(m_RotationObj, m_ZRotationField, &rot[2]);
		ActorComponent::m_pOwner->GetTransformRef().SetRotation(rot[0], rot[1], rot[2]);
		
		mono_field_get_value(m_ScaleObj, m_XScaleField, &sca[0]);
		mono_field_get_value(m_ScaleObj, m_YScaleField, &sca[1]);
		mono_field_get_value(m_ScaleObj, m_ZScaleField, &sca[2]);
		ActorComponent::m_pOwner->GetTransformRef().SetScale(sca[0], sca[1], sca[2]);
	}

	void CSharpScriptComponent::GetTransformFields()
	{
		// Get Transform Object
		MonoProperty* transformProp = mono_class_get_property_from_name(m_pClass, "Transform");
		MonoMethod* transformGetMethod = mono_property_get_get_method(transformProp);
		m_TransformObject = mono_runtime_invoke(transformGetMethod, m_pObject, nullptr, nullptr);
		MonoClass* transformClass = mono_object_get_class(m_TransformObject);

		// Get Postion Object inside Transform
		MonoProperty* posProp = mono_class_get_property_from_name(transformClass, "Position");
		MonoMethod* posGetMethod = mono_property_get_get_method(posProp);
		m_PositionObj = mono_runtime_invoke(posGetMethod, m_TransformObject, nullptr, nullptr);
		MonoClass* posClass = mono_object_get_class(m_PositionObj);
		m_XPositionField = mono_class_get_field_from_name(posClass, "m_X");
		m_YPositionField = mono_class_get_field_from_name(posClass, "m_Y");
		m_ZPositionField = mono_class_get_field_from_name(posClass, "m_Z");
		
		// Get Scale Object inside Transform
		MonoProperty* scaProp = mono_class_get_property_from_name(transformClass, "Scale");
		MonoMethod* scaGetMethod = mono_property_get_get_method(scaProp);
		m_ScaleObj = mono_runtime_invoke(scaGetMethod, m_TransformObject, nullptr, nullptr);
		MonoClass* scaClass = mono_object_get_class(m_ScaleObj);
		m_XScaleField = mono_class_get_field_from_name(scaClass, "m_X");
		m_YScaleField = mono_class_get_field_from_name(scaClass, "m_Y");
		m_ZScaleField = mono_class_get_field_from_name(scaClass, "m_Z");

		// Get Rotation Object inside Transform
		MonoProperty* rotProp = mono_class_get_property_from_name(transformClass, "Rotation");
		MonoMethod* rotGetMethod = mono_property_get_get_method(rotProp);
		m_RotationObj = mono_runtime_invoke(rotGetMethod, m_TransformObject, nullptr, nullptr);
		MonoClass* rotClass = mono_object_get_class(m_RotationObj);
		m_XRotationField = mono_class_get_field_from_name(rotClass, "m_X");
		m_YRotationField = mono_class_get_field_from_name(rotClass, "m_Y");
		m_ZRotationField = mono_class_get_field_from_name(rotClass, "m_Z");

	}

	void CSharpScriptComponent::OnUpdate(const float& deltaMs)
	{
		
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
		m_pMonoScriptManager->InvokeMethod(m_pBeginPlayMethod, m_pObject, nullptr);
	}

	void CSharpScriptComponent::Tick(const float& deltaMs)
	{
		if (!m_CanBeTicked) { return; }
		UpdateScriptFields();

		void* args[1];
		double doubleDt = (double)deltaMs;
		args[0] = &doubleDt;
		m_pMonoScriptManager->InvokeMethod(m_pUpdateMethod, m_pObject, args);

		ProcessScriptTransformChanges();
	}
}
