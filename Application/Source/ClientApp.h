#pragma once
#include <Insight.h>

/*=====================================================================

	This class is your game. Include all game logic here it will get 
	called and synced with the engine. If you would like to cage the 
	title of your game, Edit the macro 'Game' should you like to edit
	the macro, amek sure to change the name in all places of in the class.

 ======================================================================*/

#define Game Sandbox

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

class Game : public Insight::Application
{
public:
	Game()
	{
		//PushLayer(new ExampleLayer());
	}

	virtual ~Game()
	{

	}

};

Insight::Application* Insight::CreateApplication()
{
	return new Game();
}
