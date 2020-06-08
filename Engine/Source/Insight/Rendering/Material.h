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
		
		virtual bool LoadFromJson(const rapidjson::Value& jsonMaterial);


		void OnImGuiRender();
		
		void BindResources();

	private:
		Texture m_AlbedoMap;
		Texture m_NormalMap;
		Texture m_MetallicMap;
		Texture m_RoughnessMap;
		Texture m_AOMap;
		float m_RoughnessOverride = 0.5f;
		float m_MetalnessOverride = 0.5f;
		Vector2 uvOffset;
		Vector2 tiling;
	};

}