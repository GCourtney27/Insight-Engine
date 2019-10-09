#pragma once

#include <string>

class ID
{
public:
	enum eLayer
	{
		DEFAULT
	};

public:
	ID();
	ID(const std::string& id)
		: m_name(id), m_id(id), m_tag(id) {}
	ID(const char* id)
		: m_name(id), m_id(id), m_tag(id) {}

	bool operator == (const ID& id) const { return m_id == id.m_id; }
	bool operator != (const ID& id) const { return m_id != id.m_id; }
	
	bool IsValid() const
	{
		return (m_id != "");
	}

	static size_t ms_uniqueID;
	static std::string GetUniqueID();
	void SetUniqueID(std::string id) { m_id = id; }

	std::string& GetName() { return m_name; }
	void SetName(std::string name) { m_name = name; }

	std::string& GetTag() { return m_tag; }
	void SetTag(std::string tag) { m_tag = tag; }

	void SetLayer(const int& layer) { m_layer = layer; }
	int GetLayer() const { return m_layer; }

protected:
	std::string m_name;
	std::string m_id;
	std::string m_tag;
	int m_layer;
};