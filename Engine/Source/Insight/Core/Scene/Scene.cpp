#include <ie_pch.h>

#include "Scene.h"

#include "imgui.h"
#include "ImGuizmo.h"
#include "Insight/Core/Application.h"
#include "Insight/Runtime/Components/Static_Mesh_Component.h"
#include "Insight/Runtime/Components/Sky_Sphere_Component.h"
#include "Insight/Input/Input.h"

namespace Insight {


	Scene::Scene()
		: m_pSceneRoot(new SceneNode("Scene Root"))
	{

	}

	Scene::~Scene()
	{
		Destroy();
	}

	bool Scene::LoadFromJson(const std::string& fileName)
	{
		FileSystem::Get().LoadSceneFromJson(fileName, *this);


		return true;
	}

	bool Scene::Init()
	{
		// TODO: Init from file
		m_Renderer = Application::Get().GetWindow().GetRenderContext();
		m_ModelManager.Init();

		m_pPlayerCharacter = new APlayerCharacter(0, "Player Character");
		LoadFromJson(FileSystem::Get().GetRelativeAssetDirectoryPath("Scenes/MyScene.json"));
		{
			m_pSkyboxActor = new ASkySphere(8, "Sky Sphere Actor");

			m_PostFxActor = new APostFx(9, "Post-Fx Actor");
		}

		m_pSceneRoot->AddChild(m_pSkyboxActor);
		m_pSceneRoot->AddChild(m_PostFxActor);

		m_pSceneRoot->AddChild(m_pPlayerCharacter);

		m_Renderer->PostInit();
		return true;
	}

	void Scene::OnUpdate(const float& deltaMs)
	{
		m_Renderer->OnUpdate(deltaMs);
		m_pSceneRoot->OnUpdate(deltaMs);
	}

	void Scene::OnImGuiRender()
	{
		RenderSceneHeirarchy();
		RenderInspector();
		RenderCreatorWindow();
	}

	void Scene::RenderSceneHeirarchy()
	{
		ImGui::Begin("Heirarchy");
		{
			if (ImGui::CollapsingHeader(m_pSceneRoot->GetDisplayName(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				m_pSceneRoot->RenderSceneHeirarchy();
			}
		}
		ImGui::End();
	}

	void Scene::RenderInspector()
	{
		ImGui::Begin("Details");
		{
			if (m_pSelectedActor != nullptr) {

				m_pSelectedActor->OnImGuiRender();

				//XMFLOAT4X4 localMat; 
				//XMFLOAT4X4 deltaMat;
				//XMFLOAT4X4 viewMat;
				//XMFLOAT4X4 projMat;
				//XMStoreFloat4x4(&localMat, m_pSelectedActor->GetTransformRef().GetLocalMatrixRef());
				//XMStoreFloat4x4(&viewMat, m_pPlayerCharacter->GetCameraRef().GetViewMatrix());
				//XMStoreFloat4x4(&projMat, m_pPlayerCharacter->GetCameraRef().GetProjectionMatrix());

				//static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
				//if (Input::IsKeyPressed('W')) {
				//	mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
				//}else if(Input::IsKeyPressed('E')) {
				//	mCurrentGizmoOperation = ImGuizmo::ROTATE;

				//}else if (Input::IsKeyPressed('R')) {
				//	mCurrentGizmoOperation = ImGuizmo::SCALE;

				//}

				//static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);


				//
				//ImGuiIO& io = ImGui::GetIO();
				//ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
				////TODO if(Raycast::LastRayCast::Succeeded) than run this line if false than skip it (disbles the guizmo)
				//ImGuizmo::Manipulate(*viewMat.m, *projMat.m, mCurrentGizmoOperation, mCurrentGizmoMode, *localMat.m, *deltaMat.m, NULL, NULL, NULL);

				//switch (mCurrentGizmoOperation) {
				//case ImGuizmo::TRANSLATE:
				//{
				//	m_pSelectedActor->GetTransformRef().SetPosition(Vector3(localMat._41, localMat._42, localMat._43));
				//}
				//case ImGuizmo::SCALE:
				//{
				//	m_pSelectedActor->GetTransformRef().SetScale(Vector3(localMat._11, localMat._22, localMat._33));
				//}
				//case ImGuizmo::ROTATE:
				//{
				//	//XMMATRIX rotMat = XMLoadFloat4x4(&localMat);
				//	//m_pSelectedActor->GetTransformRef().SetRotationMatrix(rotMat);
				//	
				//	//XMVECTOR rotVector = XMVector3Transform(m_pSelectedActor->GetTransformRef().GetRotation(), rotMat);
				//	//XMFLOAT3 rotFloat;
				//	//XMStoreFloat3(&rotFloat, rotVector);
				//	//m_pSelectedActor->GetTransformRef().SetRotation(Vector3(rotFloat.x, rotFloat.y, rotFloat.z));

				//	//X: localMat._33; localMat._22; localMat._23; localMat._33;
				//	//Y: localMat._31; localMat._11; localMat._13; localMat._33;
				//	//Z localMat._21; localMat._11; localMat._12; localMat._22;

				//	//m_pSelectedActor->GetTransformRef().SetRotation(Vector3(localMat._13, localMat._23, localMat._33));

				//}
				//default: {break;}
				//}
			}
		}
		ImGui::End();
	}

	void Scene::RenderCreatorWindow()
	{
		// TODO make this a colapsing header with different options
		ImGui::Begin("Creator");
		{
			if (ImGui::Button("New Point Light", { 125, 25 })) {
				m_pSceneRoot->AddChild(new APointLight(5, "New cool point light"));
			}
		}
		ImGui::End();
	}

	void Scene::OnPreRender()
	{
		m_Renderer->OnPreFrameRender();
		m_pSceneRoot->OnPreRender(XMMatrixIdentity());
		m_ModelManager.UploadVertexDataToGPU();
	}

	void Scene::OnRender()
	{
		m_Renderer->OnRender();
		m_pSceneRoot->OnRender();
		m_ModelManager.Render();
	}

	void Scene::OnMidFrameRender()
	{
		m_Renderer->OnMidFrameRender();
	}

	void Scene::OnPostRender()
	{
		m_ModelManager.PostRender();
		m_Renderer->ExecuteDraw();
		m_Renderer->SwapBuffers();
	}

	void Scene::Destroy()
	{
		delete m_pSceneRoot;
	}


}
