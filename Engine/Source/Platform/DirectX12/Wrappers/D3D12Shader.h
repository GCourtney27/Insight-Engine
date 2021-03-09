#pragma once

#include <Runtime/CoreMacros.h>

namespace Insight {

	class D3D12Shader
	{
	public:
		D3D12Shader()
			: m_pShader(nullptr)
		{
			m_ShaderByteCode = { 0 };
		}

		~D3D12Shader()
		{
			if(m_pShader)
				delete[] m_pShader;
		}

		/*
			Loads a precompiled D3D12 shader. Returns S_OK if succeeded, or error code if not.
			@param pFilePath - File path to the compiled shader code.
		*/
		inline HRESULT LoadFromFile(const TChar* pFilePath)
		{
			HRESULT hr;
			
			FILE* fp = _wfopen(pFilePath, TEXT("rb"));
			if (!fp)
			{
				hr = HRESULT_FROM_WIN32(GetLastError());
				IE_LOG(Error, TEXT("Failed to open file with path: %s"), pFilePath);
				return hr;
			}

			// Get the length of the file.
			fseek(fp, 0, SEEK_END);
			size_t BytecodeLength;
			BytecodeLength = ftell(fp);

			// Get the data in the file.
			fseek(fp, 0, SEEK_SET);
			m_pShader = new char[BytecodeLength];
			fread(m_pShader, 1, BytecodeLength, fp);

			// Close it.
			//fclose(fp);

			m_ShaderByteCode.BytecodeLength = BytecodeLength;
			m_ShaderByteCode.pShaderBytecode = m_pShader;

			IE_ASSERT(IsSigned(m_ShaderByteCode));

			hr = (m_ShaderByteCode.pShaderBytecode) ? S_OK : HRESULT_FROM_WIN32(GetLastError());

			return hr;
		}

		inline const D3D12_SHADER_BYTECODE& GetByteCode() const
		{
			return m_ShaderByteCode;
		}

	protected:

		/*
			Returns true if the specified bytecode is signed by the shader compiler. False if not.
		*/
		bool IsSigned(D3D12_SHADER_BYTECODE& shader)
		{
			char* p = (char*)shader.pShaderBytecode;
			// Make sure the provided bytecode is a shader. DXBC must be included 
			// in the firt four bytes of the file
			assert(p[0] == 'D' && p[1] == 'X' && p[2] == 'B' && p[3] == 'C');

			// Signed shaders should not have 16 bytes of 0's proceeding 'DXBC'
			return *(uint64_t*)&p[4] != 0 && *(uint64_t*)&p[12] != 0;
		}

	private:
		D3D12_SHADER_BYTECODE m_ShaderByteCode;
		void* m_pShader;
	};

}
