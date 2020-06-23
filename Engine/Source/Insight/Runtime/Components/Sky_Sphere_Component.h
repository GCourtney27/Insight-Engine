#pragma once

#include <Insight/Core.h>

#include "Actor_Component.h"


namespace Insight {

	class INSIGHT_API SkySphereComponent : public ActorComponent
	{
	public:
		SkySphereComponent(AActor* pOwner);
		virtual ~SkySphereComponent();
		
		virtual bool LoadFromJson(const rapidjson::Value& jsonActor) override;

		virtual void OnInit() override;
		virtual void OnPostInit() override;
		virtual void OnDestroy() override;
		virtual void OnPreRender(const DirectX::XMMATRIX& matrix)  override;
		virtual void OnRender() override;
		virtual void OnUpdate(const float& deltaTime) override;
		virtual void OnChanged() override;
		virtual void OnImGuiRender() override;
		virtual void RenderSceneHeirarchy() override;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

	private:
		Texture m_Diffuse;
		Texture m_Irradience;
		Texture m_EnvironmentMap;
		Texture m_brdfLUT;

	};

}
