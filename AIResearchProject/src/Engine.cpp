#include "includes.h"
#include "Engine.h"

float delta = 1.f / 60.f;
Engine engine;

void Engine::Init()
{
	if (!m_window.Init())
		exit(0);
	m_editor.Init(m_window.GetWinPtr());
	m_renderer.Init();
}

void Engine::Update()
{
	while (true)
	{
		double start = glfwGetTime();
		m_renderer.Update();
		m_editor.Update();
		m_input.Update();

		m_window.SwapBuffers();
		if (!m_window.IsRunning())
			break;

		double end = glfwGetTime();

		delta = static_cast<float>(end - start);
	}
}

void Engine::End()
{
	m_renderer.End();
	m_window.End();
	m_collision.End();
}