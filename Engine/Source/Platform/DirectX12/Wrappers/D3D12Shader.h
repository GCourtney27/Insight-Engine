#pragma once

#include <Runtime/CoreMacros.h>

namespace Insight {

	class D3D12Shader
	{
	public:
		D3D12Shader()
			: m_pShader(nullptr)
		{
			m_ShaderByteCode = {};
		}

		~D3D12Shader()
		{
			delete[] m_pShader;
		}

		/*
			Loads a precompiled D3D12 shader. Returns S_OK if succeeded, or error code if not.
			@param pFilePath - File path to the compiled shader code.
		*/
		inline HRESULT LoadFromFile(const wchar_t* pFilePath)
		{
			HRESULT hr;
			FILE* fp = fopen(StringHelper::WideToString(pFilePath).c_str(), "rb");
			if (!fp)
			{
				hr = HRESULT_FROM_WIN32(GetLastError());
				return hr;
			}

			fseek(fp, 0, SEEK_END);
			size_t BytecodeLength;
			BytecodeLength = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			m_pShader = new char[BytecodeLength];
			fread(m_pShader, 1, BytecodeLength, fp);
			fclose(fp);

			m_ShaderByteCode.BytecodeLength = BytecodeLength;
			m_ShaderByteCode.pShaderBytecode = m_pShader;

			hr = (m_ShaderByteCode.pShaderBytecode) ? S_OK : HRESULT_FROM_WIN32(GetLastError());

			return hr;
		}

		inline const D3D12_SHADER_BYTECODE& GetByteCode() const
		{
			return m_ShaderByteCode;
		}

	private:
		D3D12_SHADER_BYTECODE m_ShaderByteCode;
		void* m_pShader;
	};

}
