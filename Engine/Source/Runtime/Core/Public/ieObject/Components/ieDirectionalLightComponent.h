#pragma once

#include "EngineDefines.h"
#include "InsightStd.h"

#include "Core/Public/ieObject/Components/ieActorComponent.h"

#include "Math/Public/Vectors.h"
#include "Graphics/Public/WorldRenderer/RendererCommon.h"
#include "Graphics/Public/GraphicsCore.h"
#include "Graphics/Public/ResourceManagement/LightManager.h"

namespace Insight
{
	class INSIGHT_API ieDirectionalLightComponent : public ieActorComponent
	{
	public:
		ieDirectionalLightComponent(ieActor* pOwner)
			: ieActorComponent(pOwner)
		{
			DirectionalLightData* NewLight = NULL;
			Graphics::g_LightManager.AllocateDirectionalLightData(m_LightDataHandle, &NewLight);

			NewLight->Brightness = 2.f;
			NewLight->Direction = FVector4(0.f, 20.f, 20.f, 1.f);
		}

		inline FVector4 GetColor() const;
		inline float GetBrightness() const;
		inline void SetColor(float r, float g, float b);
		inline void SetBrightness(float Brightness);

	protected:
		DirectionalLightDataHandle m_LightDataHandle;
	};


	//
	// Inline Function Implementations
	//


	inline FVector4 ieDirectionalLightComponent::GetColor() const
	{
		if (m_LightDataHandle.IsValid())
		{
			DirectionalLightData* Light = Graphics::g_LightManager.GetDirectionalLightData(m_LightDataHandle);
			return Light->Color;
		}
		else
			return FVector4::Zero;
	}

	inline float ieDirectionalLightComponent::GetBrightness() const
	{
		if (m_LightDataHandle.IsValid())
		{
			DirectionalLightData* Light = Graphics::g_LightManager.GetDirectionalLightData(m_LightDataHandle);
			return Light->Brightness;
		}
		else
			return -1.f;
	}

	inline void ieDirectionalLightComponent::SetColor(float r, float g, float b)
	{
		if (m_LightDataHandle.IsValid())
		{
			DirectionalLightData* Light = Graphics::g_LightManager.GetDirectionalLightData(m_LightDataHandle);
			Light->Color.x = r;
			Light->Color.y = g;
			Light->Color.z = b;
		}
	}

	inline void ieDirectionalLightComponent::SetBrightness(float Brightness)
	{
		if (m_LightDataHandle.IsValid())
		{
			DirectionalLightData* Light = Graphics::g_LightManager.GetDirectionalLightData(m_LightDataHandle);
			Light->Brightness = Brightness;
		}

	}
}
