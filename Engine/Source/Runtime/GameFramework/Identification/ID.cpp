#include <Engine_pch.h>
#include "ID.h"

namespace Insight {

	size_t ID::ms_uniqueID = 0;

	ID::ID()
	{
		m_Id = GetUniqueID();
	}

	EString ID::GetUniqueID()
	{
		return (TEXT("uid_") + ToString(ms_uniqueID++));
	}

}

