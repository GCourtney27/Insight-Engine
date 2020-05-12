#pragma once

#include <SimpleMath.h>
using namespace DirectX::SimpleMath;

namespace Insight {


	class AActor;
	class ActorComponent;

	typedef unsigned int ActorId;
	typedef unsigned int ComponentId;

	const ActorId INVALID_ACTOR_ID = 0;
	const ComponentId INVALID_COMPONENT_ID = 0;

	typedef shared_ptr<AActor> StrongActorPtr;
	typedef weak_ptr<AActor> WeakActorPtr;
	typedef shared_ptr<ActorComponent> StrongActorComponentPtr;
	typedef weak_ptr<ActorComponent> WeakActorComponentPtr;

	enum RenderPass
	{
		RenderPass_0,
		RenderPass_Static = RenderPass_0,
		RenderPass_Actor,
		RenderPass_Sky,
		RenderPass_NotRendered,
		RenderPass_Last
	};

	class Scene;
	class SceneNodeProperties;

	class ISceneNode
	{
	public:
		virtual const SceneNodeProperties* const VGet() const = 0;

		virtual void VSetTransform(const Matrix* toWorld, const Matrix* fromWorld = NULL) = 0;

		virtual HRESULT VOnUpdate(Scene* pScene, DWORD const elapsedMs) = 0;
		virtual HRESULT VOnRestore(Scene* pScene) = 0;

		virtual HRESULT VPreRender(Scene* pScene) = 0;
		virtual bool VIsVisible(Scene* pScene) const = 0;
		virtual HRESULT VRender(Scene* pScene) = 0;
		virtual HRESULT VRenderChildren(Scene* pScene) = 0;
		virtual HRESULT VPostRender(Scene* pScene) = 0;

		virtual bool VAddChild(shared_ptr<ISceneNode> kid) = 0;
		virtual bool VRemoveChild(ActorId id) = 0;
		virtual HRESULT VOnLostDevice(Scene* pScene) = 0;
		//virtual HRESULT VPick(Scene* pScene, RayCast* pRayCast) = 0;


		virtual ~ISceneNode() { };
	};

	class IRenderState
	{
	public:
		virtual std::string VToString() = 0;
	};

	class IRenderer
	{
	public:
		virtual void VSetBackgroundColor(BYTE bgA, BYTE bgR, BYTE bgG, BYTE bgB) = 0;
		virtual HRESULT VOnRestore() = 0;
		virtual void VShutdown() = 0;
		virtual bool VPreRender() = 0;
		virtual bool VPostRender() = 0;
		//virtual void VCalcLighting(Lights* lights, int maximumLights) = 0;
		virtual void VSetWorldTransform(const Matrix* m) = 0;
		virtual void VSetViewTransform(const Matrix* m) = 0;
		virtual void VSetProjectionTransform(const Matrix* m) = 0;
		virtual shared_ptr<IRenderState> VPrepareAlphaPass() = 0;
		virtual shared_ptr<IRenderState> VPrepareSkyBoxPass() = 0;
		virtual void VDrawLine(const Vector3& from, const Vector3& to, const Color& color) = 0;
	};

}
