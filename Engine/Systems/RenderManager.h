#pragma once
#include <vector>
#include <DirectXMath.h>

class MeshRenderer;
class Camera;

class RenderManager
{
public:
	RenderManager() {}
	~RenderManager();

	void Draw(const DirectX::XMMATRIX & projectionMatrix, const DirectX::XMMATRIX & viewMatrix);

	void AddFoliageObject(MeshRenderer* mr);
	void AddOpaqueObject(MeshRenderer* mr);
		

	void DrawFoliage(const DirectX::XMMATRIX & projectionMatrix, const DirectX::XMMATRIX & viewMatrix);
	void DrawOpaque(const DirectX::XMMATRIX & projectionMatrix, const DirectX::XMMATRIX & viewMatrix);

	void SetGameCamera(Camera* cam) { m_pGameCamera = cam; }
private:

	Camera* m_pGameCamera = nullptr;

	std::vector<MeshRenderer*> m_opaqueObjects;
	std::vector<MeshRenderer*> m_foliageObjects;
	//Entity* m_sky;
};