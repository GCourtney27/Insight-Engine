#pragma once

#include <Runtime/CoreMacros.h>

namespace Insight {

	class INSIGHT_API ieException
	{
	public:
		typedef enum EExceptionCategory
		{
			EC_Generic		= BIT_SHIFT(0),
			EC_Application	= BIT_SHIFT(1),
			EC_Rendering	= BIT_SHIFT(2),
			EC_Scripting	= BIT_SHIFT(3),
			EC_Physics		= BIT_SHIFT(4),
			EC_Engine		= BIT_SHIFT(5),
		} EExceptionCategory;
	public:
		ieException() = delete;
		ieException(const TChar* Msg, EExceptionCategory Type = EC_Generic)
		{
			m_WhatMsg = Msg;
			m_Type = Type;
		}
		~ieException() = default;

		const TChar* What() const
		{
			return m_WhatMsg;
		}

	private:
		const TChar* m_WhatMsg;
		EExceptionCategory m_Type;
	};

}