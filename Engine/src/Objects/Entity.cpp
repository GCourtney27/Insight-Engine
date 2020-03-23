#include "Entity.h"
#include "..\\Components\MeshRenderComponent.h"
#include "..\\Components\\EditorSelectionComponent.h"
#include "..\\Components\\LuaScriptComponent.h"
#include "..\Editor\Editor.h"
#include "..\Components\RigidBodyComponent.h"
#include "..\Graphics\Graphics.h"
#include "..\Systems\BenchmarkingTimer.h"

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

void Entity::LoadFromJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
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
	writer.StartArray(); // Start Transform Array

	// POSITION
	writer.StartObject(); // Start Position Object
	writer.Key("Position");
	{
		writer.StartArray();
		{
			writer.StartObject();

			writer.Key("x");
			writer.Double(GetTransform().GetPosition().x);

			writer.Key("y");
			writer.Double(GetTransform().GetPosition().y);
			
			writer.Key("z");
			writer.Double(GetTransform().GetPosition().z);
			
			writer.EndObject();
		}
		writer.EndArray();
	}
	writer.EndObject(); // End Position Object

	// ROTATION
	writer.StartObject(); // Start Rotation Object
	writer.Key("Rotation");
	{
		writer.StartArray();
		{
			writer.StartObject();

			writer.Key("x");
			writer.Double(GetTransform().GetRotation().x);

			writer.Key("y");
			writer.Double(GetTransform().GetRotation().y);

			writer.Key("z");
			writer.Double(GetTransform().GetRotation().z);

			writer.EndObject();
		}
		writer.EndArray();
	}
	writer.EndObject(); // End Rotation Object

	// SCALE
	writer.StartObject(); // Start Scale Object
	writer.Key("Scale");
	{
		writer.StartArray();
		{
			writer.StartObject();

			writer.Key("x");
			writer.Double(GetTransform().GetScale().x);

			writer.Key("y");
			writer.Double(GetTransform().GetScale().y);

			writer.Key("z");
			writer.Double(GetTransform().GetScale().z);

			writer.EndObject();
		}
		writer.EndArray();
	}
	writer.EndObject(); // End Scale Object

	writer.EndArray(); // End Transform Array
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
	//Debug::ScopedTimer objCreationTimer;
	Entity* toReturn = new Entity(&Graphics::Instance()->GetEngineInstance()->GetScene(), (*new ID()));

	toReturn->GetID().SetName("Projectile");
	toReturn->GetID().SetTag("Untagged");
	toReturn->GetID().SetType("Entity");
	toReturn->GetTransform().SetPosition(Graphics::Instance()->GetEngineInstance()->GetPlayer()->GetTransform().GetPositionCopy());
	toReturn->GetTransform().SetRotation(0.0f, 0.0f, 0.0f);
	toReturn->GetTransform().SetScale(0.5f, 0.5f, 2.3f);

	// MESH RENDERER
	Material* mat = nullptr;
	mat = mat->SetMaterialByType(Material::eMaterialType::PBR_UNTEXTURED, Material::eFlags::NOFLAGS);
	mat->Initiailze(Graphics::Instance()->GetDevice(), Graphics::Instance()->GetDeviceContext(), Material::eFlags::NOFLAGS);
	std::string file = "..\\Assets\\Objects\\Primatives\\Capsule.obj";
	MeshRenderer* mr = toReturn->AddComponent<MeshRenderer>();
	mr->Initialize(toReturn, file, Graphics::Instance()->GetDevice(), Graphics::Instance()->GetDeviceContext(), Graphics::Instance()->GetDefaultVertexShader(), mat);
	toReturn->SetHasMeshRenderer(true);
	mat->m_color.x = 1.0f;
	mat->m_color.y = 0.0f;
	mat->m_color.z = 0.0f;
	mat->m_cb_ps_PerObjectUtil.data.color = mat->m_color;
	mat->m_cb_ps_PerObjectUtil.ApplyChanges();

	Graphics::Instance()->GetEngineInstance()->GetScene().GetRenderManager().AddOpaqueInstantiatedObject(mr);

	// LUA SCRIPT
	LuaScript* ls = toReturn->AddComponent<LuaScript>();
	ls->Initialize(toReturn, "..\\Assets\\LuaScripts\\Projectile.lua");
	ls->Start();

	// RIGID BODY
	RigidBody* rb = toReturn->AddComponent<RigidBody>();
	rb->Initialize(toReturn, toReturn->GetTransform().GetScaleConst().x, AABB::eColliderType::SPHERE);
	rb->SetIsTrigger(true);
	Graphics::Instance()->GetEngineInstance()->GetScene().GetPhysicsSystem().AddEntity(rb);

	Graphics::Instance()->GetEngineInstance()->GetScene().AddInstantiatedEntity(toReturn);

	//objCreationTimer.Stop();
	//Debug::Editor::Instance()->DebugLog("Projectile instanciated in " + std::to_string(objCreationTimer.GetTimeInSeconds()) + " seconds");

	return toReturn;
}
