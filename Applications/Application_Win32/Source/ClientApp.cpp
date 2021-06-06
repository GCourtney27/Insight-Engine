#include "ClientApp.h"


std::unique_ptr<Insight::Engine> Insight::CreateApplication()
{
	return std::make_unique<SandBoxApp::Win32SandboxApp>();
}
