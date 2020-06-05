#pragma once

#include <Insight/Core.h>

#include "Insight/Rendering/Texture.h"

namespace Insight {

	struct Color
	{
		const Vector3 White = { 1.0f, 1.0f, 1.0f };
		const Vector3 Black = { 0.0f, 0.0f, 0.0f };
		const Vector3 Red = { 1.0f, 0.0f, 0.0f };
		const Vector3 Green = { 0.0f, 1.0f, 0.0f };
		const Vector3 Blue = { 0.0f, 0.0f, 1.0f };
		const Vector3 Invalid = { 1.0f, 0.0f, 1.0f };
		const Vector3 Default = { 6.0f, 0.6f, 0.6f };
	};

	class INSIGHT_API Material
	{
	public:
		Material();
		~Material();
		
		void OnImGuiRender();
		
		void BindResources();
		void AddTexture(Texture& texture) { m_Textures.push_back(texture); }

	private:
		std::vector<Texture> m_Textures;
		float m_Roughness = 0.5f;
		float m_Metalness = 0.5f;
		Vector2 uvOffset;
		Vector2 tiling;
	};

}