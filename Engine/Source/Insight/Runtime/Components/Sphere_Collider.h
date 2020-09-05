#pragma once

#include <Insight/Core.h>

#include "Insight/Core/Interfaces.h"

#include "Insight/Runtime/Components/Actor_Component.h"
#include "Insight/Physics/Physics_Common.h"

namespace Insight {



	class INSIGHT_API SphereColliderComponent : public IPhysicsObject, public ActorComponent
	{
	public:
		struct CollisionData
		{
			AActor* pCollider;

			EventCallbackFn EventCallback;
		};

	public:
		SphereColliderComponent(AActor* pOwner);
		virtual ~SphereColliderComponent();

		virtual bool LoadFromJson(const rapidjson::Value& JsonSphereColliderComponent) override;
		virtual bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer) override;
		
		virtual inline void SetEventCallback(const EventCallbackFn& callback) override { m_CollisionData.EventCallback = callback; }
		void OnEvent(Event& e);

		virtual void OnInit() override;
		virtual void OnPostInit() {}
		virtual void OnDestroy() override;
		virtual void CalculateParent(const DirectX::XMMATRIX& Matrix) override;
		virtual void OnRender() override;
		virtual void OnUpdate(const float& DeltaTime) {}
		virtual void OnChanged() {}
		virtual void OnImGuiRender() override;
		virtual void RenderSceneHeirarchy() override;

		virtual void BeginPlay() override;
		virtual void Tick(const float DeltaMs) override;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		float GetRadius() { return m_Radius; }
		void SetRadius(float Radius) { m_Radius = Radius; }


	private:
		float m_Radius = 0.0f;
		CollisionData m_CollisionData;

		uint32_t m_SphereColliderWorldIndex = 0U;
	private:
		static uint32_t s_NumActiveSphereColliderComponents;
	};

}