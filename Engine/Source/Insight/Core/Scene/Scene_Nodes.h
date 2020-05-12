#pragma once

using namespace DirectX::SimpleMath;


namespace Insight {


	class SceneNode;
	class Scene;

	enum AlphaType
	{
		AlphaOpaque,
		AlphaTexture,
		AlphaMaterial,
		AlphaVertex
	};

	class SceneNodeProperties
	{
		friend class SceneNode;

	protected:
		ActorId                 m_ActorId;
		std::string				m_Name;
		Matrix					m_ToWorld, m_FromWorld;
		float					m_Radius;
		RenderPass				m_RenderPass;
		//Material				m_Material;
		AlphaType				m_AlphaType;

		void SetAlpha(const float alpha)
		{
			m_AlphaType = AlphaMaterial;// m_Material.SetAlpha(alpha);
		}

	public:
		SceneNodeProperties();
		const ActorId& ActorId() const { return m_ActorId; }
		Matrix const& ToWorld() const { return m_ToWorld; }
		Matrix const& FromWorld() const { return m_FromWorld; }
		void Transform(Matrix* toWorld, Matrix* fromWorld) const;

		const char* Name() const { return m_Name.c_str(); }

		//bool HasAlpha() const { return m_Material.HasAlpha(); }
		//float Alpha() const { return m_Material.GetAlpha(); }
		AlphaType AlphaType() const { return m_AlphaType; }

		RenderPass RenderPass() const { return m_RenderPass; }
		float Radius() const { return m_Radius; }

		//Material GetMaterial() const { return m_Material; }
	};

	typedef std::vector<shared_ptr<ISceneNode> > SceneNodeList;


}

