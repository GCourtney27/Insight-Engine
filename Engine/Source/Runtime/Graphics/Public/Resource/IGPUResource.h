#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Public/GraphicsCore.h"

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
			FORCE_INLINE void SetUsageState(EResourceState NewState) { m_UsageState = NewState; }
			FORCE_INLINE void SetTransitionState(EResourceState NewState) { m_TransitioningState = NewState; }
			FORCE_INLINE void SetVersionID(UInt32 NewID) { m_VersionID = NewID; }
			FORCE_INLINE EResourceState GetUsageState() const { return m_UsageState; }
			FORCE_INLINE EResourceState GetTransitionState() const { return m_TransitioningState; }
			FORCE_INLINE UInt32 GetVersionID() const { return m_VersionID; }

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

			EResourceState m_UsageState;
			EResourceState m_TransitioningState;
			UInt32 m_VersionID;
		};
	}
}
