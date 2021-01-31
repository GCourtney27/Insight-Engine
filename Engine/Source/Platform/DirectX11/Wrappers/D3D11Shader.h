#pragma once

#include <Insight/Core.h>

namespace Insight {

	

	class INSIGHT_API Shader
	{

	};

	class INSIGHT_API VertexShader : public Shader
	{
	public:
		bool Init(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, std::wstring& ShaderPath, D3D11_INPUT_ELEMENT_DESC* pLayoutDesc, UINT NumElements);
		ID3D11VertexShader* GetShader();
		ID3D10Blob* GetBuffer();
		ID3D11VertexShader** GetShaderAddress() { return m_pShader.GetAddressOf(); }
		ID3D11InputLayout* GetInputLayout();
		
		void Reload();

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pShader = nullptr;
		Microsoft::WRL::ComPtr<ID3D10Blob> m_pShaderByteCode = nullptr;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout = nullptr;
	};


	class INSIGHT_API PixelShader : public Shader
	{
	public:
		bool Init(Microsoft::WRL::ComPtr<ID3D11Device>& pDevice, std::wstring& Shaderpath);
		ID3D11PixelShader* GetShader();
		ID3D11PixelShader** GetShaderAddress() { return m_pShader.GetAddressOf(); }
		ID3D10Blob* GetBuffer();
		
		void Reload();

	private:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pShader = nullptr;
		Microsoft::WRL::ComPtr<ID3D10Blob> m_pShaderByteCode = nullptr;
	};

}

