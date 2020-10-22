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
			inline void AttachTo(SceneComponent* pNewParent) { m_pParent = pNewParent; }
			// Remove the parent this scene component.
			inline void DetachParent() { m_pParent = nullptr; }

			inline void SetPosition(ieVector3 NewPosition) { m_Transform.SetPosition(NewPosition); }
			inline void SetRotation(ieVector3 NewRotation) { m_Transform.SetRotation(NewRotation); }
			inline void SetScale(ieVector3 NewScale) { m_Transform.SetScale(NewScale); }
			 
			inline void SetPosition(float X, float Y, float Z) { m_Transform.SetPosition({ X, Y, Z }); }
			inline void SetRotation(float X, float Y, float Z) { m_Transform.SetRotation({ X, Y, Z }); }
			inline void SetScale(float X, float Y, float Z) { m_Transform.SetScale({ X, Y, Z }); }
			 
			inline void SetPosition(float XYZ) { m_Transform.SetPosition({ XYZ, XYZ, XYZ }); }
			inline void SetRotation(float XYZ) { m_Transform.SetRotation({ XYZ, XYZ, XYZ }); }
			inline void SetScale(float XYZ) { m_Transform.SetScale({ XYZ, XYZ, XYZ }); }
			 
			inline void Translate(float X, float Y, float Z) { m_Transform.Translate(X, Y, Z); }
			inline void Rotate(float X, float Y, float Z) { m_Transform.Rotate(X, Y, Z); }
			inline void Scale(float X, float Y, float Z) { m_Transform.Scale(X, Y, Z); }

			inline ieVector3 GetPosition() { return m_Transform.GetPosition(); }
			inline ieVector3 GetRotation() { return m_Transform.GetRotation(); }
			inline ieVector3 GetScale() { return m_Transform.GetScale(); }
			
			inline ieVector3& GetPositionRef() { return m_Transform.GetPositionRef(); }
			inline ieVector3& GetRotationRef() { return m_Transform.GetRotationRef(); }
			inline ieVector3& GetScaleRef() { return m_Transform.GetScaleRef(); }

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
