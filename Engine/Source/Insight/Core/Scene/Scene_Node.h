#pragma once

#include "Insight/Core.h"

#include "Insight/Math/Transform.h"

namespace Insight {

	class INSIGHT_API SceneNode
	{
	public:
		SceneNode(AActor* actor = nullptr);
		~SceneNode();

		void SetParent(SceneNode* parent) { m_Parent = parent; }
		Transform& GetTransform() { return m_Transform; }
		AActor* GetActor() { return m_Actor; }
		void SetActor(AActor* mesh) { m_Actor = mesh; }
		void AddChild(SceneNode* childNode);
		std::vector<SceneNode*>::const_iterator GetChildIteratorStart() { return m_Children.begin(); }
		std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() { return m_Children.end(); }

		virtual void OnUpdate(const float& deltaMs);
		virtual void OnRender();

	private:
		SceneNode* m_Parent = nullptr;
		std::vector<SceneNode*> m_Children;
		Transform m_Transform;
		AActor* m_Actor = nullptr;
	};

}
