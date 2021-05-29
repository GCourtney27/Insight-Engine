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


	class INSIGHT_API ieStaticMeshComponent : public ieComponentBase/*<ieStaticMeshComponent>*/
	{
	public:
		ieStaticMeshComponent(ieActor* pOwner)
			: ieComponentBase()
			, m_pMeshWorldCB(NULL)
			, m_Transform()
			, m_bIsDrawEnabled(true)
		{
			Graphics::g_pConstantBufferManager->CreateConstantBuffer(L"[Static Mesh Component] World CB", &m_pMeshWorldCB, sizeof(MeshWorld));
		}
		virtual ~ieStaticMeshComponent()
		{
		}
		ieStaticMeshComponent(ieStaticMeshComponent&& Other)
		{
			m_GeometryRef = std::move(Other.m_GeometryRef);
			m_Transform = std::move(Other.m_Transform);
			m_pMeshWorldCB = std::move(Other.m_pMeshWorldCB);
			m_MaterialRef = Other.m_MaterialRef;
			m_bIsDrawEnabled = std::move(Other.m_bIsDrawEnabled);
		}
		ieStaticMeshComponent& operator=(const ieStaticMeshComponent& Other) = default;


		virtual void Render(Graphics::ICommandContext& GfxContext) override
		{
			if (!GetIsDrawEnabled()) return;

			if (m_MaterialRef.IsValid())
			{
				// Set the material information.
				m_MaterialRef->Bind(GfxContext);
			}

			if (m_GeometryRef.IsValid())
			{
				// Set the world buffer.
				MeshWorld* pWorld = m_pMeshWorldCB->GetBufferPointer<MeshWorld>();
				pWorld->WorldMat = m_Transform.GetWorldMatrix().Transpose();
				GfxContext.SetGraphicsConstantBuffer(kMeshWorld, m_pMeshWorldCB);

				// TODO Request draw from model in model manager.
				// Render the geometry.
				GfxContext.SetPrimitiveTopologyType(Graphics::PT_TiangleList);
				GfxContext.BindVertexBuffer(0, m_GeometryRef->GetVertexBuffer());
				GfxContext.BindIndexBuffer(m_GeometryRef->GetIndexBuffer());
				GfxContext.DrawIndexedInstanced(m_GeometryRef->GetNumIndices(), 1, 0, 0, 0);
			}
		}

		inline void SetMesh(StaticMeshGeometryRef Mesh) { m_GeometryRef = Mesh; }
		inline void SetMaterial(MaterialRef Material) { m_MaterialRef = Material; }

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
