#pragma once

#include <Insight/Core.h>

#include "Insight/Runtime/AActor.h"
#include "Insight/Rendering/Texture.h"

namespace Insight {

	using Microsoft::WRL::ComPtr;

	class Sphere
	{
	public:
		void Init(float radius, int slices, int segments);
		void resourceSetup();
		void Render(ComPtr<ID3D12GraphicsCommandList> commandList);

	private:
		ComPtr<ID3D12Resource> m_VertexBuffer;
		ComPtr<ID3D12Resource> m_DefaultBuffer;
		ComPtr<ID3D12Resource> m_IndexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_VertexView;
		D3D12_INDEX_BUFFER_VIEW m_IndexView;
		int m_Slices;
		int m_Segments;
		int m_TriangleSize;
		int m_IndexSize;
		float m_Radius;
		struct SimpleVertex
		{
			DirectX::XMFLOAT4 position;
		};
	};

	class INSIGHT_API ASkySphere : public AActor
	{
	public:
		ASkySphere(ActorId id, ActorType type = "Sky Sphere Actor");
		virtual ~ASkySphere();

		virtual bool LoadFromJson(const rapidjson::Value& jsonSkySphere) override;
		bool WriteToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& Writer) override;
		
		virtual bool OnInit();
		virtual bool OnPostInit();
		virtual void OnUpdate(const float& deltaMs);
		virtual void OnPreRender(XMMATRIX parentMat);
		virtual void OnRender();
		virtual void Destroy();

		void RenderSky(ComPtr<ID3D12GraphicsCommandList> commandList);


		void OnEvent(Event& e);

		virtual void BeginPlay();
		virtual void Tick(const float& deltaMs);
		virtual void Exit();

		virtual void OnImGuiRender() override;

		/*CB_PS_SpotLight GetConstantBuffer() { return m_ShaderCB; }*/

	private:
		Sphere m_Sphere;
		Texture m_Diffuse;

	};


}
