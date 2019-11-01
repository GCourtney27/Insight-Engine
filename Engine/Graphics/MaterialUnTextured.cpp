#include "MaterialUnTextured.h"
#include "Graphics.h"

bool MaterialUnTextured::Initiailze(ID3D11Device * device, ID3D11DeviceContext * deviceContext, const rapidjson::Value & assetsInformation, eFlags materialAttributeFlags)
{
	this->m_pDevice = device;
	this->m_pDeviceContext = deviceContext;
	this->m_flags = materialAttributeFlags;
	if (!InitializeJOSNPiplineAssets(assetsInformation))
	{
		ErrorLogger::Log("Failed to initialize pipeline assets for untextured material.");
		return false;
	}

	return true;
}

bool MaterialUnTextured::Initiailze(ID3D11Device * device, ID3D11DeviceContext * deviceContext, eFlags materialAttributeFlags)
{
	this->m_pDevice = device;
	this->m_pDeviceContext = deviceContext;
	this->m_flags = materialAttributeFlags;

	InitializePiplineAssets();

	return true;
}

void MaterialUnTextured::WriteToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.Key("MaterialType");
	writer.String(this->GetMaterialTypeAsString().c_str());
	writer.Key("Albedo");
	writer.StartArray(); // Start Albedo Array

	writer.StartObject(); // Start r
	writer.Key("r");
	writer.Double(this->m_color.x);
	writer.EndObject(); // End r

	writer.StartObject(); // Start g
	writer.Key("g");
	writer.Double(this->m_color.y);
	writer.EndObject(); // End g
	
	writer.StartObject(); // Start b
	writer.Key("b");
	writer.Double(this->m_color.z);
	writer.EndObject(); // End b

	writer.EndArray(); // End Albedo Array

	writer.Key("Normal");
	writer.String("..\\Assets\\Textures\\UnTexturedNormal.png");

	writer.Key("Metallic");
	writer.Double(this->m_metallic);

	writer.Key("Roughness");
	writer.Double(this->m_roughness);

	writer.Key("AO");
	writer.Double(0.5);
}

bool MaterialUnTextured::InitializePiplineAssets()
{
	float r = 1.0f;
	float g = 1.0f;
	float b = 1.0f;
	std::string normal_Filepath = "..\\Assets\\Textures\\UnTexturedNormal.png";
	float metallic_value = 0.5f;
	float roughness_value = 0.5f;
	float ao_value = 0.2f;
	std::vector<std::string> textures;

	textures.push_back(normal_Filepath);

	m_textureLocations = textures;

	std::vector<std::string>::iterator iter;
	for (iter = m_textureLocations.begin(); iter != m_textureLocations.end(); iter++)
	{
		this->m_textures.push_back(Texture(this->m_pDevice.Get(), (*iter)));
	}

	InitializeShaders();
	m_color.x = r;
	m_color.y = g;
	m_color.z = b;
	cb_ps_perObjectColor.data.color = m_color;
	m_metallic = metallic_value;
	cb_ps_perObjectColor.data.metallic = m_metallic;
	m_roughness = roughness_value;
	cb_ps_perObjectColor.data.roughnss = m_roughness;
	cb_ps_perObjectColor.ApplyChanges();

	return true;
}

bool MaterialUnTextured::InitializeJOSNPiplineAssets(const rapidjson::Value & assetsInformation)
{
	float r, g, b;
	std::string normal_Filepath;
	float metallic_value;
	float roughness_value;
	float ao_value;
	std::vector<std::string> textures;

	// Color
	const rapidjson::Value& colorInfo = assetsInformation[1]["Albedo"];
	json::get_float(colorInfo[0], "r", r);
	json::get_float(colorInfo[1], "g", g);
	json::get_float(colorInfo[2], "b", b);

	// Normal
	json::get_string(assetsInformation[1], "Normal", normal_Filepath);

	// Mettalic
	json::get_float(assetsInformation[1], "Metallic", metallic_value);

	// Roughness
	json::get_float(assetsInformation[1], "Roughness", roughness_value);

	// AO
	json::get_float(assetsInformation[1], "AO", ao_value);

	textures.push_back(normal_Filepath);

	m_textureLocations = textures;

	std::vector<std::string>::iterator iter;
	for (iter = m_textureLocations.begin(); iter != m_textureLocations.end(); iter++)
	{
		this->m_textures.push_back(Texture(this->m_pDevice.Get(), (*iter)));
	}

	InitializeShaders();
	m_color.x = r;
	m_color.y = g;
	m_color.z = b;
	cb_ps_perObjectColor.data.color = m_color;
	m_metallic = metallic_value;
	cb_ps_perObjectColor.data.metallic = m_metallic;
	m_roughness = roughness_value;
	cb_ps_perObjectColor.data.roughnss = m_roughness;
	cb_ps_perObjectColor.ApplyChanges();

	return true;
}

void MaterialUnTextured::InitializeShaders()
{
	D3D11_INPUT_ELEMENT_DESC defaultLayout3D[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"BITANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  }
	};
	UINT defaultNumElements3D = ARRAYSIZE(defaultLayout3D);

	if (!m_vertexShader.Initialize(m_pDevice, m_shaderFolder + L"PBR_UnTextured_vs.cso", defaultLayout3D, defaultNumElements3D))
	{
		ErrorLogger::Log("Failed to initialize textured vertex PBR shader");
	}

	if (!m_pixelShader.Initialize(m_pDevice, m_shaderFolder + L"PBR_UnTextured_ps.cso"))
	{
		ErrorLogger::Log("Failed to initialize pixel shader for textured material");
	}

	HRESULT hr = cb_vs_vertexShader.Initialize(m_pDevice.Get(), m_pDeviceContext.Get());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer for vertex shader inside textured material.");

	hr = cb_ps_perObjectColor.Initialize(m_pDevice.Get(), m_pDeviceContext.Get());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer for PerObjectColor inside textured material.");

}
