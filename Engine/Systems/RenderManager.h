#pragma once
#include <list>

class Entity;

class RenderManager
{
public:
	RenderManager() {}
	~RenderManager();

private:
	std::list<Entity*> m_transparentObjects;
	std::list<Entity*> m_opaqueObjects;
	Entity* m_sky;
};