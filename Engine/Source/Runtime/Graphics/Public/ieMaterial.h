#pragma once

#include "Runtime/Core.h"

#include "Runtime/Graphics/Public/ICommandContext.h"
#include "Runtime/Graphics/Public/WorldRenderer/Common.h"
#include "Runtime/Graphics/Public/ResourceManagement/ITextureManager.h"
#include "Runtime/Graphics/Public/ResourceManagement/IConstantBufferManager.h"


namespace Insight
{
	typedef UInt64 MaterialID;


	class INSIGHT_API ieMaterial
	{
		friend class MaterialManager;
		ALIGN(16) struct MaterialConstants
		{
			FVector4 Color;
		};
	public:
		ieMaterial()
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
			// Set constants.
			MaterialConstants* pMat = m_pConstantsCB->GetBufferPointer<MaterialConstants>();
			pMat->Color = m_Constants.Color;
			GfxContext.SetGraphicsConstantBuffer(SPI_MaterialParams, m_pConstantsCB);

			// Set Textures
			GfxContext.SetTexture(SPI_Texture_Albedo, m_AlbedoTexture);
			GfxContext.SetTexture(SPI_Texture_Normal, m_NormalTexture);
		}

		MaterialID GetUID() const
		{
			return m_UID;
		}

	protected:
		void Initialize()
		{
			// Init constant buffers
			Graphics::g_pConstantBufferManager->CreateConstantBuffer(TEXT("Material Params"), &m_pConstantsCB, sizeof(MaterialConstants));

			// Load Textures
			// 
			// TODO: Only dds textures can be loaded right now.
			const TChar* AlbedoTexturePath = L"Content/Textures/RustedIron/RustedIron_Albedo.dds";
			m_AlbedoTexture = Graphics::g_pTextureManager->LoadTexture(AlbedoTexturePath, Graphics::DT_Magenta2D, false);
			const TChar* NormalTexturePath = L"Content/Textures/RustedIron/RustedIron_Normal.dds";
			m_NormalTexture = Graphics::g_pTextureManager->LoadTexture(NormalTexturePath, Graphics::DT_Magenta2D, false);
		}

		void UnInitialize()
		{
			Graphics::g_pConstantBufferManager->DestroyConstantBuffer(m_pConstantsCB->GetUID());
		}

		void SetUID(const MaterialID& NewId)
		{
			m_UID = NewId;
		}

	private:
		Graphics::IConstantBuffer* m_pConstantsCB;
		MaterialConstants m_Constants;
		Graphics::ITextureRef m_AlbedoTexture;
		Graphics::ITextureRef m_NormalTexture;

		MaterialID m_UID;
	};
}
