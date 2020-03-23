#include "ScriptedGameObject.h"

ScriptedGameObejct::ScriptedGameObejct()
{
	m_name = "Hello World";
}

ScriptedGameObejct::~ScriptedGameObejct()
{
}

extern "C" ScriptedGameObejct * ScriptedGameObejct::factory()
{
	return new ScriptedGameObejct;
}

bool ScriptedGameObejct::Initialize(const std::string & filepath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	if (!model.Initialize(filepath, device, deviceContext, cb_vs_vertexshader))
		return false;

	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateMatrix();

	//sphere_radius = 20.0f;
	//sphere_position = GetPositionFloat3();

	aabb.Initialize(20.0f, GetPositionFloat3());

	return true;
}

void ScriptedGameObejct::Draw(const XMMATRIX & viewProjectionMatrix)
{
	model.Draw(this->worldMatrix, viewProjectionMatrix);
	this->SetPosition(0.0f, 0.0f, 0.0f);

}

extern "C" ScriptedGameObejct* factory(void)
{
	return new ScriptedGameObejct;
}
