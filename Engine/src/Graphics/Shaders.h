#pragma once
#include "..\\ErrorLogger.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>

class Shader {};

class VertexShader : public Shader
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device, std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC *layoutDesc, UINT numElements);
	ID3D11VertexShader * GetShader();
	ID3D11VertexShader ** GetShaderAddress() { return shader.GetAddressOf(); }
	ID3D10Blob * GetBuffer();
	ID3D11InputLayout * GetInputLayout();
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader = nullptr;
	Microsoft::WRL::ComPtr<ID3D10Blob> shader_buffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};

class PixelShader : public Shader
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>&device, std::wstring shaderpath);
	ID3D11PixelShader * GetShader();
	ID3D11PixelShader ** GetShaderAddress() { return shader.GetAddressOf(); }
	ID3D10Blob * GetBuffer();
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shader = nullptr;
	Microsoft::WRL::ComPtr<ID3D10Blob> shader_buffer = nullptr;
};