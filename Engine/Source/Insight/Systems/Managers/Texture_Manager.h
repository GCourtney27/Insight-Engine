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
		Texture* GetTextureByID(Texture::ID textureID, Texture::eTextureType textreType);
	private:
		void LoadTextureByType(const Texture::IE_TEXTURE_INFO& texInfo);
	private:
		std::vector<Texture*> m_AlbedoTextures;
		std::vector<Texture*> m_NormalTextures;
		std::vector<Texture*> m_MetallicTextures;
		std::vector<Texture*> m_RoughnessTextures;
		std::vector<Texture*> m_AOTextures;
	};

}
