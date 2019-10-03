#pragma once
#include "Component.h"
#include "..\\Graphics\Model.h"

class MeshRenderer : public Component
{
public:
	MeshRenderer(Entity* owner) : Component(owner) {}

	bool Initialize(const std::string & filepath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader); //float boundingSphere scale
	void Draw(const XMMATRIX & viewProjectionMatrix);
	
	void Update() override;
	void Destroy() override;

	void SetVisible(bool visible = true) { m_visible = visible; }
	bool GetVisible() { return m_visible; }

	void SetWorldMat(const XMMATRIX& matrix) { worldMatrix = matrix; }

protected:
	bool m_visible = true;

	std::string filepath;
	Model model;
	XMMATRIX worldMatrix = XMMatrixIdentity();
};