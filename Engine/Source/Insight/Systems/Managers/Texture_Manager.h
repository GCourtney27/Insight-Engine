#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Texture.h"


namespace Insight {

	class INSIGHT_API TextureManager
	{
	public:
		TextureManager();
		~TextureManager();
		
		bool Init();
		void Destroy();

		void FlushTextureCache();
		bool LoadResourcesFromJson(const rapidjson::Value& jsonTextures);
		StrongTexturePtr GetTextureByID(Texture::ID textureID, Texture::eTextureType textreType);
		
		StrongTexturePtr GetDefaultAlbedoTexture();
		StrongTexturePtr GetDefaultNormalTexture();
		StrongTexturePtr GetDefaultMetallicTexture();
		StrongTexturePtr GetDefaultRoughnessTexture();
		StrongTexturePtr GetDefaultAOTexture();

	private:
		bool LoadDefaultTextures();
		void RegisterTextureByType(const Texture::IE_TEXTURE_INFO& texInfo);

	private:
		Texture::ID m_HighestTextureId = 0;
		std::vector<StrongTexturePtr> m_AlbedoTextures;
		std::vector<StrongTexturePtr> m_NormalTextures;
		std::vector<StrongTexturePtr> m_MetallicTextures;
		std::vector<StrongTexturePtr> m_RoughnessTextures;
		std::vector<StrongTexturePtr> m_AOTextures;
		std::vector<StrongTexturePtr> m_OpacityTextures;
		std::vector<StrongTexturePtr> m_TranslucencyTextures;

		StrongTexturePtr m_DefaultAlbedoTexture;
		StrongTexturePtr m_DefaultNormalTexture;
		StrongTexturePtr m_DefaultMetallicTexture;
		StrongTexturePtr m_DefaultRoughnessTexture;
		StrongTexturePtr m_DefaultAOTexture;
	};

}
