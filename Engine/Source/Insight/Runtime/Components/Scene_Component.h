#pragma once

#include <Insight/Core.h>

#include "Insight/Math/Transform.h"
#include "Actor_Component.h"

namespace Insight {


	namespace Runtime {

		class ACamera;

		class INSIGHT_API SceneComponent : public Runtime::ActorComponent
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
			virtual void OnPostInit();
			virtual void OnDestroy() override;
			virtual void OnRender() override;
			virtual void OnUpdate(const float& DeltaTime);
			virtual void OnChanged() {}
			// Sub-menus this component sould render when its owning actor is 
			// selected in the details panel. 
			// NOTE: This method should always 
			// push and pop a ID in imgui to prevent dupllicated component types
			// from editing eachother inadvertently.
			virtual void OnImGuiRender() override;
			virtual void RenderSceneHeirarchy() override;

			virtual void BeginPlay() override;
			virtual void EditorEndPlay() override;
			virtual void Tick(const float DeltaMs) override;

			virtual void OnAttach() override;
			virtual void OnDetach() override;

			// Set the parent of transform this scene component will translate relative too.
			void AttachTo(SceneComponent* pNewParent) { m_pParent = pNewParent; }
			// Remove the parent this scene component.
			void DetachParent() { m_pParent = nullptr; }

			void SetPosition(ieVector3 NewPosition) { m_Transform.SetPosition(NewPosition); }
			void SetRotation(ieVector3 NewRotation) { m_Transform.SetRotation(NewRotation); }
			void SetScale(ieVector3 NewScale) { m_Transform.SetScale(NewScale); }

			void SetPosition(float X, float Y, float Z) { m_Transform.SetPosition({ X, Y, Z }); }
			void SetRotation(float X, float Y, float Z) { m_Transform.SetRotation({ X, Y, Z }); }
			void SetScale(float X, float Y, float Z) { m_Transform.SetScale({ X, Y, Z }); }

			void Translate(float X, float Y, float Z) { m_Transform.Translate(X, Y, Z); }
			void Rotate(float X, float Y, float Z) { m_Transform.Rotate(X, Y, Z); }
			void Scale(float X, float Y, float Z) { m_Transform.Scale(X, Y, Z); }

			ieVector3 GetPosition() { return m_Transform.GetPosition(); }
			ieVector3 GetRotation() { return m_Transform.GetRotation(); }
			ieVector3 GetScale() { return m_Transform.GetScale(); }

			ieVector3& GetPositionRef() { return m_Transform.GetPositionRef(); }
			ieVector3& GetRotationRef() { return m_Transform.GetRotationRef(); }
			ieVector3& GetScaleRef() { return m_Transform.GetScaleRef(); }

			inline const ieTransform& GetTransform() const { return m_Transform; }
			inline ieTransform& GetTransformRef() { return m_Transform; }

		private:
			void RenderSelectionGizmo();
			
		private:
			ieTransform m_Transform;
			ieTransform m_EditorTransform;

			bool m_IsStatic = false;
			TranslationData m_TranslationData;
			
			SceneComponent* m_pParent = nullptr;

		};
	}
}
