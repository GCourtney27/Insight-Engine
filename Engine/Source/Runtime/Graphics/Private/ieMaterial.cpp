#include <Engine_pch.h>

#include "Graphics/Public/ieMaterial.h"

namespace Insight
{
	void ieMaterial::CreateFromMemory(DataBlob Memory)
	{
		Initialize();
		return;
		UInt8* Data = Memory.GetBufferPointer();

		// 'A'
		Data++;
		// Albedo Str Len
		UInt8 AlbedoStrLen = *Data;
		char* AlbedoStr = (char*)_malloca(AlbedoStrLen + 1); // Allocate the string to store the path + null char
		Data++;

		// N
		Data++;
		UInt8 NormalStrLen = *Data;
		char* NormalStr = (char*)_malloca(NormalStrLen + 1); // Allocate the string to store the path + null char
		Data++;

		memcpy(AlbedoStr, Data, AlbedoStrLen);
		AlbedoStr[AlbedoStrLen] = '\0';
		Data += AlbedoStrLen;

		memcpy(NormalStr, Data, NormalStrLen);
		NormalStr[NormalStrLen] = '\0';

		// Load the resources.
		std::wstring wAlbedo = StringHelper::StringToWide(AlbedoStr);
		m_AlbedoTexture = Graphics::g_pTextureManager->LoadTexture(wAlbedo, Graphics::DT_Magenta2D, false);

		std::wstring wNormal= StringHelper::StringToWide(NormalStr);
		m_NormalTexture = Graphics::g_pTextureManager->LoadTexture(wNormal, Graphics::DT_Magenta2D, false);


		Initialize();
	}

	void ieMaterial::WriteToFile()
	{
		// --------
		//  Header
		// --------
		// A # -> Key String and Size in Bytes
		// N # -> Key String and Size in Bytes
		// R # -> Key String and Size in Bytes
		// M # -> Key String and Size in Bytes
		// Albedo tex path
		// Normal tex path
		// Roughness tex path
		// Metallic tex path
		

		// Set the Length of the textures file path if it is valid.
#define SetTexturePathLength(TexPtr, DstPtr) \
		if (TexPtr.IsValid())\
			*DstPtr = (UInt8)TexPtr.GetCacheKey().length();\
		else\
			*DstPtr = 0;\

		// Tex specifier + Int to store path str length + String lengths
		size_t DataLengths = 2 + 2 + m_AlbedoTexture.GetCacheKey().length() + m_NormalTexture.GetCacheKey().length();

		char* pFileMem = (char*)_alloca(DataLengths);
		void* pMemStart = pFileMem;
		ZeroMemRanged(pFileMem, DataLengths);

		// Write the header.
		*pFileMem = 'A'; pFileMem++;
		SetTexturePathLength(m_AlbedoTexture, pFileMem) pFileMem++;
		*pFileMem = 'N'; pFileMem++;
		SetTexturePathLength(m_NormalTexture, pFileMem) pFileMem++;

		// Wrtie the data.
		std::string Albedo =
#if UNICODE
			StringHelper::WideToString(m_AlbedoTexture.GetCacheKey());
#else
			m_AlbedoTexture.GetCacheKey();
#endif
		memcpy(pFileMem, Albedo.c_str(), Albedo.length());
		pFileMem += Albedo.length();

		std::string Normal =
#if UNICODE
			StringHelper::WideToString(m_NormalTexture.GetCacheKey());
#else
			m_NormalTexture.GetCacheKey();
#endif
		memcpy(pFileMem, Normal.c_str(), Normal.length());


		FILE* pFile = NULL;
		pFile = fopen("M_RustedMetal.ieMat", "wb");
		fwrite(pMemStart, DataLengths, 1, pFile);

		fclose(pFile);

#undef SetTexturePathLength

	}
}
