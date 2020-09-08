#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Texture.h"
#include "Renderer/Platform/Windows/DirectX_Shared/Constant_Buffer_Types.h"

namespace Insight {

	class INSIGHT_API Material
	{
	public:
		enum eMaterialType
		{
			eMaterialType_Invalid = -1,
			eMaterialType_Opaque = 0,
			eMaterialType_Translucent = 1,
		};
	public:
		Material();
		Material(Material&& material) noexcept;
		~Material();
		
		void Destroy();

		static Material* CreateDefaultTexturedMaterial();
		bool LoadFromJson(const rapidjson::Value& jsonMaterial);
		bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer);

		eMaterialType GetMaterialType() const { return m_MaterialType; }
		void SetMaterialType(eMaterialType MaterialType) { m_MaterialType = MaterialType; }

		CB_PS_VS_PerObjectAdditives GetMaterialOverrideConstantBuffer() { return m_ShaderCB; }

		void OnImGuiRender();
		
		void BindResources(bool IsDeferredPass);

	private:
		eMaterialType m_MaterialType = eMaterialType::eMaterialType_Invalid;

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
		Math::ieVector2 m_UVOffset;
		Math::ieVector2 m_Tiling;
		Math::ieVector3 m_ColorAdditive;

		CB_PS_VS_PerObjectAdditives m_ShaderCB;
	};

}