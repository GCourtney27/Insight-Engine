#include <Engine_pch.h>

#include "Runtime/Math/Public/FVector.h"

namespace Insight
{

	const FVector FVector::One(1.0f, 1.0f, 1.0f);
	const FVector FVector::Zero(0.0f, 0.0f, 0.0f);
	const FVector FVector::Up(0.0f, 1.0f, 0.0f);
	const FVector FVector::Down(0.0f, -1.0f, 0.0f);
	const FVector FVector::Left(-1.0f, 0.0f, 0.0f);
	const FVector FVector::Right(1.0f, 0.0f, 0.0f);
	const FVector FVector::Forward(0.0f, 0.0f, 1.0f);
	const FVector FVector::Backward(0.0f, 0.0f, -1.0f);

}
