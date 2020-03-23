#include "RenderManager.h"
#include "..\Components\MeshRenderComponent.h"
#include "..\Objects\Camera.h"
#include <algorithm>

RenderManager::~RenderManager()
{
	/*std::list<Entity*>::iterator iter;
	for (iter = m_transparentObjects.begin(); iter != m_transparentObjects.end(); iter++)
	{
		(*iter)->
	}*/
}

void RenderManager::Flush()
{
	m_opaqueObjects.clear();
	m_opaqueInstanciatedObjects.clear();
	m_foliageObjects.clear();
	m_pGameCamera = nullptr;
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

void RenderManager::AddOpaqueInstantiatedObject(MeshRenderer * mr)
{
	m_opaqueInstanciatedObjects.push_back(mr);
}

void RenderManager::ClearInstanciatedOpaqueObjects()
{
	m_opaqueInstanciatedObjects.clear();
}

void RenderManager::DrawOpaque(const DirectX::XMMATRIX & projectionMatrix, const DirectX::XMMATRIX & viewMatrix)
{
	m_pGameCamera->Draw(projectionMatrix, viewMatrix);

	for (MeshRenderer* mr : m_opaqueObjects)
	{
		mr->Draw(projectionMatrix, viewMatrix);
	}

	for (MeshRenderer* mr : m_opaqueInstanciatedObjects)
	{
		mr->Draw(projectionMatrix, viewMatrix);
	}
}

void RenderManager::RemoveOpaqueInstance(MeshRenderer * mr)
{
	auto iter = std::find(m_opaqueInstanciatedObjects.begin(), m_opaqueInstanciatedObjects.end(), mr);
	if (iter != m_opaqueInstanciatedObjects.end())
	{
		//(*iter)->Destroy();
		//delete *iter;
	}
	iter = m_opaqueInstanciatedObjects.erase(iter);


}

void RenderManager::DrawFoliage(const DirectX::XMMATRIX & projectionMatrix, const DirectX::XMMATRIX & viewMatrix)
{
	for (MeshRenderer* mr : m_foliageObjects)
	{
		mr->Draw(projectionMatrix, viewMatrix);
	}
}
