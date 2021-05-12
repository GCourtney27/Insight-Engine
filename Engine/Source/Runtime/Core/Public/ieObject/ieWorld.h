// Copyright Insight Interactive. All Rights Reserved.
#pragma once

#include <Runtime/Core.h>

#include "Runtime/Core/Public/ECS/ECS.h"
#include "Runtime/Graphics/Public/WorldRenderer/WorldRenderer.h"

namespace Insight
{
	class ieCameraActor;
	using BeginPlayFn = std::function<void()>;
	using TickFn = std::function<void(float)>;


	class INSIGHT_API ieWorld
	{
		friend class ieObjectBase;
		friend class Application;
	public:
		ieWorld() 
			: m_EntityAdmin()
			, m_WorldRenderer(this, m_EntityAdmin)
		{
		}
		~ieWorld() 
		{
			m_EntityAdmin.Flush();
			m_pCurrentCamera = NULL;
		}

		void Initialize(std::shared_ptr<Window> pWindow)
		{
			m_WorldRenderer.Initialize(pWindow, Graphics::RB_Direct3D12);
		}

		void BeginPlay()
		{
			UInt32 NumListeners = (UInt32)m_BeginPlayListeners.size();
			
			for (UInt32 i = 0; i < NumListeners; ++i)
				m_BeginPlayListeners[i]();
		}

		void Tick()
		{
			float DeltaMs = GetDeltaTime();
			UInt32 NumListeners = (UInt32)m_TickListeners.size();

			for (UInt32 i = 0; i < NumListeners; ++i)
				m_TickListeners[i](DeltaMs);
		}

		/*
			Register a function to receive tick events.
			@param Fn - Function to be called when world Tick is invoked.
		*/
		inline void AttachTickListener(TickFn Fn);

		/*
			Register a function to receive begin play events.
			@param Fn - Function to be called when world BeginPlay is invoked.
		*/
		inline void AttachBegiPlayListener(BeginPlayFn Fn);

		/*
			Returns a reference to the camera actor currently rendering frames to the main viewport.
		*/
		inline ieCameraActor* GetCurrentSceneCamera() const;

		/*
			Set the camera to render frames to the main viewport.
		*/
		inline void SetSceneCamera(ieCameraActor* pCamera);

		/*
			Returns the time between frame updates in milliseconds.
		*/
		inline float GetDeltaTime() const;

	protected:
		/*
			Renders the world to the main viewport.
		*/
		void Render()
		{
			m_WorldRenderer.Render();
			// m_PostEffectsRenderer.Render();
			// m_GameUI.Render();
		}

		/*
			Creates a entity instance in the entity admin and returns it.
		*/
		inline ECS::Entity_t CreateEntity();

		/*
			Destroys a instance of an entity in the wntity admin and released any 
			components it possesed.
		*/
		inline void DestroyEntity(ECS::Entity_t EntityWorldId);

	private:
		ECS::EntityAdmin m_EntityAdmin;
		WorldRenderer m_WorldRenderer;
		// TODO: PostEffectsRenderer m_PostEffectsRenderer;
		// TODO: UIRenderer m_GameUI;

		ieCameraActor* m_pCurrentCamera;

		std::vector<TickFn> m_TickListeners;
		std::vector<BeginPlayFn> m_BeginPlayListeners;
	};

	//
	// Inline function implementations
	//


	inline void ieWorld::AttachTickListener(TickFn Fn)
	{
		m_TickListeners.push_back(Fn);
	}

	inline void ieWorld::AttachBegiPlayListener(BeginPlayFn Fn)
	{
		m_BeginPlayListeners.push_back(Fn);
	}

	inline ECS::Entity_t ieWorld::CreateEntity()
	{
		return m_EntityAdmin.CreateEntity();
	}

	inline void ieWorld::DestroyEntity(ECS::Entity_t EntityWorldId)
	{
		m_EntityAdmin.DestroyEntity(EntityWorldId);
	}

	inline ieCameraActor* ieWorld::GetCurrentSceneCamera() const
	{
		return m_pCurrentCamera;
	}

	inline void ieWorld::SetSceneCamera(ieCameraActor* pCamera)
	{
		m_pCurrentCamera = pCamera;
	}

	inline float ieWorld::GetDeltaTime() const
	{
		return m_WorldRenderer.GetFrameTime();
	}
}