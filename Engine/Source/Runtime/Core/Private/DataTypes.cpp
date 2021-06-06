#include <Engine_pch.h>

#include "Core/Public/DataTypes.h"

namespace Insight
{
	DataBlob::DataBlob()
		: m_DataSize(0)
	{
		m_ByteArray = make_shared<std::vector<UInt8>>();
	}
}
