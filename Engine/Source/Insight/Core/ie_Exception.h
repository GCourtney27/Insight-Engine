#pragma once

#include <Insight/Core.h>

namespace Insight {

	class ieException
	{
	public:
		ieException(const char* Msg)
		{
			m_WhatMsg = Msg;
		}

		const char* What() const
		{
			return m_WhatMsg.c_str();
		}

	private:
		std::string m_WhatMsg;
	};

}