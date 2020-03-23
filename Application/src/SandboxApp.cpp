
#include "Insight.h"

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

int main()
{
	Sandbox * pSandbox = new Sandbox();
	pSandbox->Run();
	delete pSandbox;
}