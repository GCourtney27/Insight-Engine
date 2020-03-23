#pragma once
#include "Component.h"
#include <DirectXMath.h>
#include <d3d11.h >
#include <SimpleMath.h>

class EditorSelection : public Component
{
public:
	enum eStatus
	{
		DEFUALT,
		OFF
	};

public:
	EditorSelection(Entity* owner)
		: Component(owner) {}
	
	void Initialize(Entity* owner, float radius, const DirectX::XMFLOAT3 & position);

	void Start() override;
	void Update(const float& deltaTime) override;
	void Destroy() override;
	//void Update(const DirectX::XMFLOAT3 parentPosition);
	void OnImGuiRender() override;
	void OnEditorStop() override;
	void InitFromJSON(Entity* owner, const rapidjson::Value& componentInformation) override;
	void WriteToJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;

	void SetRadius(float value) { m_radius = value; }
	void SetPosition(const DirectX::XMFLOAT3 & position) { m_position = position; }

	float GetRadius() const { return m_radius; }
	DirectX::XMFLOAT3 & GetPosition() { return m_position; }

	


private:
	float m_radius = 0.0f;
	DirectX::SimpleMath::Vector3 m_position;
};