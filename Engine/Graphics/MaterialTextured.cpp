#include "MaterialTextured.h"


bool MaterialTextured::Initiailze(ID3D11Device * device, ID3D11DeviceContext * deviceContext, const rapidjson::Value& assetsInformation, eFlags materialAttributeFlags)
{
	this->m_pDevice = device;
	this->m_pDeviceContext = deviceContext;
	this->m_flags = materialAttributeFlags;
	InitializeJOSNPiplineAssets(assetsInformation);

	return true;
}

bool MaterialTextured::Initiailze(ID3D11Device * device, ID3D11DeviceContext * deviceContext, eFlags materialAttributeFlags)
{
	this->m_pDevice = device;
	this->m_pDeviceContext = deviceContext;
	this->m_flags = materialAttributeFlags;
	InitializePiplineAssets();

	return true;
}

void MaterialTextured::WriteToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.Key("MaterialType");
	writer.String(this->GetMaterialTypeAsString().c_str());
	writer.Key("MaterialFlags");
	writer.String(this->GetMaterialFlagsAsString().c_str());
	writer.Key("Albedo");
	writer.String(m_textureLocations[0].c_str());
	writer.Key("Normal");
	writer.String(m_textureLocations[1].c_str());
	writer.Key("Metallic");
	writer.String(m_textureLocations[2].c_str());
	writer.Key("Roughness");
	writer.String(m_textureLocations[3].c_str());

	writer.Key("uvOffset");
	writer.StartArray();// Begin uvOffset

	writer.StartObject();
	writer.Key("x");
	writer.Double(m_newUVOffset.x);
	writer.Key("y");
	writer.Double(m_newUVOffset.y);
	writer.EndObject();

	writer.EndArray();// End uvOffset

	writer.Key("Tiling");
	writer.StartArray();// Start Tiling
	
	writer.StartObject();
	writer.Key("x");
	writer.Double(m_tiling.x);
	writer.Key("y");
	writer.Double(m_tiling.y);
	writer.EndObject();
	
	writer.EndArray();// End Tiling

	writer.Key("Color_Override");
	writer.StartArray();// Start Color

	writer.StartObject();
	writer.Key("r");
	writer.Double(m_color.x);
	writer.Key("g");
	writer.Double(m_color.y);
	writer.Key("b");
	writer.Double(m_color.z);
	writer.EndObject();

	writer.EndArray();// End Array

	writer.Key("Metallic_Override");
	writer.Double(m_metallic);

	writer.Key("Roughness_Override");
	writer.Double(m_roughness);

}

bool MaterialTextured::InitializeJOSNPiplineAssets(const rapidjson::Value& assetsInformation)
{
	std::string albedo_Filepath;
	std::string normal_Filepath;
	std::string metallic_Filepath;
	std::string roughness_Filepath;
	std::string ao_Filepath;
	std::vector<std::string> textures;

	float uvOffsetX = 0.0f;
	float uvOffsetY = 0.0f;
	float tilingX = 0.0f;
	float tilingY = 0.0f;

	float colorR = 1.0f;
	float colorG = 1.0f;
	float colorB = 1.0f;
	float metallic = 0.0f;
	float roughness = 0.0f;

	// Textures
	json::get_string(assetsInformation[1], "Albedo", albedo_Filepath);
	json::get_string(assetsInformation[1], "Normal", normal_Filepath);
	json::get_string(assetsInformation[1], "Metallic", metallic_Filepath);
	json::get_string(assetsInformation[1], "Roughness", roughness_Filepath);
	json::get_string(assetsInformation[1], "AO", ao_Filepath);

	// UVs and Tiling
	json::get_float(assetsInformation[1]["uvOffset"][0], "x", uvOffsetX);
	json::get_float(assetsInformation[1]["uvOffset"][0], "y", uvOffsetY);
	json::get_float(assetsInformation[1]["Tiling"][0], "x", tilingX);
	json::get_float(assetsInformation[1]["Tiling"][0], "y", tilingY);

	// Color Overrides
	json::get_float(assetsInformation[1]["Color_Override"][0], "r", colorR);
	json::get_float(assetsInformation[1]["Color_Override"][0], "g", colorG);
	json::get_float(assetsInformation[1]["Color_Override"][0], "b", colorB);
	json::get_float(assetsInformation[1], "Metallic_Override", metallic);
	json::get_float(assetsInformation[1], "Roughness_Override", roughness);


	textures.push_back(albedo_Filepath);
	textures.push_back(normal_Filepath);
	textures.push_back(metallic_Filepath);
	textures.push_back(roughness_Filepath);
	textures.push_back(ao_Filepath);
	m_textureLocations = textures;

	std::vector<std::string>::iterator iter;
	for (iter = m_textureLocations.begin(); iter != m_textureLocations.end(); iter++)
	{
		this->m_textures.push_back(Texture(this->m_pDevice.Get(), (*iter)));
	}

	InitializeShaders();

	// Per-Object modifiers for Pixel Shader
	m_color.x = colorR;
	m_color.y = colorG;
	m_color.z = colorB;
	m_cb_ps_PerObjectUtil.data.color = m_color;
	m_metallic = metallic;
	m_cb_ps_PerObjectUtil.data.metallic = m_metallic;
	m_roughness = roughness;
	m_cb_ps_PerObjectUtil.data.roughness = m_roughness;
	m_cb_ps_PerObjectUtil.ApplyChanges();

	// Per-Object modifiers for Vertex Shader
	m_newUVOffset.x = uvOffsetX;
	m_newUVOffset.y = uvOffsetY;
	m_tiling.x = tilingX;
	m_tiling.y = tilingY;
	m_cb_vs_PerObjectUtil.data.tiling = m_tiling;
	m_cb_vs_PerObjectUtil.data.uvOffset = m_newUVOffset;
	m_cb_vs_PerObjectUtil.data.vertOffset = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_cb_vs_PerObjectUtil.ApplyChanges();

	return true;
}

bool MaterialTextured::InitializePiplineAssets()
{
	std::string tex_albedo = "..\\Assets\\Objects\\Norway\\Opaque\\Rock02\\Rock02_Albedo.jpg";
	std::string tex_normal = "..\\Assets\\Objects\\Norway\\Opaque\\Rock02\\Rock02_Normal.jpg";
	std::string tex_opacity = "..\\Assets\\Objects\\Norway\\Opaque\\Rock02\\Rock02_Specular.jpg";
	std::string tex_roughness = "..\\Assets\\Objects\\Norway\\Opaque\\Rock02\\Rock02_Roughness.jpg";
	std::vector<std::string> textures;

	textures.push_back(tex_albedo);
	textures.push_back(tex_normal);
	textures.push_back(tex_opacity);
	textures.push_back(tex_roughness);
	m_textureLocations = textures;

	float uvOffsetX = 0.0f;
	float uvOffsetY = 0.0f;
	float tilingX = 1.0f;
	float tilingY = 1.0f;

	float colorR = 1.0f;
	float colorG = 1.0f;
	float colorB = 1.0f;
	float metallic = 0.0f;
	float roughness = 0.0f;

	this->m_textures.push_back(Texture(this->m_pDevice.Get(), tex_albedo));
	this->m_textures.push_back(Texture(this->m_pDevice.Get(), tex_normal));
	this->m_textures.push_back(Texture(this->m_pDevice.Get(), tex_opacity));
	this->m_textures.push_back(Texture(this->m_pDevice.Get(), tex_roughness));

	InitializeShaders();

	// Per-Object modifiers for Pixel Shader
	m_color.x = colorR;
	m_color.y = colorG;
	m_color.z = colorB;
	m_cb_ps_PerObjectUtil.data.color = m_color;
	m_metallic = metallic;
	m_cb_ps_PerObjectUtil.data.metallic = m_metallic;
	m_roughness = roughness;
	m_cb_ps_PerObjectUtil.data.roughness = m_roughness;
	m_cb_ps_PerObjectUtil.ApplyChanges();

	// Per-Object modifiers for Vertex Shader
	m_newUVOffset.x = uvOffsetX;
	m_newUVOffset.y = uvOffsetY;
	m_tiling.x = tilingX;
	m_tiling.y = tilingY;
	m_cb_vs_PerObjectUtil.data.tiling = m_tiling;
	m_cb_vs_PerObjectUtil.data.uvOffset = m_newUVOffset;
	m_cb_vs_PerObjectUtil.data.vertOffset = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_cb_vs_PerObjectUtil.ApplyChanges();

	return true;
}

void MaterialTextured::InitializeShaders()
{
	D3D11_INPUT_ELEMENT_DESC defaultLayout3D[] =
	{
		{ "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{ "NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{ "TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{ "BITANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  }
	};
	UINT numElements3D = ARRAYSIZE(defaultLayout3D);

	if (!m_vertexShader.Initialize(m_pDevice, m_shaderFolder + L"PBR_Textured_vs.cso", defaultLayout3D, numElements3D))
		ErrorLogger::Log("Failed to initialize default vertex PBR shader");

	if (!m_pixelShader.Initialize(m_pDevice, m_shaderFolder + L"PBR_Textured_ps.cso"))
		ErrorLogger::Log("Failed to initialize pixel shader for default material");

	HRESULT hr = m_cb_vs_PerObject.Initialize(m_pDevice.Get(), m_pDeviceContext.Get());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer for vertex shader inside default material.");

	hr = m_cb_vs_PerObjectUtil.Initialize(m_pDevice.Get(), m_pDeviceContext.Get());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer for PerObjectColor inside default material.");

	hr = m_cb_ps_PerObjectUtil.Initialize(m_pDevice.Get(), m_pDeviceContext.Get());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer for PerObjectColor inside default material.");
}

void MaterialTextured::PSSetShaderResources()
{
	for (int i = 0; i < 5; i++)
	{
		this->m_pDeviceContext->PSSetShaderResources(i, 1, m_textures[i].GetTextureResourceViewAddress());
	}
}
