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

		bool LoadResourcesFromJson(const rapidjson::Value& jsonTextures);
		StrongTexturePtr GetTextureByID(Texture::ID textureID, Texture::eTextureType textreType);
	private:
		void LoadTextureByType(const Texture::IE_TEXTURE_INFO& texInfo);
	private:
		std::vector<StrongTexturePtr> m_AlbedoTextures;
		std::vector<StrongTexturePtr> m_NormalTextures;
		std::vector<StrongTexturePtr> m_MetallicTextures;
		std::vector<StrongTexturePtr> m_RoughnessTextures;
		std::vector<StrongTexturePtr> m_AOTextures;
	};

}
