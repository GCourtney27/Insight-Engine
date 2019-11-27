#include "Entity.h"
#include "..\\Components\MeshRenderComponent.h"
#include "..\\Components\\EditorSelectionComponent.h"
#include "..\\Components\\LuaScriptComponent.h"
#include "..\Editor\Editor.h"
#include "..\Components\RigidBodyComponent.h"
#include "..\Graphics\Graphics.h"

bool Entity::Initialize()
{
	bool success = true;

	UpdateTransformCopyWithTransform();
	
	return success;
}

void Entity::Update(const float& deltaTime)
{

	// If the editor is not playing keep coppying the transforms
	if(!Debug::Editor::Instance()->PlayingGame())
		UpdateTransformCopyWithTransform();

	// If editor is present do this if not just remove this
	EditorSelection* es = GetComponent<EditorSelection>();
	if (es != nullptr)
	{
		es->SetPosition(m_transform.GetPosition());
	}

	MeshRenderer* mr = GetComponent<MeshRenderer>();
	if (mr != nullptr)
	{
		mr->Update(deltaTime);
	}

	// Update the objects transform
	m_transform.Update();
}

void Entity::Draw(const XMMATRIX & projectionMatrix, const XMMATRIX & viewMatrix)
{
	MeshRenderer* mr = GetComponent<MeshRenderer>();

	if (mr != nullptr)
	{
		mr->Draw(projectionMatrix, viewMatrix);
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
	for (Component* component : m_components)
	{
		component->Start();
	}
}

void Entity::OnUpdate(const float& deltaTime)
{
	//m_transform.AdjustRotation(0.0f, 0.0f, 0.001f * deltaTime); // This line is not needed for wntity to work, It is a Debug line to test Playing Game feature

	for (Component* component : m_components)
	{
		component->Update(deltaTime);
	}

	this->m_transform.Update();
}

void Entity::OnExit()
{
}

void Entity::OnEditorStop()
{
	UpdateTransformWithCopy();
	for (Component* com : m_components)
	{
		com->OnEditorStop();
	}
}

void Entity::WriteToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer> & writer)
{
	writer.StartObject(); // Start Entity
	writer.Key("Type");
	writer.String(GetID().GetType().c_str());

	writer.Key("Name");
	writer.String(GetID().GetName().c_str());

#pragma region Write Transform
	writer.Key("Transform");
	writer.StartArray(); // Start Transform

	// POSITION
	writer.StartObject(); // Start Array Object
	writer.Key("Position");
	writer.StartArray(); // Start Position
	// X
	writer.StartObject(); // Start X
	writer.Key("x");
	writer.Double(GetTransform().GetPosition().x);
	writer.EndObject(); // End X
	// Y
	writer.StartObject(); // Start Y
	writer.Key("y");
	writer.Double(GetTransform().GetPosition().y);
	writer.EndObject(); // End Y

	// Z
	writer.StartObject(); // Start Z
	writer.Key("z");
	writer.Double(GetTransform().GetPosition().z);
	writer.EndObject(); // End Z
	writer.EndArray(); // End Pisition
	writer.EndObject(); // End Array Object

	// ROTATION
	writer.StartObject(); // Start Array Object
	writer.Key("Rotation");
	writer.StartArray(); // Start Position
	// X
	writer.StartObject(); // Start X
	writer.Key("x");
	writer.Double(GetTransform().GetRotation().x);
	writer.EndObject(); // End X
	// Y
	writer.StartObject(); // Start Y
	writer.Key("y");
	writer.Double(GetTransform().GetRotation().y);
	writer.EndObject(); // End Y

	// Z
	writer.StartObject(); // Start Z
	writer.Key("z");
	writer.Double(GetTransform().GetRotation().z);
	writer.EndObject(); // End Z
	writer.EndArray(); // End Pisition
	writer.EndObject(); // End Array Object

	// SCALE
	writer.StartObject(); // Start Array Object
	writer.Key("Scale");
	writer.StartArray(); // Start Position
	// X
	writer.StartObject(); // Start X
	writer.Key("x");
	writer.Double(GetTransform().GetScale().x);
	writer.EndObject(); // End X
	// Y
	writer.StartObject(); // Start Y
	writer.Key("y");
	writer.Double(GetTransform().GetScale().y);
	writer.EndObject(); // End Y

	// Z
	writer.StartObject(); // Start Z
	writer.Key("z");
	writer.Double(GetTransform().GetScale().z);
	writer.EndObject(); // End Z
	writer.EndArray(); // End Pisition
	writer.EndObject(); // End Array Object

	writer.EndArray(); // End Transform
#pragma endregion

	writer.Key("Components");
	writer.StartArray(); // Start Components
	for (Component * component : m_components)
	{
		writer.StartObject(); // Start Component

		component->WriteToJSON(writer);

		writer.EndObject(); // End Component
	}
	writer.EndArray(); // End Components

	writer.EndObject(); // End Entity
}

void Entity::OnEvent(const Event & event)
{
}

bool Entity::PhysicsIsColliding()
{
	RigidBody* rb = this->GetComponent<RigidBody>();
	if (rb != nullptr)
	{
		return rb->IsColliding();
	}
	
	return false;
}

void Entity::Translate(float x, float y, float z)
{
	RigidBody* rb = this->GetComponent<RigidBody>();
	if (rb != nullptr)
	{
		rb->Translate(x, y, z);
	}
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

Entity * Entity::CreateEntityWithDefaultParams()
{
	Entity* toReturn = new Entity(&Graphics::Instance()->GetEngineInstance()->GetScene(), (*new ID()));

	toReturn->GetID().SetName("Default Entity");
	toReturn->GetID().SetTag("Untagged");
	toReturn->GetID().SetType("Entity");
	toReturn->GetTransform().SetPosition(Graphics::Instance()->GetEngineInstance()->GetPlayer()->GetTransform().GetPositionConst());
	toReturn->GetTransform().SetRotation(0.0f, 0.0f, 0.0f);
	toReturn->GetTransform().SetScale(6.0f, 6.0f, 6.0f);

	// MESH RENDERER
	Material* mat = nullptr;
	mat = mat->SetMaterialByType(Material::eMaterialType::PBR_DEFAULT, Material::eFlags::NOFLAGS);
	mat->Initiailze(Graphics::Instance()->GetDevice(), Graphics::Instance()->GetDeviceContext(), Material::eFlags::NOFLAGS);

	std::string file = "..\\Assets\\Objects\\Primatives\\Cube.fbx";
	MeshRenderer* mr = toReturn->AddComponent<MeshRenderer>();
	mr->Initialize(toReturn, file, Graphics::Instance()->GetDevice(), Graphics::Instance()->GetDeviceContext(), Graphics::Instance()->GetDefaultVertexShader(), mat);
	toReturn->SetHasMeshRenderer(true);
	Graphics::Instance()->GetEngineInstance()->GetScene().GetRenderManager().AddOpaqueInstantiatedObject(mr);

	// LUA SCRIPT
	LuaScript* ls = toReturn->AddComponent<LuaScript>();
	ls->Initialize(toReturn, "..\\Assets\\LuaScripts\\Projectile.lua");
	ls->Start();

	// EDITOR SELECTION
	toReturn->AddComponent<EditorSelection>()->Initialize(toReturn, 10.0f, toReturn->GetTransform().GetPosition());

	//RIGID BODY
	RigidBody* rb = toReturn->AddComponent<RigidBody>();
	rb->Initialize(toReturn, toReturn->GetTransform().GetScaleConst().x, AABB::eColliderType::SPHERE);
	Graphics::Instance()->GetEngineInstance()->GetScene().GetPhysicsSystem().AddEntity(rb);

	Graphics::Instance()->GetEngineInstance()->GetScene().AddInstantiatedEntity(toReturn);


	return toReturn;
}
