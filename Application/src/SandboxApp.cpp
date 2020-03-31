
#include <Insight.h>


class ExampleLayer : public Insight::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{

	}

	void OnUpdate() override
	{

	}

	void OnEvent(Insight::Event& event) override
	{
		//IE_INFO("{0}", event);
	}
};

class Sandbox : public Insight::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
		//PushLayer(new Insight::ImGuiLayer());
	}

	virtual ~Sandbox()
	{

	}

};

Insight::Application * Insight::CreateApplication()
{
	return new Sandbox();
}