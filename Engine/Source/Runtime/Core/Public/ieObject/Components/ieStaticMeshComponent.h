// Copyright Insight Interactive. All Rights Reserved.
#pragma once

#include <Runtime/Core.h>

#include "Runtime/Core/Public/ieObject/Components/ieComponentBase.h"

#include "Runtime/Math/Public/Transform.h"

#include "Runtime/Graphics/Public/WorldRenderer/Common.h"

#include "Runtime/Core/Public/ECS/Component/ComponentFwd.h"

namespace Insight
{
	class ieObjectBase;

	struct INSIGHT_API ieMaterial
	{

		ALIGN(16) struct MaterialConstants
		{
			FVector4 Color;
		};
		ieMaterial()
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
		~ieMaterial()
		{
			Graphics::g_pConstantBufferManager->DestroyConstantBuffer(m_pConstantsCB->GetUID());
		}

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

	private:
		Graphics::IConstantBuffer* m_pConstantsCB;
		MaterialConstants m_Constants;
		Graphics::ITextureRef m_AlbedoTexture;
		Graphics::ITextureRef m_NormalTexture;

		//static UInt32 s_MaterialIDs;
	};

	class INSIGHT_API ieStaticMeshComponent : public ieComponentBase<ieStaticMeshComponent>
	{
	public:
		ieStaticMeshComponent()
			: ieComponentBase()
			, m_pMeshWorldCB(NULL)
		{
			Init();
		}
		virtual ~ieStaticMeshComponent()
		{
			UnInit();
		}

		void Load(const FString& Path)
		{
			Vertex3D Verts[] = {
				// Front
				{ { -0.5f,  0.5f, -0.5f }, { 1.f, 0.f, 0.f, 1.f }, { 0.0f, 0.0f} },
				{ {  0.5f, -0.5f, -0.5f }, { 0.f, 1.f, 0.f, 1.f }, { 0.0f, 1.0f} },
				{ { -0.5f, -0.5f, -0.5f }, { 0.f, 0.f, 1.f, 1.f }, { 0.0f, 1.0f} },
				{ {  0.5f,  0.5f, -0.5f }, { 1.f, 0.f, 0.f, 1.f }, { 1.0f, 0.0f} },
				// Right 
				{ {  0.5f, -0.5f, -0.5f }, { 0.f, 1.f, 0.f, 1.f }, { 0.0f, 0.0f} },
				{ {  0.5f,  0.5f,  0.5f }, { 0.f, 0.f, 1.f, 1.f }, { 0.0f, 1.0f} },
				{ {  0.5f, -0.5f,  0.5f }, { 1.f, 0.f, 0.f, 1.f }, { 0.0f, 1.0f} },
				{ {  0.5f,  0.5f, -0.5f }, { 0.f, 1.f, 0.f, 1.f }, { 1.0f, 0.0f} },
				// Left
				{ { -0.5f,  0.5f,  0.5f }, { 0.f, 0.f, 1.f, 1.f }, { 0.0f, 0.0f } },
				{ { -0.5f, -0.5f, -0.5f }, { 1.f, 0.f, 0.f, 1.f }, { 0.0f, 1.0f } },
				{ { -0.5f, -0.5f,  0.5f }, { 0.f, 1.f, 0.f, 1.f }, { 0.0f, 1.0f } },
				{ { -0.5f,  0.5f, -0.5f }, { 0.f, 0.f, 1.f, 1.f }, { 1.0f, 0.0f } },
				// Back
				{ {  0.5f,  0.5f,  0.5f }, { 1.f, 0.f, 0.f, 1.f }, { 0.0f, 0.0f} },
				{ { -0.5f, -0.5f,  0.5f }, { 0.f, 1.f, 0.f, 1.f }, { 0.0f, 1.0f} },
				{ {  0.5f, -0.5f,  0.5f }, { 0.f, 0.f, 1.f, 1.f }, { 0.0f, 1.0f} },
				{ { -0.5f,  0.5f,  0.5f }, { 1.f, 0.f, 0.f, 1.f }, { 1.0f, 0.0f} },
				// Top
				{ { -0.5f,  0.5f, -0.5f }, { 0.f, 1.f, 0.f, 1.f }, { 0.0f, 0.0f} },
				{ {  0.5f,  0.5f,  0.5f }, { 0.f, 0.f, 1.f, 1.f }, { 0.0f, 1.0f} },
				{ {  0.5f,  0.5f, -0.5f }, { 1.f, 0.f, 0.f, 1.f }, { 0.0f, 1.0f} },
				{ { -0.5f,  0.5f,  0.5f }, { 0.f, 1.f, 0.f, 1.f }, { 1.0f, 0.0f} },
				// Bottom
				{ {  0.5f, -0.5f,  0.5f }, { 0.f, 0.f, 1.f, 1.f }, { 0.0f, 0.0f} },
				{ { -0.5f, -0.5f, -0.5f }, { 1.f, 0.f, 0.f, 1.f }, { 0.0f, 1.0f} },
				{ {  0.5f, -0.5f, -0.5f }, { 0.f, 1.f, 0.f, 1.f }, { 0.0f, 1.0f} },
				{ { -0.5f, -0.5f,  0.5f }, { 0.f, 0.f, 1.f, 1.f }, { 1.0f, 0.0f} },
			};

			const UInt32 VertexBufferSize = sizeof(Verts);
			m_DrawArgs.NumVerts = VertexBufferSize / sizeof(Vertex3D);

			// Init Vertex buffer.
			IE_ASSERT(m_DrawArgs.VertexBufferHandle != IE_INVALID_VERTEX_BUFFER_HANDLE); // Vertex buffer was not registered properly with geometry buffer manager.
			Graphics::IVertexBuffer& Buffer = Graphics::g_pGeometryManager->GetVertexBufferByUID(m_DrawArgs.VertexBufferHandle);
			Buffer.Create(TEXT("Vertex Buffer"), VertexBufferSize, sizeof(Vertex3D), Verts);

			UInt32 Indices[] =
			{
				// front face
				0, 1, 2, // first triangle
				0, 3, 1, // second triangle

				// left face
				4, 5, 6, // first triangle
				4, 7, 5, // second triangle

				// right face
				8, 9, 10, // first triangle
				8, 11, 9, // second triangle

				// back face
				12, 13, 14, // first triangle
				12, 15, 13, // second triangle

				// top face
				16, 17, 18, // first triangle
				16, 19, 17, // second triangle

				// bottom face
				20, 21, 22, // first triangle
				20, 23, 21, // second triangle
			};
			UInt32 IndexBufferSize = sizeof(Indices);
			m_DrawArgs.NumIndices = IndexBufferSize / sizeof(UInt32);

			// Init Index buffer
			IE_ASSERT(m_DrawArgs.IndexBufferHandle != IE_INVALID_INDEX_BUFFER_HANDLE); // Index buffer was not registered properly with geometry buffer manager.
			Graphics::IIndexBuffer& IndexBuffer = Graphics::g_pGeometryManager->GetIndexBufferByUID(m_DrawArgs.IndexBufferHandle);
			IndexBuffer.Create(TEXT("Index Buffer"), IndexBufferSize, Indices);
		}

		void Draw(Graphics::ICommandContext& GfxContext)
		{
			Graphics::IVertexBuffer& VertBuffer = Graphics::g_pGeometryManager->GetVertexBufferByUID(m_DrawArgs.VertexBufferHandle);
			Graphics::IIndexBuffer& IndexBuffer = Graphics::g_pGeometryManager->GetIndexBufferByUID(m_DrawArgs.IndexBufferHandle);

			m_Material.Bind(GfxContext);

			// Set the world buffer.
			MeshWorld* pWorld = m_pMeshWorldCB->GetBufferPointer<MeshWorld>();
			pWorld->WorldMat = m_Transform.GetWorldMatrix().Transpose();
			GfxContext.SetGraphicsConstantBuffer(SPI_MeshWorld, m_pMeshWorldCB);


			// Render the geometry.
			GfxContext.SetPrimitiveTopologyType(Graphics::PT_TiangleList);
			GfxContext.BindVertexBuffer(0, VertBuffer);
			GfxContext.BindIndexBuffer(IndexBuffer);
			GfxContext.DrawIndexedInstanced(m_DrawArgs.NumIndices, 1, 0, 0, 0);
		}

		inline ieMaterial& GetMaterial() { return m_Material; }
		inline ieTransform& GetTransform() { return m_Transform; }

	protected:

		ALIGN(16) struct MeshWorld
		{
			FMatrix WorldMat;
		};

		struct Vertex3D
		{
			FVector3 Position;
			FVector4 Color;
			FVector2 UV0;
		};

		struct DrawArgs
		{
			UInt32 NumVerts;
			UInt32 NumIndices;
			Graphics::VertexBufferUID VertexBufferHandle;
			Graphics::IndexBufferUID IndexBufferHandle;
		};

		void Init()
		{
			m_DrawArgs.VertexBufferHandle = Graphics::g_pGeometryManager->AllocateVertexBuffer();
			m_DrawArgs.IndexBufferHandle = Graphics::g_pGeometryManager->AllocateIndexBuffer();

			Graphics::g_pConstantBufferManager->CreateConstantBuffer(TEXT("Mesh World Params"), &m_pMeshWorldCB, sizeof(MeshWorld));

		}

		void UnInit()
		{
			Graphics::g_pGeometryManager->DeAllocateVertexBuffer(m_DrawArgs.VertexBufferHandle);
			Graphics::g_pGeometryManager->DeAllocateIndexBuffer(m_DrawArgs.IndexBufferHandle);

			Graphics::g_pConstantBufferManager->DestroyConstantBuffer(m_pMeshWorldCB->GetUID());
		}

		Graphics::IConstantBuffer* m_pMeshWorldCB;
		ieMaterial m_Material;
		ieTransform m_Transform;
		DrawArgs m_DrawArgs;
	};
}
