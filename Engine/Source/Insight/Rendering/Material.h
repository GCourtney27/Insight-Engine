#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Texture.h"

namespace Insight {

	class INSIGHT_API Material
	{
	public:
		Material();
		~Material();

		void BindResources();
		void AddTexture(Texture& texture) { m_Textures.push_back(texture); }

	private:
		std::vector<Texture> m_Textures;
	};

}