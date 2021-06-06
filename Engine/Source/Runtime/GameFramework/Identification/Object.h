#pragma once
#include "GameFramework/Identification/ID.h"
#include "EngineDefines.h"

namespace Insight {

	// Base class for all Entitys
	class INSIGHT_API Object
	{
	public:
		Object(const ID& id = ID()) : m_id(id) {}
		virtual ~Object() {}

		bool operator == (const Object& object) const { return m_id == object.m_id; }
		bool operator != (const Object& object) const { return m_id != object.m_id; }

		bool operator == (const ID& id) const { return m_id == id; }
		bool operator != (const ID& id) const { return m_id != id; }

		const ID& GetID() { return m_id; }

	protected:
		ID m_id;
	};

}

