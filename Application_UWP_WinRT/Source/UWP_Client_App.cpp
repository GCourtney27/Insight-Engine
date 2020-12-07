#include "pch.h"
#include "UWP_Client_App.h"

std::unique_ptr<Insight::Application> Insight::CreateApplication()
{
	return std::make_unique<SandBoxApp::UWPSandboxApp>();
}
