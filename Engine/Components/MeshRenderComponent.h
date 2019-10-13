#pragma once
#include "Component.h"
#include "..\\Graphics\Model.h"
#include "..\Graphics\Material.h"

class MeshRenderer : public Component
{
public:
	MeshRenderer(Entity* owner)
		: Component(owner) {}

	bool Initialize(Entity* owner, const std::string & filepath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader);
	//bool Initialize(const std::string & filepath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader, Material* material);
	void Draw(const XMMATRIX & viewProjectionMatrix);
	
	

	void Update() override;
	void Destroy() override;

	void OnImGuiRender() override;

	void JSONLoad() override;

	void SetIsDrawEnabled(bool visible = true) { m_drawEnabled = visible; }
	bool GetIsDrawEnabled() { return m_drawEnabled; }

	void SetWorldMat(const XMMATRIX& matrix) { worldMatrix = matrix; }

	Model* GetModel() { return &model; }

protected:
	bool m_drawEnabled = true;

	std::string filepath;
	Model model;
	Material* m_pMaterial;
	XMMATRIX worldMatrix = XMMatrixIdentity();
};