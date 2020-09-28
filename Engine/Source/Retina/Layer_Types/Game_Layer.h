#pragma once

#include <Retina/Core.h>
#include "Retina/Core/Layer/Layer.h"

namespace Retina {


	class Scene;

	class RETINA_API GameLayer : public Layer
	{
	public:
		GameLayer();
		~GameLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		virtual void OnUpdate(const float DeltaMs) override;
		void OnEvent(Event& event) override;

		bool LoadScene(const std::string& FileName);

		Scene* GetScene() const { return m_pScene; }
		bool IsPlaySesionUnderWay() { return m_TickScene; }

		void BeginPlay();
		void Update(const float DeltaMs);
		void PreRender();
		void Render();
		void EndPlay();
		void PostInit();

	private:
		Scene* m_pScene = nullptr;
		bool m_TickScene = false;
	};

}
