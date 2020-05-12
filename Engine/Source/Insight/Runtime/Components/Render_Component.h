#pragma once

#include <Insight/Core.h>

#include "Render_Component_Interface.h"
#include "Insight/Core/Scene/Scene_Nodes.h"

namespace Insight {

    class BaseRenderComponent : public RenderComponentInterface
    {
    protected:
        //Color m_color;
        shared_ptr<SceneNode> m_pSceneNode;

    public:
        virtual bool VInit(TiXmlElement* pData) override;
        virtual void VPostInit(void) override;
        virtual void VOnChanged(void) override;
        virtual TiXmlElement* VGenerateXml(void) override;
        const Color GetColor() const { return m_color; }

    protected:
        // loads the SceneNode specific data (represented in the <SceneNode> tag)
        virtual bool VDelegateInit(TiXmlElement* pData) { return true; }
        virtual shared_ptr<SceneNode> VCreateSceneNode(void) = 0;  // factory method to create the appropriate scene node
        Color LoadColor(TiXmlElement* pData);

        // editor stuff
        virtual TiXmlElement* VCreateBaseElement(void) { return GCC_NEW TiXmlElement(VGetName()); }
        virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement) = 0;

    private:
        virtual shared_ptr<SceneNode> VGetSceneNode(void) override;
    };

}
