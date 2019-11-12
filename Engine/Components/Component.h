#pragma once
#include "..\\Objects\\Object.h"

#include "document.h"
#include "json.h"
#include "prettywriter.h"
#include "ostreamwrapper.h"
#include "stringbuffer.h"
#include "filewritestream.h"
#include "istreamwrapper.h"
#include "writer.h"

class Entity;

class Component : public Object
{
public:
	Component(Entity* owner, const ID& id = ID()) : m_owner(owner), Object(id) {}

	virtual void Destroy() = 0;
	virtual void Start() = 0;
	virtual void Update(const float& deltaTime) = 0;
	virtual void OnImGuiRender() = 0;
	virtual void InitFromJSON(Entity* owner, const rapidjson::Value& componentInformation) = 0;
	virtual void WriteToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) = 0;

	bool& GetIsEnabled() { return m_enabled; }
	void SetEnable(bool enable) { m_enabled = enable; }

	Entity* GetOwner() { return m_owner; }

	void SetName(const char* name) { m_pName = name; }
	const char* GetName() { return m_pName; }

protected:
	Entity* m_owner = nullptr;
	const char* m_pName;
	bool m_enabled = true;


};