#include "Entity.h"
#include "..\\Components\MeshRenderComponent.h"
#include "..\\Components\\EditorSelectionComponent.h"
#include "..\\Components\\LuaScriptComponent.h"
#include "..\Editor\Editor.h"

bool Entity::Initialize()
{
	bool success = true;

	UpdateTransformCopyWithTransform();
	
	return success;
}

void Entity::Update(float deltaTime)
{
	m_transform.AdjustPosition(0.0f, 0.0f, 0.0f);

	// If the editor is not playing keep coppying the transforms
	if(!Debug::Editor::Instance()->PlayingGame())
		UpdateTransformCopyWithTransform();

	EditorSelection* es = GetComponent<EditorSelection>();
	if (es != nullptr)
	{
		es->SetPosition(m_transform.GetPosition());
	}

}

void Entity::Draw(const XMMATRIX & viewProjectionMatrix)
{
	MeshRenderer* mr = GetComponent<MeshRenderer>();

	if (mr != nullptr)
	{
		mr->SetWorldMat(this->m_transform.GetWorldMatrix());
		mr->Draw(viewProjectionMatrix);
	}
}

void Entity::Destroy()
{
	for (Component* component : m_components)
	{
		component->Destroy();
		delete component;
	}
	m_components.clear();
}

void Entity::OnStart()
{

}

void Entity::OnUpdate(float deltaTime)
{
	//m_transform.AdjustRotation(0.0f, 0.0f, 0.001f * deltaTime); // This line is not needed, It is a Debug line to test Playing Game feature

	for (Component* component : m_components)
	{
		component->Update();
	}
}

void Entity::OnExit()
{
}

void Entity::OnEditorStop()
{
	UpdateTransformWithCopy();

	///// CHANGE THIS!!!////
	LuaScript* ls = GetComponent<LuaScript>();
	if (ls != nullptr)
	{
		ls->SetCallCounter(1);
	}
}

void Entity::OnEvent(const Event & event)
{
}

void Entity::AddComponent(Component * component)
{
	assert(component);
	assert(std::find(m_components.begin(), m_components.end(), component) == m_components.end());

	m_components.push_back(component);
}

void Entity::RemoveComponent(Component * component)
{
	assert(std::find(m_components.begin(), m_components.end(), component) != m_components.end());

	auto iter = std::find(m_components.begin(), m_components.end(), component);
	(*iter)->Destroy();
	delete *iter;
	m_components.erase(iter);
}
