#pragma once

#include <Insight/Core.h>

#include "Insight/Runtime/Components/Actor_Component.h"

#include "Renderer/Platform/Windows/DirectX_Shared/Constant_Buffer_Types.h"
#include "Insight/Math/Transform.h"

namespace Insight {

	namespace Runtime {

		class SceneComponent;

		class INSIGHT_API PointLightComponent : public ActorComponent
		{
		public:
			struct EventData
			{
				EventCallbackFn EventCallback;

			};
		public:
			PointLightComponent(AActor* pOwner);
			virtual ~PointLightComponent();

			virtual bool LoadFromJson(const rapidjson::Value& jsonStaticMeshComponent) override;
			virtual bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer) override;

			virtual inline void SetEventCallback(const EventCallbackFn& callback) override { m_EventData.EventCallback = callback; }
			void OnEvent(Event& e);

			virtual void OnInit() override;
			virtual void OnPostInit() {}
			virtual void OnDestroy() override;
			virtual void OnRender() override;
			virtual void OnUpdate(const float& deltaTime);
			virtual void OnChanged() {}
			virtual void OnImGuiRender() override;

			void RenderSceneHeirarchy() override;

			virtual void BeginPlay() override;
			virtual void Tick(const float DeltaMs) override;

			virtual void OnAttach() override;
			virtual void OnDetach() override;

			inline CB_PS_PointLight GetConstantBuffer() { return m_ShaderCB; }

		private:
			EventData m_EventData;

			CB_PS_PointLight m_ShaderCB;
			float m_TempInnerCutoff = 12.5f;
			float m_TempOuterCutoff = 15.0f;

			SceneComponent* m_pOwnerSceneComponent;
			ieTransform m_Transform;
		};

	}

}
