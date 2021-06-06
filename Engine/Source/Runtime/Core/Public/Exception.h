#pragma once

#include "EngineDefines.h"

namespace Insight 
{


	class INSIGHT_API ieException
	{
	public:
		enum EExceptionCategory
		{
			EC_Generic		=  1 << 0,
			EC_Application	=  1 << 1,
			EC_Rendering	=  1 << 2,
			EC_Scripting	=  1 << 3,
			EC_Physics		=  1 << 4,
			EC_Engine		=  1 << 5,
		};
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