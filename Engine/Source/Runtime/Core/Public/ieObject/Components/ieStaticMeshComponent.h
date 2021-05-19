// Copyright Insight Interactive. All Rights Reserved.
#pragma once

#include <Runtime/Core.h>

#include "Runtime/Core/Public/ieObject/Components/ieComponentBase.h"

#include "Runtime/Math/Public/Transform.h"
#include "Runtime/Graphics/Public/WorldRenderer/Common.h"
#include "Runtime/Core/Public/ECS/Component/ComponentFwd.h"
#include "Runtime/Graphics/Public/GraphicsCore.h"
#include "Runtime/Graphics/Public/ResourceManagement/ModelManager.h"
#include "Runtime/Graphics/Public/ResourceManagement/MaterialManager.h"

namespace Insight
{
	class ieObjectBase;


	class INSIGHT_API ieStaticMeshComponent : public ieComponentBase<ieStaticMeshComponent>
	{
	public:
		ieStaticMeshComponent()
			: ieComponentBase()
			, m_pMeshWorldCB(NULL)
			, m_Transform()
			, m_bIsDrawEnabled(true)
		{
			FString Name = L"SM: " + ToString(GetId());
			SetName(Name);

			Graphics::g_pConstantBufferManager->CreateConstantBuffer(L"World CB", &m_pMeshWorldCB, sizeof(MeshWorld));

			Graphics::g_StaticGeometryManager.LoadFBXFromFile(L"Content/Models/Cube.fbx");
			m_GeometryRef = Graphics::g_StaticGeometryManager.GetStaticMeshByName(L"Cube");

			Graphics::g_MaterialManager.LoadMaterialFromFile(L"Content/Materials/M_RustedMetal.ieMat");
			m_MaterialRef = Graphics::g_MaterialManager.GetMaterialByName(L"M_RustedMetal.ieMat");
		}
		virtual ~ieStaticMeshComponent()
		{
		}
		ieStaticMeshComponent(ieStaticMeshComponent&& Other)
		{
			m_GeometryRef		= std::move(Other.m_GeometryRef);
			m_Transform			= std::move(Other.m_Transform);
			m_pMeshWorldCB		= std::move(Other.m_pMeshWorldCB);
			m_MaterialRef		= Other.m_MaterialRef;
			m_bIsDrawEnabled	= std::move(Other.m_bIsDrawEnabled);
		}
		ieStaticMeshComponent& operator=(const ieStaticMeshComponent& Other) = default;


		void Draw(Graphics::ICommandContext& GfxContext)
		{
			if (!GetIsDrawEnabled()) return;

			// Set the material information.
			m_MaterialRef->Bind(GfxContext);

			// Set the world buffer.
			MeshWorld* pWorld = m_pMeshWorldCB->GetBufferPointer<MeshWorld>();
			pWorld->WorldMat = m_Transform.GetWorldMatrix().Transpose();
			GfxContext.SetGraphicsConstantBuffer(SPI_MeshWorld, m_pMeshWorldCB);

			// TODO Request draw from model in model manager.
			// Render the geometry.
			GfxContext.SetPrimitiveTopologyType(Graphics::PT_TiangleList);
			GfxContext.BindVertexBuffer(0, m_GeometryRef->GetVertexBuffer());
			GfxContext.BindIndexBuffer(m_GeometryRef->GetIndexBuffer());
			GfxContext.DrawIndexedInstanced(m_GeometryRef->GetNumIndices(), 1, 0, 0, 0);
		}

		inline MaterialRef& GetMaterial() { return m_MaterialRef; }
		inline ieTransform& GetTransform() { return m_Transform; }
		inline bool GetIsDrawEnabled() const;
		inline void SetIsDrawEnabled(bool bVisible);

	protected:
		StaticMeshGeometryRef m_GeometryRef;
		Graphics::IConstantBuffer* m_pMeshWorldCB;
		MaterialRef m_MaterialRef;
		ieTransform m_Transform;
		bool m_bIsDrawEnabled;

	};


	// 
	// Inline funciton implementations
	//

	inline bool ieStaticMeshComponent::GetIsDrawEnabled() const
	{
		return m_bIsDrawEnabled;
	}

	inline void ieStaticMeshComponent::SetIsDrawEnabled(bool bEnabled)
	{
		m_bIsDrawEnabled = bEnabled;
	}
}
