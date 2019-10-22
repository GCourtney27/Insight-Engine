#include "MaterialTextured.h"

bool MaterialTextured::Initialize(std::vector<std::string> textureLocations)
{
	this->m_textureLocations = textureLocations;

	std::vector<std::string>::iterator iter;
	for (iter = textureLocations.begin(); iter != textureLocations.end(); iter++)
	{
		m_textures.push_back(Texture(this->m_pDevice, (*iter)));
	}
	return true;
}
