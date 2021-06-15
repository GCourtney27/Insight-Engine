#pragma once

#include "EngineDefines.h"
#include "InsightStd.h"

#include "Core/Public/ieObject/Components/ieActorComponent.h"

#include "Math/Public/Vectors.h"
#include "Math/Public/Transform.h"
#include "Graphics/Public/WorldRenderer/RendererCommon.h"
#include "Graphics/Public/GraphicsCore.h"
#include "Graphics/Public/ResourceManagement/LightManager.h"

namespace Insight
{
	class ieObjectBase;


	class INSIGHT_API iePointLightComponent : public ieActorComponent/*<iePointLightComponent>*/
	{
	public:
		iePointLightComponent(ieActor* pOwner)
			: ieActorComponent(pOwner)
		{
			PointLightData* NewLight = NULL;
			Graphics::g_LightManager.AllocatePointLightData(m_LightDataHandle , &NewLight);

			if (NewLight != NULL)
			{
				NewLight->Brightness = kDefaultBrightness;
				NewLight->Color = FVector4::One;
				NewLight->Position.x = m_Transform.GetPosition().x;
				NewLight->Position.y = m_Transform.GetPosition().y;
				NewLight->Position.z = m_Transform.GetPosition().z;
			}
		}

		inline ieTransform& GetTransform();
		inline FVector4 GetColor() const;
		inline float GetBrightness() const;
		inline void SetColor(float r, float g, float b);
		inline void SetBrightness(float Brightness);

	protected:
		PointLightDataHandle m_LightDataHandle;
		ieTransform m_Transform;
	};


	//
	// Inline Function Implementations
	//


	inline ieTransform& iePointLightComponent::GetTransform()
	{
		return m_Transform;
	}

	inline FVector4 iePointLightComponent::GetColor() const
	{
		if (m_LightDataHandle.IsValid())
		{
			PointLightData* Light = Graphics::g_LightManager.GetPointLightData(m_LightDataHandle);
			return Light->Color;
		}
		else
			return FVector4::Zero;
	}

	inline float iePointLightComponent::GetBrightness() const
	{
		if (m_LightDataHandle.IsValid())
		{
			PointLightData* Light = Graphics::g_LightManager.GetPointLightData(m_LightDataHandle);
			return Light->Brightness;
		}
		else
			return -1.f;
	}

	inline void iePointLightComponent::SetColor(float r, float g, float b)
	{
		if (m_LightDataHandle.IsValid())
		{
			PointLightData* Light = Graphics::g_LightManager.GetPointLightData(m_LightDataHandle);
			Light->Color.x = r;
			Light->Color.y = g;
			Light->Color.z = b;
		}
	}

	inline void iePointLightComponent::SetBrightness(float Brightness)
	{
		if (m_LightDataHandle.IsValid())
		{
			PointLightData* Light = Graphics::g_LightManager.GetPointLightData(m_LightDataHandle);
			Light->Brightness = Brightness;
		}
	}
}