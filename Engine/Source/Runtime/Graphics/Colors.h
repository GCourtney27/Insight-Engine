#pragma once

#include <Runtime/CoreMacros.h>

#include "Runtime/Math/ie_Vectors.h"

namespace Colors
{
	typedef Insight::Math::ieFloat3 Color;

	static constexpr Insight::Math::ieFloat3 White(255.0f, 255.0f, 255.0f);
	static constexpr Insight::Math::ieFloat3 Black(0.0f, 0.0f, 0.0f);
	static constexpr Insight::Math::ieFloat3 Red(255.0f, 0.0f, 0.0f);
	static constexpr Insight::Math::ieFloat3 Green(0.0f, 255.0f, 0.0f);
	static constexpr Insight::Math::ieFloat3 Blue(0.0f, 0.0, 255.0f);
	static constexpr Insight::Math::ieFloat3 Yellow(255.0f, 255.0f, 0.0f);
	static constexpr Insight::Math::ieFloat3 Orange(255.0f, 110.0f, 0.0f);
}
