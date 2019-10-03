#pragma once
#include "../../Graphics/BaseScriptableGameObject.h"

class ScriptedGameObejct : public BaseScriptableGameObject
{
public:
	ScriptedGameObejct();
	~ScriptedGameObejct();
	ScriptedGameObejct* factory();

	bool Initialize(const std::string & filepath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader) override;
	void Draw(const XMMATRIX & viewProjectionMatrix) override;

	const char* GetName() { return m_name; }

private:
	Model model;
	const char * m_name = nullptr;
};