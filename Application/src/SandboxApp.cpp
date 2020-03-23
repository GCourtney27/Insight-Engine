
#include <Insight.h>

class Sandbox : public Insight::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}
};

Insight::Application * Insight::CreateApplication()
{
	return new Sandbox();
}