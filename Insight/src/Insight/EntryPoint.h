#pragma once

#ifdef INSI_PLATFORM_WINDOWS

extern Insight::Application* Insight::CreateApplication();

int main(int argc, char** argv)
{
	auto app = Insight::CreateApplication();
	app->Run();
	delete app;
}

#endif