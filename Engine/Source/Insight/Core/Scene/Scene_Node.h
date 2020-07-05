#pragma once

#include "Insight/Core.h"

#include "Insight/Math/Transform.h"

namespace Insight {
	
	class Scene;

	class INSIGHT_API SceneNode
	{
	public:
		SceneNode(std::string displayName = "Default Scene Node");
		virtual ~SceneNode();

		void SetParent(SceneNode* parent) 
		{
			if (m_Parent) { m_Parent = nullptr; } 
			m_Parent = parent;
		}

		const Transform& GetTransform() { return m_RootTransform; }
		Transform& GetTransformRef() { return m_RootTransform; }
		const char* GetDisplayName() { return m_DisplayName.c_str(); }
		void SetDisplayName(std::string Name) { m_DisplayName = Name; }
		void SetCanBeFileParsed(bool CanBeParsed) { m_CanBeFileParsed = CanBeParsed; }

		void AddChild(SceneNode* childNode);
		std::vector<SceneNode*>::const_iterator GetChildIteratorStart() { return m_Children.begin(); }
		std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() { return m_Children.end(); }

		virtual bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer);
		virtual bool LoadFromJson(const rapidjson::Value& JsonActor);

		virtual void RenderSceneHeirarchy();
		virtual bool OnInit();
		virtual bool OnPostInit();
		virtual void OnUpdate(const float& DeltaMs);
		virtual void OnPreRender(XMMATRIX ParentMat);
		virtual void OnRender();
		virtual void Destroy();

		virtual void BeginPlay();
		virtual void Tick(const float& DeltaMs);
		virtual void Exit();

		virtual void EditorEndPlay();

		std::vector<SceneNode*> m_Children;
	protected:
		SceneNode* m_Parent = nullptr;
		Transform m_RootTransform;
		std::string m_DisplayName;
		bool m_CanBeFileParsed = true;
	};

}
