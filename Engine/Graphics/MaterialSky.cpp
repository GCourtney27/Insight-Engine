#include "MaterialSky.h"

bool MaterialSky::Initiailze(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const rapidjson::Value & assetsInformation, eFlags materialAttributeFlags)
{
	this->m_pDevice = pDevice;
	this->m_pDeviceContext = pDeviceContext;
	this->m_flags = materialAttributeFlags;
	if (!InitializeJOSNPiplineAssets(assetsInformation))
	{
		ErrorLogger::Log("Failed to initialize pipline assets for skybox material.");
		return false;
	}
	
	return true;
}

bool MaterialSky::Initiailze(ID3D11Device * device, ID3D11DeviceContext * deviceContext, eFlags materialAttributeFlags)
{
	return false;
}

void MaterialSky::WriteToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
}

bool MaterialSky::InitializeJOSNPiplineAssets(const rapidjson::Value & assetsInformation)
{
	std::string sky_Map;
	std::string ir_Map;
	std::string Environment_Map;
	std::string brdf_LUT_Map;
	std::vector<std::string> textures;

	json::get_string(assetsInformation[1], "Sky", sky_Map);
	json::get_string(assetsInformation[1], "IR", ir_Map);
	json::get_string(assetsInformation[1], "Environment", Environment_Map);
	json::get_string(assetsInformation[1], "BRDF_LUT", brdf_LUT_Map);

	textures.push_back(sky_Map);
	textures.push_back(ir_Map);
	textures.push_back(Environment_Map);
	textures.push_back(brdf_LUT_Map);
	m_textureLocations = textures;

	std::vector<std::string>::iterator iter;
	for (iter = m_textureLocations.begin(); iter != m_textureLocations.end(); iter++)
	{
		this->m_textures.push_back(Texture(this->m_pDevice.Get(), (*iter)));
	}

	InitializeShaders();

	return true;
}

bool MaterialSky::InitializePiplineAssets()
{
	return true;
}

void MaterialSky::InitializeShaders()
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

	if (!m_vertexShader.Initialize(m_pDevice, m_shaderFolder + L"Sky_vs.cso", defaultLayout3D, defaultNumElements3D))
		ErrorLogger::Log("Failed to initialize Sky vertex shader");

	if (!m_pixelShader.Initialize(m_pDevice, m_shaderFolder + L"Sky_ps.cso"))
		ErrorLogger::Log("Failed to initialize Sky pixel shader");

	HRESULT hr = cb_vs_vertexShader.Initialize(m_pDevice.Get(), m_pDeviceContext.Get());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer for vertex shader inside textured material.");

	hr = cb_ps_perObjectColor.Initialize(m_pDevice.Get(), m_pDeviceContext.Get());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer for PerObjectColor inside textured material.");
}
//{
//	"Type": "Sky",
//		"Components" : [
//	{
//		"MeshRenderer": [
//		{
//			"Model": "..\\Assets\\Objects\\Primatives\\sphere.obj"
//		},
//						{
//							"MaterialType": "PBR_SKY",
//							"Sky" : "..\\Assets\\Textures\\Skyboxes\\skybox1_Diff.dds",
//							"IR" : "..\\Assets\\Textures\\Skyboxes\\skybox1_IR.dds",
//							"Environment" : "..\\Assets\\Textures\\Skyboxes\\skybox1_EnvMap.dds",
//							"BRDF_LUT" : "..\\Assets\\Textures\\Skyboxes\\ibl_brdf_lut.png"
//						}
//		]
//	}
//		]
//},