#include "RenderManager.h"
#include "..\Components\MeshRenderComponent.h"
#include "..\Objects\Camera.h"

RenderManager::~RenderManager()
{
	/*std::list<Entity*>::iterator iter;
	for (iter = m_transparentObjects.begin(); iter != m_transparentObjects.end(); iter++)
	{
		(*iter)->
	}*/
}

void RenderManager::Draw(const DirectX::XMMATRIX & projectionMatrix, const DirectX::XMMATRIX & viewMatrix)
{

	//TODO: DrawSky();
	DrawOpaque(projectionMatrix, viewMatrix);
	DrawFoliage(projectionMatrix, viewMatrix);
}

void RenderManager::AddFoliageObject(MeshRenderer * mr)
{
	m_foliageObjects.push_back(mr);
}

void RenderManager::AddOpaqueObject(MeshRenderer * mr)
{
	m_opaqueObjects.push_back(mr);
}

void RenderManager::DrawOpaque(const DirectX::XMMATRIX & projectionMatrix, const DirectX::XMMATRIX & viewMatrix)
{
	m_pGameCamera->Draw(projectionMatrix, viewMatrix);

	for (MeshRenderer* mr : m_opaqueObjects)
	{
		mr->Draw(projectionMatrix, viewMatrix);
	}
}

void RenderManager::DrawFoliage(const DirectX::XMMATRIX & projectionMatrix, const DirectX::XMMATRIX & viewMatrix)
{
	for (MeshRenderer* mr : m_foliageObjects)
	{
		mr->Draw(projectionMatrix, viewMatrix);
	}
}
