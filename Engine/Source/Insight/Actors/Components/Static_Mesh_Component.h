#pragma once

#include <Insight/Core.h>

#include "Actor_Component.h"
#include "Insight/Rendering/Geometry/Model.h"

namespace Insight {

	namespace Runtime {

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
			void OnEvent(Event& e);

			virtual void OnInit() override;
			virtual void OnPostInit() {}
			virtual void OnDestroy() override;
			virtual void OnRender() override;
			virtual void OnUpdate(const float& deltaTime);
			virtual void OnChanged() {}
			virtual void OnImGuiRender() override;

			void RenderSceneHeirarchy() override;
			void AttachMesh(const std::string& AssesDirectoryRelPath);
			void SetMaterial(Material* pMaterial);

			virtual void BeginPlay() override;
			virtual void EditorEndPlay() override;
			virtual void Tick(const float DeltaMs) override;

			virtual void OnAttach() override;
			virtual void OnDetach() override;

			inline void SetPosition(ieVector3& Pos) { m_pModel->GetMeshRootTransformRef().SetPosition(Pos.x, Pos.y, Pos.z); }
			inline void SetRotation(ieVector3& Rot) { m_pModel->GetMeshRootTransformRef().SetRotation(Rot.x, Rot.y, Rot.z); }
			inline void SetScale(ieVector3& Sca) { m_pModel->GetMeshRootTransformRef().SetScale(Sca.x, Sca.y, Sca.z); }
		private:
			bool OnEventTranslation(TranslationEvent& e);

		private:
			std::string m_DynamicAssetDir;
			StrongModelPtr m_pModel;
			Material* m_pMaterial;
			std::future<bool> m_ModelLoadFuture;

			SceneComponent* m_pParentTransformRef;

			uint32_t m_SMWorldIndex = 0U;
			EventData m_EventData;

		private:
			static uint32_t s_NumActiveSMComponents;
		};

	}// end namespace Runtime
} // end namespace Insight
