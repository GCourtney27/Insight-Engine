#pragma once

#include <string>

#include <Runtime/Core.h>

namespace Insight {


	class INSIGHT_API ID
	{
	public:
		enum eLayer
		{
			DEFAULT
		};

	public:
		ID();
		ID(const std::string& id)
			: m_Name(id), m_Id(id), m_Tag(id) {}
		ID(const char* id)
			: m_Name(id), m_Id(id), m_Tag(id) {}

		bool operator == (const ID& id) const { return m_Id == id.m_Id; }
		bool operator != (const ID& id) const { return m_Id != id.m_Id; }

		bool IsValid() const
		{
			return (m_Id != "");
		}

		static size_t ms_uniqueID;
		static std::string GetUniqueID();
		void SetUniqueID(std::string id) { m_Id = id; }

		std::string& GetName() { return m_Name; }
		void SetName(std::string name) { m_Name = name; }

		std::string& GetType() { return m_Type; }
		void SetType(std::string type) { m_Type = type; }

		std::string& GetTag() { return m_Tag; }
		void SetTag(std::string tag) { m_Tag = tag; }

		void SetLayer(const int& layer) { m_Layer = layer; }
		int GetLayer() const { return m_Layer; }

	protected:
		std::string m_Type;
		std::string m_Name;
		std::string m_Id;
		std::string m_Tag;
		int m_Layer;
	};

}

