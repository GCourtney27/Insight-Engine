#pragma once

#include "Insight/Core.h"

#include "Insight/Math/Transform.h"

namespace Insight {
	

	class INSIGHT_API SceneNode
	{
	public:
		SceneNode(std::string displayName = "Default Scene Node");
		~SceneNode();

		void SetParent(SceneNode* parent) { m_Parent = parent; }
		Transform& GetTransform() { return m_RootTransform; }
		const char* GetDisplayName() { return m_DisplayName.c_str(); }
		void SetDisplayName(std::string name) { m_DisplayName = name; }

		void AddChild(SceneNode* childNode);
		std::vector<SceneNode*>::const_iterator GetChildIteratorStart() { return m_Children.begin(); }
		std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() { return m_Children.end(); }

		virtual void RenderSceneHeirarchy();
		virtual bool OnInit();
		virtual bool OnPostInit();
		virtual void OnUpdate(const float& deltaMs);
		virtual void OnRender();
		virtual void Destroy();

		virtual void BeginPlay();
		virtual void Tick(const float& deltaMs);
		virtual void Exit();

	protected:
		SceneNode* m_Parent = nullptr;
		std::vector<SceneNode*> m_Children;
		Transform m_RootTransform;
		std::string m_DisplayName;
	};

}
