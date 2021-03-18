#pragma once

#include <string>

#include <Runtime/Core.h>

namespace Insight {


	class INSIGHT_API ID
	{
	public:
		enum ELayer
		{
			DEFAULT
		};

	public:
		ID();
		ID(const EString& id)
			: m_Name(id), m_Id(id), m_Tag(id) {}
		ID(const TChar* id)
			: m_Name(id), m_Id(id), m_Tag(id) {}

		bool operator == (const ID& id) const { return m_Id == id.m_Id; }
		bool operator != (const ID& id) const { return m_Id != id.m_Id; }

		bool IsValid() const
		{
			return (m_Id != TEXT(""));
		}

		static size_t ms_uniqueID;
		static EString GetUniqueID();
		void SetUniqueID(const EString& id) { m_Id = id; }

		EString& GetName() { return m_Name; }
		void SetName(const EString& name) { m_Name = name; }

		EString& GetType() { return m_Type; }
		void SetType(const EString& type) { m_Type = type; }

		EString& GetTag() { return m_Tag; }
		void SetTag(const EString& tag) { m_Tag = tag; }

		void SetLayer(const int& layer) { m_Layer = layer; }
		int GetLayer() const { return m_Layer; }

	protected:
		EString m_Type;
		EString m_Name;
		EString m_Id;
		EString m_Tag;
		int m_Layer;
	};

}

