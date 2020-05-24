#include <ie_pch.h>

#include "Material.h"

namespace Insight {



	Material::Material()
	{
	}

	Material::~Material()
	{
	}

	void Material::BindResources()
	{
		for (unsigned int i = 0; i < m_Textures.size(); i++)
		{
			m_Textures[i].Bind();
		}
	}

}