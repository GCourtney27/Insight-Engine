#include "ClientApp.h"


std::unique_ptr<Insight::Engine> Insight::CreateEngine()
{
	return std::make_unique<SandBoxApp::Win32SandboxApp>();
}
