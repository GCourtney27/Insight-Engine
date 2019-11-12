#pragma once
#include <vector>
#include <DirectXMath.h>

class MeshRenderer;

class RenderManager
{
public:
	RenderManager() {}
	~RenderManager();

	void Draw(const DirectX::XMMATRIX & projectionMatrix, const DirectX::XMMATRIX & viewMatrix);

	void AddFoliageObject(MeshRenderer* mr);
	void AddOpaqueObject(MeshRenderer* mr);
		

private:
	void DrawFoliage(const DirectX::XMMATRIX & projectionMatrix, const DirectX::XMMATRIX & viewMatrix);
	void DrawOpaque(const DirectX::XMMATRIX & projectionMatrix, const DirectX::XMMATRIX & viewMatrix);

	std::vector<MeshRenderer*> m_opaqueObjects;
	std::vector<MeshRenderer*> m_foliageObjects;
	//Entity* m_sky;
};