#pragma once

#include <Insight/Core.h>

namespace Insight {

	class INSIGHT_API HashedString
	{
	public:
		explicit HashedString(char const* const pIdentString)
			: m_Ident(HashName(pIdentString))
		{
		}

		unsigned long GetHashValue() const { return reinterpret_cast<unsigned long>(m_Ident); }
		const std::string& GetString() const { return m_IdentStr; }
		
		static void* HashName(const char* pIdentStr);

		bool operator < (HashedString const& obj) const
		{
			bool value = (GetHashValue() < obj.GetHashValue());
			return value;
		}

		bool operator== (HashedString const& o) const
		{
			bool r = (GetHashValue() == o.GetHashValue());
			return r;
		}

	private:
		void* m_Ident;
		std::string m_IdentStr;
	};
}
#pragma warning(pop)
