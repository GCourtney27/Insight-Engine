#pragma once
#include "Texture.h"
#include "Shaders.h"
#include "..\Graphics\ConstantBuffers.h"

#include "document.h"
#include "json.h"

#include <vector>
#include <map>

class Material
{
public:
	enum eMaterialType
	{
		PBR_MAPPED, // Material is expecting mapps (albedo, roughness, normal, metallic)
		PBR_MAPPED_WITHSCALERS, // Material can have values scale its textures (more rough less metallic etc.)
		PBR_UNTEXTURED, // No Textures all albedo, roughness and metallic are defined through shader inputs fo the material through ImGuiRender
		PBR_SKY // Material only exepts a .dds defining the sky sphere look
		//DEFAULT NO PBR
	};

	enum eFlags
	{
		NOFLAGS, // Default opaque shader
		FOLIAGE, // Requires opacity culling in shader
		TERRAIN // Requires tessilation shader
	};

public:
	//Material(ID3D11Device * device, ID3D11DeviceContext * deviceContext, eMaterialType materialType, eFlags flags, std::vector<std::string> textureLocations);
	virtual bool Initiailze(ID3D11Device * device, ID3D11DeviceContext * deviceContext, const rapidjson::Value& assetsInformation, eFlags materialAttributeFlags) = 0;

	static eMaterialType GetMaterialTypeFromString(std::string str_material);
	std::string GetMaterialTypeAsString();
	eMaterialType GetMaterialType() { return m_materialType; }

	Material* SetMaterialByType(eMaterialType materialType);

	void PSSetShader();
	void VSSetShader();
	void IASetInputLayout();

	std::vector<Texture> m_textures;
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexShader; // This objects buffer for world, view and project matricies
	ConstantBuffer<CB_PS_PerObjectColor> cb_ps_perObjectColor;

	DirectX::XMFLOAT3 m_color;
	float m_metallic = 0.0f;
	float m_roughness = 0.0f;

protected:
	virtual bool InitializePiplineAssets(const rapidjson::Value& assetsInformation) = 0; // Weather it is textured or not, a json object needs to be read from to initialize its A, N, R, M values
	virtual void InitializeShaders() = 0;

	VertexShader m_vertexShader; // Every Material has shaders, its just how they get initialized that is different. Some could use .cso files that others dont
	PixelShader m_pixelShader;
	// Internal for shaders

	// External for shaders
	
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