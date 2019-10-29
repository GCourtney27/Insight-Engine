#include "Player.h"
#include "..\Graphics\Graphics.h"

void Player::InitializeCamera(Scene* scene)
{
	m_pCamera = new Camera(scene, *(new ID("PlayerCamera")));
	
	//m_pCamera->SetParent(&this->GetTransform());
	m_pCamera->SetProjectionValues(80.0f, static_cast<float>(1920) / static_cast<float>(1000), 0.1f, 1000.0f);
	m_pCamera->GetTransform().SetPosition(DirectX::XMFLOAT3(0.0f, 10.0f, -10.0f));

	m_pCamera->GetTransform().SetRotation(0.0f, 0.0f, 0.0f);
	m_pCamera->GetTransform().SetScale(1.0f, 1.0f, 1.0f);
	EditorSelection* es = m_pCamera->AddComponent<EditorSelection>();
	es->Initialize(m_pCamera, 10.0f, m_pCamera->GetTransform().GetPosition());

	MeshRenderer* mr = m_pCamera->AddComponent<MeshRenderer>();
	mr->Initialize(m_pCamera, "..\\Assets\\Objects\\Camera.obj", Graphics::Instance()->GetDevice(), Graphics::Instance()->GetDeviceContext(), Graphics::Instance()->GetDefaultVertexShader(), m_pMaterial);
	
	LuaScript* ls = m_pCamera->AddComponent<LuaScript>();
	std::string file("..\\Assets\\LuaScripts\\UpWards.lua");
	ls->Initialize(m_pCamera, file);

	//m_children.push_back(&m_pCamera->GetTransform());
	scene->AddEntity(m_pCamera);
}
