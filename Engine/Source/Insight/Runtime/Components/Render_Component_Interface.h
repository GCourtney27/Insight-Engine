#pragma once

#include"Actor_Component.h"

namespace Insight {
    
    class RenderComponentInterface : public AActorComponent
    {
    public:
        //    virtual ComponentId VGetComponentId(void) const override { return COMPONENT_ID; }

            // RenderComponentInterface
        virtual shared_ptr<SceneNode> VGetSceneNode(void) = 0;
    };
}
