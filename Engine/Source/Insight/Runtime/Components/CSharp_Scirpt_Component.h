#pragma once
#include <Insight/Core.h>

#include "Actor_Component.h"

namespace Insight {

	class INSIGHT_API CSharpScriptComponent : public ActorComponent
	{
	public:
		CSharpScriptComponent(StrongActorPtr pOwner);
		~CSharpScriptComponent();


	};

}
