#pragma once

#include <Runtime/Core.h>

#include "ActorComponent.h"
#include "Runtime/Graphics/Geometry/Model.h"

namespace Insight {

	namespace GameFramework {

		class SceneComponent;

		class INSIGHT_API StaticMeshComponent : public ActorComponent
		{
		public:
			struct EventData
			{
				EventCallbackFn EventCallback;
			};
		public:
			StaticMeshComponent(AActor* pOwner);
			virtual ~StaticMeshComponent();

			virtual bool LoadFromJson(const rapidjson::Value& jsonStaticMeshComponent) override;
			virtual bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer) override;

			virtual inline void SetEventCallback(const EventCallbackFn& callback) override { m_EventData.EventCallback = callback; }
			virtual void OnEvent(Event& e) override;

			virtual void OnInit() override;
			virtual void OnPostInit() {}
			virtual void OnDestroy() override;
			virtual void OnRender() override;
			virtual void OnUpdate(const float& deltaTime);
			virtual void OnChanged() {}
			virtual void OnImGuiRender() override;

			void RenderSceneHeirarchy() override;
			/*
				Attach a mesh to to be rendered.
				@param Path - Path to the mesh relative to the "Content/" directory.
			*/
			void AttachMesh(const FString& Path);
			void SetMaterial(Material* pMaterial);

			virtual void BeginPlay() override;
			virtual void EditorEndPlay() override;
			virtual void Tick(const float DeltaMs) override;

			virtual void OnAttach() override;
			virtual void OnDetach() override;

			inline void SetPosition(const FVector3& Pos) { m_pModel->GetMeshRootTransformRef().SetPosition(Pos); }
			inline void SetRotation(const FVector3& Rot) { m_pModel->GetMeshRootTransformRef().SetRotation(Rot); }
			inline void SetScale(const FVector3& Scale) { m_pModel->GetMeshRootTransformRef().SetScale(Scale); }

			inline void SetPosition(float X, float Y, float Z) { m_pModel->GetMeshRootTransformRef().SetPosition(X, Y, Z); }
			inline void SetRotation(float X, float Y, float Z) { m_pModel->GetMeshRootTransformRef().SetRotation(X, Y, Z); }
			inline void SetScale(float X, float Y, float Z) { m_pModel->GetMeshRootTransformRef().SetScale(X, Y, Z); }
		private:
			bool OnEventTranslation(TranslationEvent& e);

		private:
			StrongModelPtr m_pModel;
			Material* m_pMaterial;
			std::future<bool> m_ModelLoadFuture;

			SceneComponent* m_pParentTransformRef;

			uint32_t m_SMWorldIndex = 0U;
			EventData m_EventData;

		private:
			static uint32_t s_NumActiveSMComponents;
		};

	}// end namespace GameFramework
} // end namespace Insight
