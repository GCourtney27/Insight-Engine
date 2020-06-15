#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Texture.h"
#include "Platform/DirectX_Shared/Constant_Buffer_Types.h"
namespace Insight {

	class INSIGHT_API Material
	{
	public:
		Material();
		//Material(Material&& material) noexcept;
		~Material();
		
		virtual bool LoadFromJson(const rapidjson::Value& jsonMaterial);
		CB_PS_VS_PerObjectAdditives GetMaterialOverrideConstantBuffer() { return m_ShaderCB; }

		void OnImGuiRender();
		
		void BindResources();

	private:

		StrongTexturePtr m_AlbedoMap;
		StrongTexturePtr m_NormalMap;
		StrongTexturePtr m_MetallicMap;
		StrongTexturePtr m_RoughnessMap;
		StrongTexturePtr m_AOMap;

		float m_RoughnessAdditive = 0.5f;
		float m_MetallicAdditive = 0.5f;
		Vector2 m_UVOffset;
		Vector2 m_Tiling;
		Vector3 m_ColorAdditive;

		CB_PS_VS_PerObjectAdditives m_ShaderCB;
	};

}