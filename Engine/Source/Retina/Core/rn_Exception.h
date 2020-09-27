#pragma once

#include <Retina/Core.h>

namespace Retina {

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