#pragma once

#include <Retina/Core.h>

#include "Retina/Math/Transform.h"
#include "Actor_Component.h"

namespace Retina {


	namespace Runtime {

		class RETINA_API SceneComponent : public Runtime::ActorComponent
		{
		public:
			struct TranslationData
			{
				ieVector3 Translation = Vector3::Zero;
				ieVector3 Rotation = Vector3::Zero;
				ieVector3 Scale = Vector3::Zero;

				EventCallbackFn EventCallback;
			};

		public:
			SceneComponent(AActor* pOwner);
			~SceneComponent();

			virtual bool LoadFromJson(const rapidjson::Value& JsonComponent) override;
			virtual bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer) override;

			virtual void SetEventCallback(const EventCallbackFn& callback) override;
			virtual void OnEvent(Event& e) override;

			virtual void OnInit() override;
			virtual void OnPostInit() {}
			virtual void OnDestroy() override;
			virtual void CalculateParent(const DirectX::XMMATRIX& Matrix) override;
			virtual void OnRender() override;
			virtual void OnUpdate(const float& DeltaTime) {}
			virtual void OnChanged() {}
			// Sub-menus this component sould render when its owning actor is 
			// selected in the details panel. 
			// NOTE: This method should always 
			// push and pop a ID in imgui to prevent dupllicated component types
			// from editing eachother inadvertently.
			virtual void OnImGuiRender() override;
			virtual void RenderSceneHeirarchy() override;

			virtual void BeginPlay() override;
			virtual void Tick(const float DeltaMs) override;

			virtual void OnAttach() override;
			virtual void OnDetach() override;


			void SetPosition(float X, float Y, float Z) { m_Transform.SetPosition({ X, Y, Z }); }
			void SetRotation(float X, float Y, float Z) { m_Transform.SetRotation({ X, Y, Z }); }
			void SetScale(float X, float Y, float Z) { m_Transform.SetScale({ X, Y, Z }); }

			void Translate(float X, float Y, float Z) { m_Transform.Translate(X, Y, Z); }
			void Rotate(float X, float Y, float Z) { m_Transform.Rotate(X, Y, Z); }
			void Scale(float X, float Y, float Z) { m_Transform.Scale(X, Y, Z); }

			ieVector3 GetPosition() { return m_Transform.GetPosition(); }
			ieVector3 GetRotation() { return m_Transform.GetRotation(); }
			ieVector3 GetScale() { return m_Transform.GetScale(); }

			inline const ieTransform& GetTransform() const { return m_Transform; }
			inline ieTransform& GetTransformRef() { return m_Transform; }

		private:
			void RenderSelectionGizmo();
			
		private:
			ieTransform m_Transform;

			TranslationData m_TranslationData;
		};
	}
}
