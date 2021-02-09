#include "Win32_Client_App.h"

std::unique_ptr<Insight::Application> Insight::CreateApplication()
{
	return std::make_unique<SandBoxApp::Win32SandboxApp>();
}
