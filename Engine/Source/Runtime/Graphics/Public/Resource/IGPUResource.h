#pragma once

#include "EngineDefines.h"
#include <Runtime/Graphics/Public/GraphicsCore.h>


namespace Insight
{
	namespace Graphics
	{
		
		class INSIGHT_API IGPUResource
		{
			friend class ICommandContext;
		public:
			//
			// Getters/Setters
			//
			FORCEINLINE void SetUsageState(EResourceState NewState) { m_UsageState = NewState; }
			FORCEINLINE void SetTransitionState(EResourceState NewState) { m_TransitioningState = NewState; }
			FORCEINLINE void SetVersionID(UInt32 NewID) { m_VersionID = NewID; }
			FORCEINLINE EResourceState GetUsageState() const { return m_UsageState; }
			FORCEINLINE EResourceState GetTransitionState() const { return m_TransitioningState; }
			FORCEINLINE UInt32 GetVersionID() const { return m_VersionID; }

		protected:
			IGPUResource(EResourceState CurrentState, EResourceState TransitionState)
				: m_UsageState(CurrentState)
				, m_TransitioningState(TransitionState)
				, m_VersionID(0u)
			{
			}
			IGPUResource() 
				: m_UsageState(RS_Common)
				, m_TransitioningState(RESOURCE_STATE_INVALID)
				, m_VersionID(0u)
			{
			}
			virtual ~IGPUResource() 
			{
			}
			virtual void Destroy() = 0;

			//virtual void InitializeGPUResource() = 0;

			EResourceState m_UsageState;
			EResourceState m_TransitioningState;
			UInt32 m_VersionID;
		};
	}
}
