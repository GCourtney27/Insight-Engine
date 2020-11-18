#include "ClientApp.h"

Insight::Application* Insight::CreateApplication()
{
	return new SandBoxApp::Game();
}
