#pragma once

#include <Insight/Core.h>

namespace Insight {

	class ieException
	{
	public:
		ieException() = default;
		ieException(const char* Msg)
		{
			m_WhatMsg = Msg;
		}
		~ieException() = default;

		const char* What() const
		{
			return m_WhatMsg;
		}

	private:
		const char* m_WhatMsg;
	};

}