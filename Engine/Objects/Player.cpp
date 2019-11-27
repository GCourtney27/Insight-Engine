#include "Player.h"
#include "..\Graphics\Graphics.h"
#include "..\Editor\Editor.h"
#include "..\Components\RigidBodyComponent.h"

bool Player::Initialize(Scene * scene, const ID & id)
{
	this->m_id = id;
	this->m_scene = scene;

	InitializeCamera(scene);

	return true;
}

void Player::InitializeCamera(Scene* scene)
{
	m_pCamera = new Camera(scene, *(new ID("Player Camera")));
	
	//m_pCamera->GetTransform().SetParent(&this->GetTransform());
	
	m_pCamera->SetProjectionValues(80.0f, static_cast<float>(1600) / static_cast<float>(900), 0.1f, 10000.0f);

	//m_pCamera->GetTransform().SetPosition(DirectX::XMFLOAT3(60.0f, 18.0f, -13.0f));
	//m_pCamera->GetTransform().SetPosition(DirectX::XMFLOAT3(-26.0f, 11.6f, -44.3f));
	m_pCamera->GetTransform().SetPosition(DirectX::XMFLOAT3(-1.5f, 49.9f, -73.9f));// Physics Test Scene
	m_pCamera->GetTransform().SetRotation(0.5f, 0.0f, 0.0f);
	m_pCamera->GetTransform().SetScale(5.0f, 5.0f, 5.0f);

	m_pMaterial = m_pMaterial->SetMaterialByType(Material::eMaterialType::PBR_UNTEXTURED, Material::eFlags::NOFLAGS);
	m_pMaterial->Initiailze(Graphics::Instance()->GetDevice(), Graphics::Instance()->GetDeviceContext(), Material::eFlags::NOFLAGS);

	MeshRenderer* mr = m_pCamera->AddComponent<MeshRenderer>();
	mr->Initialize(m_pCamera, "..\\Assets\\Objects\\Camera.obj", Graphics::Instance()->GetDevice(), Graphics::Instance()->GetDeviceContext(), Graphics::Instance()->GetDefaultVertexShader(), m_pMaterial);

	//LuaScript* ls = m_pCamera->AddComponent<LuaScript>();
	//ls->Initialize(m_pCamera, "..\\Assets\\LuaScripts\\PlayerController.lua");

	EditorSelection* es = m_pCamera->AddComponent<EditorSelection>();
	es->Initialize(m_pCamera, 10.0f, m_pCamera->GetTransform().GetPosition());
	m_pCamera->SetCanBeJSONSaved(false);

	/*RigidBody* rb = m_pCamera->AddComponent<RigidBody>();
	rb->GetCollider().Initialize(m_pCamera);
	scene->GetPhysicsSystem().AddEntity(rb);*/
	
	scene->GetRenderManager().SetGameCamera(m_pCamera);
	scene->AddEntity(m_pCamera);
}

