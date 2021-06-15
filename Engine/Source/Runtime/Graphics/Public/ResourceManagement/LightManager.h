#pragma once

#include "EngineDefines.h"
#include "InsightStd.h"

#include "Graphics/Public/WorldRenderer/RendererCommon.h"

namespace Insight
{

	class INSIGHT_API LightManager
	{
	public:
		LightManager() = default;
		~LightManager() = default;

		/*
			Allocates a spot light and adds it to the scene. Populates a handle to the newly created point light.
			@param [out] OutHandle - Handle to the newly allocated light.
			@param [out] ppOutLight - Optional pointer to the light that will be created. Only for use with initialization.
		*/
		inline void AllocateSpotLightData(SpotLightDataHandle& OutHandle, SpotLightData** ppOutLight);

		/*
			Allocates a point light and adds it to the scene. Populates a handle to the newly created point light.
			@param [out] OutHandle - Handle to the newly allocated light.
			@param [out] ppOutLight - Optional pointer to the light that will be created. Only for use with initialization.
		*/
		inline void AllocatePointLightData(PointLightDataHandle& OutHandle, PointLightData** ppOutLight);

		/*
			Allocates a directional light and adds it to the scene. Populates a handle to the newly created point light.
			@param [out] OutHandle - Handle to the newly allocated light.
			@param [out] ppOutLight - Optional pointer to the light that will be created. Only for use with initialization.
		*/
		inline void AllocateDirectionalLightData(DirectionalLightDataHandle& OutHandle, DirectionalLightData** ppOutLight);


		inline SpotLightData* GetSpotLightData(SpotLightDataHandle Handle);
		inline PointLightData* GetPointLightData(PointLightDataHandle Handle);
		inline DirectionalLightData* GetDirectionalLightData(DirectionalLightDataHandle Handle);

		inline SpotLightData* GetSpotLightBufferPointer();
		inline PointLightData* GetPointLighBufferPointer();
		inline DirectionalLightData* GetDirectionalLightBufferPointer();

		inline UInt32 GetSceneSpotLightCount();
		inline UInt32 GetScenePointLightCount();
		inline UInt32 GetSceneDirectionalLightCount();

	private:
		inline void InitializeSpotLightData(SpotLightData& Light);
		inline void InitializePointLightData(PointLightData& Light);
		inline void InitializeDirectionalLightData(DirectionalLightData& Light);

		template <typename LightType, typename LightHandleType>
		inline LightType* InternalFindLightByHandle(std::vector<LightType> LightBuffer, const LightHandleType& Handle);

	private:
		std::vector<SpotLightData>			m_SceneSpotLightDatas;
		std::vector<PointLightData>			m_ScenePointLightDatas;
		std::vector<DirectionalLightData>	m_SceneDirectionalLightDatas;

		SpotLightDataHandle					s_NextSpotLightDatahandle;
		PointLightDataHandle				s_NextPointLightDataHandle;
		DirectionalLightDataHandle			s_NextDirectionalLightDataHandle;

	};

	//
	// Inline Function Implementations
	//


	inline void LightManager::AllocateSpotLightData(SpotLightDataHandle& OutHandle, SpotLightData** pOutLight)
	{
		if (m_SceneSpotLightDatas.size() == IE_MAX_SPOT_LIGHTS)
		{
			IE_LOG(Warning, TEXT("Too many spot lights added to the scene!"));

			*pOutLight = NULL;
			OutHandle = IE_INVALID_SPOT_LIGHT_HANDLE;
		}
		SpotLightData& NewLight = m_SceneSpotLightDatas.emplace_back(SpotLightData{});
		InitializeSpotLightData(NewLight);
		NewLight.Id = s_NextSpotLightDatahandle++;
		
		if(pOutLight != NULL)
			*pOutLight = &NewLight;

		OutHandle = NewLight.Id;
	}

	inline void LightManager::AllocatePointLightData(PointLightDataHandle& OutHandle, PointLightData** pOutLight)
	{
		if (m_ScenePointLightDatas.size() == IE_MAX_POINT_LIGHTS)
		{
			IE_LOG(Warning, TEXT("Too many point lights added to the scene!"));

			*pOutLight = NULL;
			OutHandle = IE_INVALID_POINT_LIGHT_HANDLE;
		}
		PointLightData& NewLight = m_ScenePointLightDatas.emplace_back(PointLightData{});
		InitializePointLightData(NewLight);
		NewLight.Id = s_NextPointLightDataHandle++;

		if (pOutLight != NULL)
			*pOutLight = &NewLight;

		OutHandle = NewLight.Id;
	}

	inline void LightManager::AllocateDirectionalLightData(DirectionalLightDataHandle& OutHandle, DirectionalLightData** pOutLight)
	{
		if (m_SceneDirectionalLightDatas.size() == IE_MAX_DIRECTIONAL_LIGHTS)
		{
			IE_LOG(Warning, TEXT("Too many directional lights added to the scene!"));
			*pOutLight = NULL;
			OutHandle = IE_INVALID_DIRECTIONAL_LIGHT_HANDLE;
		}
		DirectionalLightData& NewLight = m_SceneDirectionalLightDatas.emplace_back(DirectionalLightData{});
		InitializeDirectionalLightData(NewLight);
		NewLight.Id = s_NextDirectionalLightDataHandle++;

		if (pOutLight != NULL)
			*pOutLight = &NewLight;

		OutHandle = NewLight.Id;
	}

	inline SpotLightData* LightManager::GetSpotLightData(SpotLightDataHandle Handle)
	{
		return InternalFindLightByHandle(m_SceneSpotLightDatas, Handle);
	}

	inline PointLightData* LightManager::GetPointLightData(PointLightDataHandle Handle)
	{
		return InternalFindLightByHandle(m_ScenePointLightDatas, Handle);
	}

	inline DirectionalLightData* LightManager::GetDirectionalLightData(DirectionalLightDataHandle Handle)
	{
		return InternalFindLightByHandle(m_SceneDirectionalLightDatas, Handle);
	}

	template <typename LightType, typename LightHandleType>
	inline LightType* LightManager::InternalFindLightByHandle(std::vector<LightType> LightBuffer, const LightHandleType& LightHandle)
	{
		IE_ASSERT(LightHandle.IsValid()); // Invalid handle provided when searching for corresponding light!
		
		auto Iter = std::find_if(LightBuffer.begin(), LightBuffer.end(), [&](LightType& Light)
		{
			return Light.Id == LightHandle;
		});
		if (Iter != LightBuffer.end())
			return &(*Iter);

		return NULL;
	}

	inline PointLightData* LightManager::GetPointLighBufferPointer()
	{
		return m_ScenePointLightDatas.data();
	}

	inline UInt32 LightManager::GetScenePointLightCount()
	{
		return (UInt32)m_ScenePointLightDatas.size();
	}

	inline SpotLightData* LightManager::GetSpotLightBufferPointer()
	{
		return m_SceneSpotLightDatas.data();
	}

	inline UInt32 LightManager::GetSceneSpotLightCount()
	{
		return (UInt32)m_SceneSpotLightDatas.size();
	}

	inline DirectionalLightData* LightManager::GetDirectionalLightBufferPointer()
	{
		return m_SceneDirectionalLightDatas.data();
	}

	inline UInt32 LightManager::GetSceneDirectionalLightCount()
	{
		return (UInt32)m_SceneDirectionalLightDatas.size();
	}

	inline void LightManager::InitializeSpotLightData(SpotLightData& Light)
	{

	}

	inline void LightManager::InitializePointLightData(PointLightData& Light)
	{
		ZeroMemRanged(&Light, sizeof(PointLightData));
		Light.Brightness = kDefaultBrightness;
		Light.Color = FVector4::One;
	}

	inline void LightManager::InitializeDirectionalLightData(DirectionalLightData& Light)
	{
		ZeroMemRanged(&Light, sizeof(DirectionalLightData));
		Light.Brightness = kDefaultBrightness;
		Light.Color = FVector4::One;
		Light.Direction = FVector4::One;
	}
}
