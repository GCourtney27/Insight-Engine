#pragma once
#pragma once

#include <Insight/Core.h>

#include "Insight/Runtime/AActor.h"
#include "Insight/Rendering/Texture.h"

namespace Insight {

	class INSIGHT_API ASkyLight : public Runtime::AActor
	{
	public:
		ASkyLight(ActorId id, Runtime::ActorType type = "Sky Light Actor");
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
