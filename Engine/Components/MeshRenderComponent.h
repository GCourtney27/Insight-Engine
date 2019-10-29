#pragma once
#include "Component.h"
#include "..\\Graphics\Model.h"

class MeshRenderer : public Component
{
public:
	MeshRenderer(Entity* owner)
		: Component(owner) {}

	bool Initialize(Entity* owner, const std::string & filepath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader, Material * material);
	void Draw(const XMMATRIX & viewProjectionMatrix, const XMMATRIX & viewMatrix);
	
	
	void Update(const float& deltaTime) override;
	void Destroy() override;
	void OnImGuiRender() override;

	void InitFromJSON(Entity* owner, const rapidjson::Value& componentInformation) override;

	void SetIsDrawEnabled(bool visible = true) { m_drawEnabled = visible; }
	bool GetIsDrawEnabled() { return m_drawEnabled; }

	void SetisDebugMesh(bool visible) { m_isDebugMesh = visible; }
	bool GetIsDebugMesh() { return m_isDebugMesh; }

	void SetWorldMat(const XMMATRIX& matrix) { worldMatrix = matrix; }

	Model* GetModel() { return &model; }

protected:
	bool m_drawEnabled = true;
	bool m_isDebugMesh = false;
	std::string filepath;

	Model model;

	XMMATRIX worldMatrix = XMMatrixIdentity();
};