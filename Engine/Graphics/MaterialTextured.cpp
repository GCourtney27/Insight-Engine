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
	writer.Key("Albedo");
	writer.String(m_textureLocations[0].c_str());
	writer.Key("Normal");
	writer.String(m_textureLocations[1].c_str());
	writer.Key("Metallic");
	writer.String(m_textureLocations[2].c_str());
	writer.Key("Roughness");
	writer.String(m_textureLocations[3].c_str());
}

bool MaterialTextured::InitializeJOSNPiplineAssets(const rapidjson::Value& assetsInformation)
{
	std::string albedo_Filepath;
	std::string normal_Filepath;
	std::string metallic_Filepath;
	std::string roughness_Filepath;
	std::string ao_Filepath;
	std::vector<std::string> textures;

	json::get_string(assetsInformation[1], "Albedo", albedo_Filepath);
	json::get_string(assetsInformation[1], "Normal", normal_Filepath);
	json::get_string(assetsInformation[1], "Metallic", metallic_Filepath);
	json::get_string(assetsInformation[1], "Roughness", roughness_Filepath);
	json::get_string(assetsInformation[1], "AO", ao_Filepath);
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
	m_color.x = 0.0f;
	m_color.y = 0.0f;
	m_color.z = 0.0f;
	cb_ps_perObjectColor.data.color = m_color;
	m_metallic = 0.0f;
	cb_ps_perObjectColor.data.metallic = m_metallic;
	m_roughness = 0.0f;
	cb_ps_perObjectColor.data.roughnss = m_roughness;
	cb_ps_perObjectColor.ApplyChanges();

	return true;
}

bool MaterialTextured::InitializePiplineAssets()
{
	// TODO: Add defaultTestures to tell that they have not been overriden
	return true;
}

void MaterialTextured::InitializeShaders()
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

	if (!m_vertexShader.Initialize(m_pDevice, m_shaderFolder + L"PBR_Textured_vs.cso", defaultLayout3D, defaultNumElements3D))
	{
		ErrorLogger::Log("Failed to initialize textured vertex PBR shader");
	}

	if (!m_pixelShader.Initialize(m_pDevice, m_shaderFolder + L"PBR_Textured_ps.cso"))
	{
		ErrorLogger::Log("Failed to initialize pixel shader for textured material");
	}

	HRESULT hr = cb_vs_vertexShader.Initialize(m_pDevice.Get(), m_pDeviceContext.Get());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer for vertex shader inside textured material.");

	hr = cb_ps_perObjectColor.Initialize(m_pDevice.Get(), m_pDeviceContext.Get());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer for PerObjectColor inside textured material.");
}
