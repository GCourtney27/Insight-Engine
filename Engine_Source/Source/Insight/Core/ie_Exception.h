#pragma once

#include <Insight/Core.h>

namespace Insight {

	class INSIGHT_API ieException
	{
	public:
		typedef enum _ExceptionCategory
		{
			ExceptionCategory_Generic		= BIT_SHIFT(0),
			ExceptionCategory_Application	= BIT_SHIFT(1),
			ExceptionCategory_Rendering		= BIT_SHIFT(2),
			ExceptionCategory_Scripting		= BIT_SHIFT(3),
			ExceptionCategory_Physics		= BIT_SHIFT(4),
			ExceptionCategory_Engine		= BIT_SHIFT(5),
		} ExceptionCategory;
	public:
		ieException() = delete;
		ieException(const char* Msg, ExceptionCategory Type = ExceptionCategory_Generic)
		{
			m_WhatMsg = Msg;
			m_Type = Type;
		}
		~ieException() = default;

		const char* What() const
		{
			return m_WhatMsg;
		}

	private:
		const char* m_WhatMsg;
		ExceptionCategory m_Type;
	};

}