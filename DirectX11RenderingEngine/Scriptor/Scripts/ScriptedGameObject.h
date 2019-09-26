#pragma once
#include "../Graphics/BaseScriptableGameObject.h"

class ScriptedGameObejct : public BaseScriptableGameObject
{
public:
	ScriptedGameObejct();
	~ScriptedGameObejct();

	bool Initialize(const std::string & filepath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader) override;
	void Draw(const XMMATRIX & viewProjectionMatrix) override;

private:
	Model model;

};