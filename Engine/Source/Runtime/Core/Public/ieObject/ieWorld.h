// Copyright Insight Interactive. All Rights Reserved.
#pragma once

#include "EngineDefines.h"

#include "Core/Public/ECS/ECS.h"

#include "Graphics/Public/WorldRenderer/WorldRenderer.h"
#include "Core/Public/ComponentSystems/CameraSystem.h"


namespace Insight
{
	using BeginPlayFn = std::function<void()>;
	using TickFn = std::function<void(float)>;

	class ieCameraComponent;

	class INSIGHT_API ieWorld
	{
		friend class ieActor;
		friend class ieObjectBase;
		friend class Engine;
	public:
		ieWorld() 
			: m_EntityAdmin()
			, m_WorldRenderer(this, m_EntityAdmin)
			, m_CameraSystem(m_EntityAdmin)
		{
		}
		~ieWorld() 
		{
			m_EntityAdmin.Flush();
			m_pCurrentRenderCamera = NULL;

			for (UInt32 i = 0; i < m_WorldActors.size(); ++i)
			{
				delete m_WorldActors[i];
			}
		}

		void Initialize(std::shared_ptr<Window> pWindow)
		{
			m_WorldRenderer.Initialize(pWindow, Graphics::RB_Direct3D12);
		}

		void BeginPlay()
		{
			const UInt32 kNumListeners = (UInt32)m_BeginPlayListeners.size();
			
			for (UInt32 i = 0; i < kNumListeners; ++i)
				m_BeginPlayListeners[i]();
		}

		void ExecuteCoreSystems()
		{
			m_CameraSystem.Execute();

		}

		void ExecuteGameplaySystems()
		{
		}

		void TickWorld(float TimeStep)
		{
			m_AppDeltaTime = TimeStep;

			const UInt32 kNumListeners = (UInt32)m_TickListeners.size();

			for (UInt32 i = 0; i < kNumListeners; ++i)
				m_TickListeners[i](TimeStep);
		}

#if !IE_CACHEOPTIMIZED_ECS_ENABLED
		template <typename ActorClass>
		ActorClass* CreateActor()
		{
			ieActor* pNewClass = new ActorClass(this);
			m_WorldActors.push_back(pNewClass);
			return SCast<ActorClass*>(pNewClass);
		}
#endif

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
		inline ieCameraComponent* GetCurrentSceneRenderCamera() const;

		/*
			Set the camera to render frames to the main viewport.
		*/
		inline void SetSceneRenderCamera(ieCameraComponent* pCamera);

		/*
			Returns the time between each frame in milliseconds.
		*/
		inline float ieWorld::GetDeltaTime();

		inline std::vector<ieActor*>& GetAllActors()
		{
			return m_WorldActors;
		}

	protected:
		/*
			Renders the world to the main viewport.
		*/
		void Render()
		{
			m_WorldRenderer.Render();
			// m_PostEffectsRenderer.Render();
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

		inline ECS::EntityAdmin& GetEntityAdmin()
		{
			return m_EntityAdmin;
		}

	private:
		ECS::EntityAdmin m_EntityAdmin;
		WorldRenderer m_WorldRenderer;
		// TODO: PostEffectsRenderer m_PostEffectsRenderer;
		CameraSystem m_CameraSystem;
		float m_AppDeltaTime;

		ieCameraComponent* m_pCurrentRenderCamera;

		std::vector<TickFn> m_TickListeners;
		std::vector<BeginPlayFn> m_BeginPlayListeners;

		std::vector<ieActor*> m_WorldActors;
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

	inline ieCameraComponent* ieWorld::GetCurrentSceneRenderCamera() const
	{
		return m_pCurrentRenderCamera;
	}

	inline void ieWorld::SetSceneRenderCamera(ieCameraComponent* pCamera)
	{
		m_pCurrentRenderCamera = pCamera;
	}

	inline float ieWorld::GetDeltaTime()
	{
		return m_AppDeltaTime;
	}
}