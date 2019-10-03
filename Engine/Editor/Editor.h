#pragma once
#include "..\\Input\\Keyboard\\KeyboardClass.h"
#include "..\\Input\\Mouse\\MouseClass.h"
#include "..\\Input\\InputManager.h"

#include "..\\Physics\\Ray.h"
#include "..\\Objects\\Entity.h"

class Editor
{
public:
	Editor() {}

	bool Initialize();
	void Update();
	void Shutdown();

	DirectX::XMFLOAT3 GetMouseDirectionVector();
	bool hit_sphere(const SimpleMath::Vector3& center, float radius, const Ray& r);
	float intersection_distance(const SimpleMath::Vector3& center, float radius, const Ray& r);


	void SetIsEditorEnabled(bool enabled) { m_isEditorEnabled = enabled; }
	bool IsEditorEnabled() { return m_isEditorEnabled; }


private:
	Entity* selectedEntity = nullptr;

	bool m_isEditorEnabled = true;
	MouseClass mouse;
	KeyboardClass keyboard;
};