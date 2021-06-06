#pragma once

#include "Runtime/Core.h"

#include "Runtime/Graphics/Public/ICommandContext.h"
#include "Runtime/Graphics/Public/WorldRenderer/RendererCommon.h"
#include "Runtime/Graphics/Public/ResourceManagement/ITextureManager.h"
#include "Runtime/Graphics/Public/ResourceManagement/IConstantBufferManager.h"


namespace Insight
{
	typedef UInt64 MaterialID;


	class INSIGHT_API ieMaterial
	{
		friend class MaterialManager;

		IE_ALIGN(16) struct MaterialConstants
		{
			FVector4 Color;
		};
		enum EMaterialType
		{
			MT_Opaque,
			MT_Translucent,
		};

	public:
		ieMaterial()
			: m_UID(IE_INVALID_MATERIAL_ID)
			, m_Type(MT_Opaque)
		{
		}
		~ieMaterial()
		{
		}
		ieMaterial(ieMaterial&& Other) noexcept
		{
			m_pConstantsCB = std::move(Other.m_pConstantsCB);
			m_Constants = std::move(Other.m_Constants);
			m_AlbedoTexture = std::move(Other.m_AlbedoTexture);
			m_NormalTexture = std::move(Other.m_NormalTexture);
		}
		ieMaterial& operator=(const ieMaterial& Other) = default;

		void Bind(Graphics::ICommandContext& GfxContext)
		{
			switch (m_Type)
			{
			case MT_Opaque:
				BindForOpaquePass(GfxContext);
				break;
			case MT_Translucent:
				BindForTranslucentPass(GfxContext);
				break;
			default:
				IE_ASSERT(false, "Invalid material type provided when binding for draw.");
				break;
			}
		}

		void CreateFromMemory(DataBlob Memory);
		void WriteToFile();
		

		FVector4 GetColor() const
		{
			return m_Constants.Color;
		}

		Graphics::ITextureRef GetAlbedoTexture() const
		{
			return m_AlbedoTexture;
		}

		Graphics::ITextureRef GetNormalTexture() const
		{
			return m_NormalTexture;
		}

		EMaterialType GetType() const
		{
			return m_Type;
		}

		void SetColor(FVector4& Color)
		{
			m_Constants.Color = Color;
		}

		void SetAlbedoTexture(Graphics::ITextureRef TextureRef)
		{
			m_AlbedoTexture = TextureRef;
		}

		void SetNormalTexture(Graphics::ITextureRef TextureRef)
		{
			m_NormalTexture = TextureRef;
		}

		void SetType(EMaterialType Type)
		{
			m_Type = Type;
		}

		MaterialID GetUID() const
		{
			return m_UID;
		}

		bool IsValid()
		{
			return m_UID != IE_INVALID_MATERIAL_ID;
		}

	protected:
		void BindForOpaquePass(Graphics::ICommandContext& GfxContext)
		{
			// Set constants.
			MaterialConstants* pMat = m_pConstantsCB->GetBufferPointer<MaterialConstants>();
			pMat->Color = m_Constants.Color;
			GfxContext.SetGraphicsConstantBuffer(kMaterial, m_pConstantsCB);

			// Set Textures.
			{
				GfxContext.SetTexture(GRP_MaterialTextureAlbedo, m_AlbedoTexture);
				GfxContext.SetTexture(GRP_MaterialTextureNormal, m_NormalTexture);
			}
		}

		void BindForTranslucentPass(Graphics::ICommandContext& GfxContext)
		{
			// TODO Forward transparent pass
		}

		void Initialize()
		{
			// Init constant buffers
			Graphics::g_pConstantBufferManager->CreateConstantBuffer(TEXT("Material Params"), &m_pConstantsCB, sizeof(MaterialConstants));
		}

		void UnInitialize()
		{
			Graphics::g_pConstantBufferManager->DestroyConstantBuffer(m_pConstantsCB->GetUID());
		}

		void SetUID(const MaterialID& NewId)
		{
			m_UID = NewId;
		}

		void SetDebugName(const FString& Name)
		{
#if IE_DEBUG
			m_DebugName = Name;
#endif
		}

	private:
		Graphics::IConstantBuffer* m_pConstantsCB;
		MaterialConstants m_Constants;
		Graphics::ITextureRef m_AlbedoTexture;
		Graphics::ITextureRef m_NormalTexture;
#if IE_DEBUG
		FString m_DebugName;
#endif

		MaterialID m_UID;
		EMaterialType m_Type;
	};
}
