#include <Engine_pch.h>

#include "D3D11Shader.h"

#include "Platform/Win32/Error/COMException.h"

namespace Insight {

	bool VertexShader::Init(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, std::wstring& ShaderPath, D3D11_INPUT_ELEMENT_DESC* pLayoutDesc, UINT NumElements)
	{
		// If the shader exists already then we are probably resising a window or
		// doing something else. Dont read in a file or recreate the shader, just return.
		if (m_pShader.Get()) {
			return true;
		}
		//HRESULT hr = D3DReadFileToBlob(ShaderPath.c_str(), m_pShaderByteCode.GetAddressOf());
		//ThrowIfFailed(hr, "Failed to read D3D 11 vertex shader from file.");

		/*hr = pDevice->CreateVertexShader(m_pShaderByteCode->GetBufferPointer(), m_pShaderByteCode->GetBufferSize(), NULL, m_pShader.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create D3D 11 vertex shader from blob.");

		hr = pDevice->CreateInputLayout(pLayoutDesc, NumElements, m_pShaderByteCode->GetBufferPointer(), m_pShaderByteCode->GetBufferSize(), m_pInputLayout.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create D3D 11 vertex shader input layout.");*/

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

	void VertexShader::Reload()
	{
		m_pInputLayout.Reset();
		m_pShaderByteCode.Reset();
		m_pShader.Reset();
	}




	bool PixelShader::Init(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring& shaderpath)
	{
		// If the shader exists already then we are probably resising a window or
		// doing something else. Dont read in a file or recreate the shader, just return
		if (m_pShader.Get()) {
			return true;
		}
		HRESULT hr = E_FAIL;// = D3DReadFileToBlob(shaderpath.c_str(), m_pShaderByteCode.GetAddressOf());
		ThrowIfFailed(hr, "Failed to read D3D 11 pixel shader from file.");

		hr = device->CreatePixelShader(m_pShaderByteCode.Get()->GetBufferPointer(), m_pShaderByteCode.Get()->GetBufferSize(), NULL, m_pShader.GetAddressOf());
		ThrowIfFailed(hr, "Failed to create D3D 11 pixel shader from blob.");

		return true;
	}

	ID3D11PixelShader* PixelShader::GetShader()
	{
		return m_pShader.Get();
	}

	ID3D10Blob* PixelShader::GetBuffer()
	{
		return m_pShaderByteCode.Get();
	}

	void PixelShader::Reload()
	{
		m_pShaderByteCode.Reset();
		m_pShader.Reset();
	}


}
