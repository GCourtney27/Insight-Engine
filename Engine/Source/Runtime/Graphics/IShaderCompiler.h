#pragma once

#include <Runtime/Core.h>

namespace Insight
{

	class INSIGHT_API IShaderCompiler
	{
	public:
		/*
			Compile a singe shader and store the result in the cache directory.
		*/
		virtual void* CompileShader(const TChar* FilePath) = 0;

		virtual void CompileShaders(const TChar* Directory, bool bForceRecompile) = 0;

	private:
		IShaderCompiler() = default;
		~IShaderCompiler() = default;
	};

	/*class INSGHT_API D3D12ShaderCompiler : public IShaderCompiler
	{
	public:
		D3D12ShaderCompiler() = default;
		~D3D12ShaderCompiler() = default;

		virtual void* CompileShader(const TChar* FilePath) override;

		virtual void CompileShaders(const TChar* Directory, bool bForceRecompile) override;

	};*/
	/*
	
#if !IE_PLATFORM_BUILD_XBOX_ONE
	static HRESULT CompileShader(const WChar* FileName, const WChar* Target, const WChar* DebugName, const WChar* FilePath)
	{
		std::vector<LPCWSTR> CompileCommands;
		CompileCommands.emplace_back(DebugName);
		CompileCommands.emplace_back(L"-E"); CompileCommands.emplace_back(L"main");
		CompileCommands.emplace_back(L"-T"); CompileCommands.emplace_back(Target); // Compile target
#if IE_DEBUG
		CompileCommands.emplace_back(L"-Zi"); // Enable debug information
		std::wstring CSO = FileName; CSO.append(L".cso");
		std::wstring PDB = FileName; PDB.append(L".pdb");
		CompileCommands.emplace_back(L"-Fo"); CompileCommands.emplace_back(CSO.c_str()); // output obj file
		CompileCommands.emplace_back(L"-Fd"); CompileCommands.emplace_back(PDB.c_str()); // Write debug information to file
		CompileCommands.emplace_back(L"-Od"); // Disable optimizations
#endif
		CompileCommands.emplace_back(L"-Zpr"); // Pack matricies in row-major

		Microsoft::WRL::ComPtr<IDxcUtils> pUtils;
		Microsoft::WRL::ComPtr<IDxcCompiler3> pCompiler;
		DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&pUtils));
		DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&pCompiler));

		CComPtr<IDxcIncludeHandler> pIncludeHandler;
		ThrowIfFailed(pUtils->CreateDefaultIncludeHandler(&pIncludeHandler), TEXT("Failed to create default include handler for shader compiler."));

		// 
		// Load the file
		//
		CComPtr<IDxcBlobEncoding> pSource = NULL;
		HRESULT hr = pUtils->LoadFile(FilePath, NULL, &pSource);
		ThrowIfFailed(hr, TEXT("Failed to load shader file for compilation."))
		DxcBuffer Source;
		Source.Ptr = pSource->GetBufferPointer();
		Source.Size = pSource->GetBufferSize();
		Source.Encoding = DXC_CP_ACP;

		//
		// Compile the shader
		//
		CComPtr<IDxcResult> pVSResult;
		hr = pCompiler->Compile(
			&Source,
			CompileCommands.data(),
			(UINT32)CompileCommands.size(),
			pIncludeHandler,
			IID_PPV_ARGS(&pVSResult)
		);
		ThrowIfFailed(hr, TEXT("Failed to compile shader file."));

		//
		// Check for errors in the compilation
		//
		CComPtr<IDxcBlobUtf8> pErrors = NULL;
		hr = pVSResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&pErrors), NULL);
		if (pErrors != NULL && pErrors->GetStringLength() != 0)
		{
			IE_LOG(Warning, TEXT("Errors while compilng shader: %s"), StringHelper::StringToWide(pErrors->GetStringPointer()).c_str());
		}

		ThrowIfFailed(hr, TEXT("Failed to get error output for compiled shader result."));

		pVSResult->GetStatus(&hr);
		if (FAILED(hr))
		{
			IE_LOG(Error, TEXT("There was a error while compiling shaders."));
			return hr;
		}


		//
		// Save shader binary.
		//
		CComPtr<IDxcBlob> pShader = nullptr;
		CComPtr<IDxcBlobUtf16> pShaderName = nullptr;
		pVSResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pShader), &pShaderName);
		if (pShader != nullptr)
		{
			FILE* fp = NULL;
			wchar_t FilePath[128];
			swprintf_s(FilePath, L"Shaders/ShaderCache/HLSL/%s", reinterpret_cast<const wchar_t*>(pShaderName->GetStringPointer()));
			if (fp)
			{
				_wfopen_s(&fp, FilePath, L"wb");
				fwrite(pShader->GetBufferPointer(), pShader->GetBufferSize(), 1, fp);
				fclose(fp);
			}
		}

#if IE_DEBUG
		//
		// Save the shader pdb
		//
		CComPtr<IDxcBlob> pPDB = nullptr;
		CComPtr<IDxcBlobUtf16> pPDBName = nullptr;
		pVSResult->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(&pPDB), &pPDBName);
		{
			FILE* fp = NULL;

			// Note that if you don't specify -Fd, a pdb name will be automatically generated. Use this file name to save the pdb so that PIX can find it quickly.
			wchar_t FilePath[128];
			swprintf_s(FilePath, L"Shaders/ShaderCache/HLSL/%s", reinterpret_cast<const wchar_t*>(pPDBName->GetStringPointer()));
			_wfopen_s(&fp, FilePath, L"wb");
			if (fp)
			{
				fwrite(pPDB->GetBufferPointer(), pPDB->GetBufferSize(), 1, fp);
				fclose(fp);
			}
		}
#endif

		return S_OK;
	}

	//void Direct3D12Context::TryCompiledShaders(bool bForceRecompile/* = false*/
//	{
//	if (std::filesystem::exists("Shaders/ShaderCache/HLSL/"))
//	{
//		if (!bForceRecompile)
//		{
//			IE_LOG(Log, TEXT("Shader cache found."))
//				return;
//		}
//	}
//	else
//	{
//		IE_LOG(Warning, TEXT("No shader cache found."));
//		std::filesystem::create_directory("Shaders/ShaderCache/HLSL/");
//	}
//
//	IE_LOG(Log, TEXT("Compiling shaders, please wait..."));
//
//	ScopedSecondTimer(TEXT("Shader compilation"));
//	using RecursiveDirectoryIter = std::filesystem::recursive_directory_iterator;
//	for (const auto& Directory : RecursiveDirectoryIter("Shaders\\HLSL\\"))
//	{
//		size_t firstPos = Directory.path().string().find_first_of(".") + 1;
//		size_t lastPos = Directory.path().string().find_last_of(".");
//		if (firstPos != std::string::npos && lastPos != std::string::npos)
//		{
//			EString FileName = StringHelper::GetFilenameFromDirectory(Directory.path().wstring());
//			EString FilePathW = Directory.path().wstring();
//
//			EString Ext = Directory.path().wstring().substr(firstPos);
//
//			//if (ext != L"hlsli" && ext != L"bat")
//			//if (StrCmpCW(ext.c_str(), TEXT("hlsli")) != 0) // TODO: Fastest, UWP compatible? Uses shlwapi.h
//			if (Ext.compare(TEXT("hlsli")) != 0)
//			{
//				TChar ShaderTarget[8] = { 0 };
//				if (Ext.compare(L"lib.hlsl") == 0)
//					swprintf_s(ShaderTarget, TEXT("%c%c%c_6_3"), Ext[0], Ext[1], Ext[2]);
//				else
//					swprintf_s(ShaderTarget, TEXT("%c%c_6_0"), Ext[0], Ext[1]);
//
//				CompileShader(FileName.c_str(), ShaderTarget, FileName.c_str(), FilePathW.c_str());
//			}
//		}
//	}
//
//	IE_LOG(Log, TEXT("Shaders compiled."));
//	}
//#endif
//	*/
}
