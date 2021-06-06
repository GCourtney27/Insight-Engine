#pragma once

#include "EngineDefines.h"
#include "Core/Public/Layer/Layer.h"

namespace Insight {


	class Scene;

	class INSIGHT_API GameLayer : public Layer
	{
	public:
		GameLayer();
		~GameLayer();

		virtual void OnAttach()			override;
		virtual void OnDetach()			override;
		virtual void OnImGuiRender()	override;

		virtual void OnUpdate(const float DeltaMs)	override;
		virtual void OnEvent(Event& event)			override;

		bool LoadScene(const std::string& FileName);

		inline Scene* GetScene() const	{ return m_pScene; }
		bool IsPlaySessionUnderWay()	{ return m_TickScene; }

		void BeginPlay();
		void Update(const float DeltaMs);
		void EndPlay();
		void PostInit();

	private:
		Scene* m_pScene;
		bool m_TickScene;
	};

}
