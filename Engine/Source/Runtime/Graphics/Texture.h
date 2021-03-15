#pragma once

#include <Runtime/Core.h>

namespace Insight {


	class INSIGHT_API Texture
	{
	public:
		typedef int ID;

		enum ETextureType
		{
			TT_Invalid = -1,
			// Per Object
			TT_Albedo = 0,
			TT_Normal = 1,
			TT_Roughness = 2,
			TT_Metallic = 3,
			TT_AmbientOcclusion = 4,
			TT_Opacity = 5,
			TT_Translucency = 6,
			// Sky Sphere
			TT_SkyIrradience = 7,
			TT_SkyRadianceMap = 8,
			TT_IBLBRDFLUT = 9,
			TT_SkyDiffuse = 10,
		};

		struct IE_TEXTURE_INFO
		{
			ETextureType Type = ETextureType::TT_Invalid;
			bool GenerateMipMaps = true;
			bool IsCubeMap = false;
			EString Filepath;
			ID Id;
		};

	public:
		Texture(IE_TEXTURE_INFO CreateInfo)
			: m_TextureInfo(CreateInfo) {}
		Texture() = delete;
		virtual ~Texture() = default;
	
		bool operator==(const Texture& Tex)
		{
			return m_TextureInfo.Id == Tex.GetTextureInfo().Id;
		}
		
		bool operator!=(const Texture& Tex)
		{
			return m_TextureInfo.Id != Tex.GetTextureInfo().Id;
		}

		// Destroy and release texture resources.
		virtual void Destroy() = 0;
		// Binds the texture to the pipeline to be drawn in the scene pass.
		virtual void BindForDeferredPass() = 0;
		virtual void BindForForwardPass() = 0;
		
		// Get the general information about this texture.
		inline const IE_TEXTURE_INFO& GetTextureInfo() const { return m_TextureInfo; }
		// Get the full file path to this texture on disk.
		inline const std::wstring& GetFilepath() const { return m_TextureInfo.Filepath; }
		// Returns true if the texture is the default for its type. False if not.
		inline bool IsDefaultTexture() const { return m_TextureInfo.Id < 0; }

	protected:
		IE_TEXTURE_INFO	m_TextureInfo = {};
	};

	using IE_TEXTURE_INFO = Texture::IE_TEXTURE_INFO;
}