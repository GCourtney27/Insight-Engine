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
			return m_WhatMsg;
		}

	private:
		const char* m_WhatMsg;
	};

}