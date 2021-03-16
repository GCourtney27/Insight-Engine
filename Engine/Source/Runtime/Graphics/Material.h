#pragma once

#include <Runtime/Core.h>

#include "Runtime/Graphics/Texture.h"
#include "Platform/DirectXShared/ConstantBufferTypes.h"

namespace Insight {

	class INSIGHT_API Material
	{
	public:
		enum EMaterialType
		{
			MT_Invalid = -1,
			MT_Opaque = 0,
			MT_Translucent = 1,
		};
	public:
		Material();
		/*
		@param TextureMangerIds - 0: Albedo, 1: Normal, 2: Metallic, 3: Roughness, 4:AO
		*/
		Material(std::array<Texture::ID, 5> TextureMangerIds);
		Material(Material&& material) noexcept;
		~Material();
		
		void Destroy();

		static Material* CreateDefaultTexturedMaterial();
		bool LoadFromJson(const rapidjson::Value& jsonMaterial);
		bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer);

		void AddColorAddative(float R, float G, float B) { m_ShaderCB.DiffuseAdditive.x += R; m_ShaderCB.DiffuseAdditive.y += G; m_ShaderCB.DiffuseAdditive.z += B;}
		void SetColorAddative(float R, float G, float B) { m_ShaderCB.DiffuseAdditive.x = R; m_ShaderCB.DiffuseAdditive.y = G; m_ShaderCB.DiffuseAdditive.z = B;}
		void SetUVTilingOffset(float U, float V) { m_ShaderCB.UVTiling.x = U; m_ShaderCB.UVTiling.y = V; }
		void AddUVTilingOffset(float U, float V) { m_ShaderCB.UVTiling.x += U; m_ShaderCB.UVTiling.y += V; }
		void SetUVOffset(float U, float V) { m_ShaderCB.UVOffset.x = U; m_ShaderCB.UVOffset.y = V; }
		void AddUVOffset(float U, float V) { m_ShaderCB.UVOffset.x += U; m_ShaderCB.UVOffset.y += V; }
		void SetMetallicOverride(float Override) { m_ShaderCB.MetallicAdditive = Override; }
		void AddMetallicOverride(float Override) { m_ShaderCB.MetallicAdditive += Override; }
		void SetRoughnessOverride(float Override) { m_ShaderCB.RoughnessAdditive = Override; }
		void AddRoughnessOverride(float Override) { m_ShaderCB.RoughnessAdditive += Override; }

		EMaterialType GetMaterialType() const { return m_MaterialType; }
		void SetMaterialType(EMaterialType MaterialType) { m_MaterialType = MaterialType; }

		CB_PS_VS_PerObjectMaterialAdditives GetMaterialOverrideConstantBuffer() { return m_ShaderCB; }

		void OnImGuiRender();
		
		void BindResources(bool IsDeferredPass);

	private:
		EMaterialType m_MaterialType = EMaterialType::MT_Invalid;

		StrongTexturePtr m_AlbedoMap;
		StrongTexturePtr m_NormalMap;
		StrongTexturePtr m_MetallicMap;
		StrongTexturePtr m_RoughnessMap;
		StrongTexturePtr m_AOMap;
		StrongTexturePtr m_OpacityMap;
		StrongTexturePtr m_TranslucencyMap;

		int m_AlbedoTextureManagerID;
		int m_NormalTextureManagerID;
		int m_MetallicTextureManagerID;
		int m_RoughnessTextureManagerID;
		int m_AoTextureManagerID;
		int m_OpacityTextureManagerID;
		int m_TranslucencyTextureManagerID;

		float m_RoughnessAdditive = 0.5f;
		float m_MetallicAdditive = 0.5f;
		FVector2 m_UVOffset;
		FVector2 m_Tiling;
		FVector3 m_ColorAdditive;

		CB_PS_VS_PerObjectMaterialAdditives m_ShaderCB;
	};

}