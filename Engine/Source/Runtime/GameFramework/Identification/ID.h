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
		ID(const FString& id)
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
		static FString GetUniqueID();
		void SetUniqueID(const FString& id) { m_Id = id; }

		FString& GetName() { return m_Name; }
		void SetName(const FString& name) { m_Name = name; }

		FString& GetType() { return m_Type; }
		void SetType(const FString& type) { m_Type = type; }

		FString& GetTag() { return m_Tag; }
		void SetTag(const FString& tag) { m_Tag = tag; }

		void SetLayer(const int& layer) { m_Layer = layer; }
		int GetLayer() const { return m_Layer; }

	protected:
		FString m_Type;
		FString m_Name;
		FString m_Id;
		FString m_Tag;
		int m_Layer;
	};

}

