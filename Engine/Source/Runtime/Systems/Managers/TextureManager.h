#pragma once

#include <Runtime/CoreMacros.h>

#include "Runtime/Rendering/Texture.h"

#define DEFAULT_ALBEDO_TEXTURE_ID -1
#define DEFAULT_NORMAL_TEXTURE_ID -2
#define DEFAULT_METALLIC_TEXTURE_ID -3
#define DEFAULT_ROUGHNESS_TEXTURE_ID -4
#define DEFAULT_AO_TEXTURE_ID -5

namespace Insight {

	class INSIGHT_API TextureManager
	{
	public:
		TextureManager();
		~TextureManager();
		
		bool Init();
		bool PostInit();
		void Destroy();

		// Destroy all textures owned by the manager.
		void FlushTextureCache();
		// Load the textures in to the texture cache from a scene's resource file.
		bool LoadResourcesFromJson(const rapidjson::Value& jsonTextures);
		// Returns a reference to an existing texture by id that is owned by the manager. 
		// Returns the default texture for the given texture type if it does not exist.
		StrongTexturePtr GetTextureByID(Texture::ID TextureID, Texture::eTextureType TextreType);
		// Queue a texture to wait for its file asset to be loaded. 
		void RegisterTextureLoadCallback(Texture::ID AwaitingTextureId, StrongTexturePtr* AwaitingTexture);

		// Return the default albedo texture.
		StrongTexturePtr GetDefaultAlbedoTexture() { return m_DefaultAlbedoTexture; }
		// Return the default normal texture.
		StrongTexturePtr GetDefaultNormalTexture() { return m_DefaultNormalTexture; }
		// Return the default metallic texture.
		StrongTexturePtr GetDefaultMetallicTexture() { return m_DefaultMetallicTexture; }
		// Return the default roughness texture.
		StrongTexturePtr GetDefaultRoughnessTexture() { return m_DefaultRoughnessTexture; }
		// Return the default ambient occlusion texture.
		StrongTexturePtr GetDefaultAOTexture() { return m_DefaultAOTexture; }

	private:
		// Load the default textures that will be used as fallbacks for invalid or placeholders textures.
		bool LoadDefaultTextures();
		// Create and register a texture inside the texture manager to be reused by other materials.
		void RegisterTextureByType(const IE_TEXTURE_INFO TexInfo);

	private:
		Texture::ID m_HighestTextureId;

		std::map<Texture::ID, StrongTexturePtr> m_AlbedoTextureMap;
		std::map<Texture::ID, StrongTexturePtr> m_NormalTextureMap;
		std::map<Texture::ID, StrongTexturePtr> m_MetallicTextureMap;
		std::map<Texture::ID, StrongTexturePtr> m_RoughnessTextureMap;
		std::map<Texture::ID, StrongTexturePtr> m_AOTextureMap;
		std::map<Texture::ID, StrongTexturePtr> m_OpacityTextureMap;
		std::map<Texture::ID, StrongTexturePtr> m_TranslucencyTextureMap;

		StrongTexturePtr m_DefaultAlbedoTexture;
		StrongTexturePtr m_DefaultNormalTexture;
		StrongTexturePtr m_DefaultMetallicTexture;
		StrongTexturePtr m_DefaultRoughnessTexture;
		StrongTexturePtr m_DefaultAOTexture;
		
		std::vector<std::future<void>> m_TextureLoadFutures;

		std::map<Texture::ID, std::list<StrongTexturePtr*>> m_AwaitingLoadTextures;
	};

}
