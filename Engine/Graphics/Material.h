#pragma once
#include "Texture.h"
#include "Shaders.h"
#include "..\Graphics\ConstantBuffers.h"

#include "document.h"
#include "json.h"
#include "prettywriter.h"
#include "ostreamwrapper.h"
#include "stringbuffer.h"
#include "filewritestream.h"
#include "istreamwrapper.h"
#include "writer.h"

#include <vector>
#include <map>

class Material
{
public:
	enum eMaterialType
	{
		PBR_DEFAULT, // Material is expecting mapps (albedo, roughness, normal, metallic)
		PBR_UNTEXTURED, // No Textures all albedo, roughness and metallic are defined through shader inputs for the material through ImGuiRender
		PBR_SKY // Material only exepts a .dds defining the sky sphere look
		//DEFAULT NO PBR
	};

	enum eFlags
	{
		NOFLAGS, // Default opaque shader
		FOLIAGE, // Requires opacity culling in shader
		TERRAIN, // Requires tessilation shader
		WATER
	};

public:
	//Material(ID3D11Device * device, ID3D11DeviceContext * deviceContext, eMaterialType materialType, eFlags flags, std::vector<std::string> textureLocations);
	virtual bool Initiailze(ID3D11Device * device, ID3D11DeviceContext * deviceContext, const rapidjson::Value& assetsInformation, eFlags materialAttributeFlags) = 0;
	virtual bool Initiailze(ID3D11Device * device, ID3D11DeviceContext * deviceContext, eFlags materialAttributeFlags) = 0;

	virtual void WriteToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) = 0;

	static eFlags GetMaterialFlagsFromString(std::string str_flags);
	std::string GetMaterialFlagsAsString();
	eFlags GetMaterialFlags() { return m_flags; }

	static eMaterialType GetMaterialTypeFromString(std::string str_material);
	std::string GetMaterialTypeAsString();
	eMaterialType GetMaterialType() { return m_materialType; }

	Material* SetMaterialByType(eMaterialType materialType, eFlags flags);

	void PSSetShader();
	void VSSetShader();
	void IASetInputLayout();
	virtual void PSSetShaderResources() = 0;

	std::vector<Texture> m_textures;

	ConstantBuffer<CB_VS_PerObject> m_cb_vs_PerObject; // This objects buffer for world, view and project matricies
	ConstantBuffer<CB_VS_PerObject_Util> m_cb_vs_PerObjectUtil;
	ConstantBuffer<CB_PS_PerObject_Util> m_cb_ps_PerObjectUtil;

	DirectX::XMFLOAT2 m_newUVOffset;
	DirectX::XMFLOAT2 m_tiling;
	DirectX::XMFLOAT3 m_newVertOffset;

	DirectX::XMFLOAT3 m_color;
	float m_metallic = 0.0f;
	float m_roughness = 0.0f;

protected:
	virtual bool InitializeJOSNPiplineAssets(const rapidjson::Value& assetsInformation) = 0; // Weather it is textured or not, a json object needs to be read from to initialize its A, N, R, M values
	virtual bool InitializePiplineAssets() = 0;

	virtual void InitializeShaders() = 0;

	VertexShader m_vertexShader; // Every Material has shaders, its just how they get initialized that is different. Some could use .cso files that others dont
	PixelShader m_pixelShader;

	

	eMaterialType m_materialType;
	eFlags m_flags;

	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
	Microsoft::WRL::ComPtr <ID3D11DeviceContext> m_pDeviceContext;


#pragma region DetermineShaderPath
#ifdef _DEBUG // Debug Mode
#ifdef _WIN64 //x64
		std::wstring m_shaderFolder = L"..\\bin\\x64\\Debug\\";
#else // x86 (Win32)
		std::wstring m_shaderFolder = L"..\\bin\\Win32\\Debug\\";
#endif
#else // Release Mode
#ifdef _WIN64 //x64
		std::wstring m_shaderFolder = L"..\\bin\\x64\\Release\\";
#else //x86 (Win32)
		std::wstring m_shaderFolder = L"..\\bin\\Win32\\Release\\";
#endif
#endif
#pragma endregion

};