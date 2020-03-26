
#include <Insight.h>

class Sandbox : public Insight::Application
{
public:
	Sandbox()
	{

	}

	virtual ~Sandbox()
	{

	}
};

Insight::Application * Insight::CreateApplication()
{
	return new Sandbox();
}