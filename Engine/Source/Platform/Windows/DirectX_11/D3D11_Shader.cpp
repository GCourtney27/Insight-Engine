#include <ie_pch.h>

#include "D3D11_Shader.h"

#include "Platform/Windows/Error/COM_Exception.h"

namespace Insight {

	bool VertexShader::Init(ComPtr<ID3D11Device>& pDevice, const std::wstring& ShaderPath, D3D11_INPUT_ELEMENT_DESC* pLayoutDesc, UINT NumElements)
	{
		HRESULT hr = D3DReadFileToBlob(ShaderPath.c_str(), m_pShaderByteCode.GetAddressOf());
		ThrowIfFailed(hr, "Failed to read D3D 11 vertex shader from file.");

		hr = pDevice->CreateVertexShader(m_pShaderByteCode->GetBufferPointer(), m_pShaderByteCode->GetBufferSize(), NULL, m_pShader.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create D3D 11 vertex shader from blob.");

		hr = pDevice->CreateInputLayout(pLayoutDesc, NumElements, m_pShaderByteCode->GetBufferPointer(), m_pShaderByteCode->GetBufferSize(), m_pInputLayout.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create D3D 11 vertex shader input layout.");

		return true;
	}

	ID3D11VertexShader* VertexShader::GetShader()
	{
		return m_pShader.Get();
	}

	ID3D10Blob* VertexShader::GetBuffer()
	{
		return m_pShaderByteCode.Get();
	}

	ID3D11InputLayout* VertexShader::GetInputLayout()
	{
		return m_pInputLayout.Get();
	}




	bool PixelShader::Init(ComPtr<ID3D11Device>& device, const std::wstring& shaderpath)
	{
		HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), m_ShaderByteCode.GetAddressOf());
		ThrowIfFailed(hr, "Failed to read D3D 11 pixel shader from file.");

		hr = device->CreatePixelShader(m_ShaderByteCode.Get()->GetBufferPointer(), m_ShaderByteCode.Get()->GetBufferSize(), NULL, m_Shader.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create D3D 11 pixel shader from blob.");

		return true;
	}

	ID3D11PixelShader* PixelShader::GetShader()
	{
		return m_Shader.Get();
	}

	ID3D10Blob* PixelShader::GetBuffer()
	{
		return m_ShaderByteCode.Get();
	}


}
