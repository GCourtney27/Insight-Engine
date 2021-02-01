#pragma once
#pragma once

#include <Runtime/Core.h>

#include "Runtime/GameFramework/AActor.h"
#include "Runtime/Rendering/Texture.h"

namespace Insight {

	class INSIGHT_API ASkyLight : public GameFramework::AActor
	{
	public:
		ASkyLight(ActorId id, GameFramework::ActorType type = "Sky Light Actor");
		virtual ~ASkyLight();

		virtual bool LoadFromJson(const rapidjson::Value* jsonSkyLight) override;
		bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>* Writer) override;

		virtual bool OnInit();
		virtual bool OnPostInit();
		virtual void OnUpdate(const float DeltaMs);
		virtual void OnPreRender(XMMATRIX parentMat);
		virtual void OnRender();
		virtual void Destroy();

		void OnEvent(Event& e);

		virtual void BeginPlay();
		virtual void Tick(const float DeltaMs);
		virtual void Exit();

		virtual void OnImGuiRender() override;

		void BindCubeMaps(bool RenderPassIsDeferred);

		/*CB_PS_SpotLight GetConstantBuffer() { return m_ShaderCB; }*/

	private:
		bool m_Enabled = true;
		
		Texture* m_Irradiance;
		Texture* m_Radiance;
		Texture* m_BrdfLUT;

	};


}
