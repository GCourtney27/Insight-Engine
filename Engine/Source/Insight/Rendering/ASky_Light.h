#pragma once
#pragma once

#include <Insight/Core.h>

#include "Insight/Runtime/AActor.h"
#include "Insight/Rendering/Texture.h"

namespace Insight {

	class INSIGHT_API ASkyLight : public AActor
	{
	public:
		ASkyLight(ActorId id, ActorType type = "Sky Light Actor");
		virtual ~ASkyLight();

		virtual bool LoadFromJson(const rapidjson::Value& jsonSkyLight) override;
		bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer) override;

		virtual bool OnInit();
		virtual bool OnPostInit();
		virtual void OnUpdate(const float& deltaMs);
		virtual void OnPreRender(XMMATRIX parentMat);
		virtual void OnRender();
		virtual void Destroy();

		void OnEvent(Event& e);

		virtual void BeginPlay();
		virtual void Tick(const float& deltaMs);
		virtual void Exit();

		virtual void OnImGuiRender() override;

		void BindCubeMaps();

		/*CB_PS_SpotLight GetConstantBuffer() { return m_ShaderCB; }*/

	private:
		Texture* m_Irradiance;
		Texture* m_Environment;
		Texture* m_BrdfLUT;
	};


}
