#ifndef ENGINE_H
#define ENGINE_H

#include "Renderer.h"
#include "Editor.h"
#include "Window.h"
#include "Collision.h"
#include "Input.h"

extern float delta;

class Engine
{
public:
	void Init();
	void Update();
	void End();

	Renderer& GetRenderer() { return m_renderer; }
	Input& GetInput() { return m_input; }
	Editor& GetEditor() { return m_editor; }

private:
	Input		m_input;
	Renderer	m_renderer;
	Editor		m_editor;
	Window		m_window;
	Collision	m_collision;
};

extern Engine engine;

#endif // !ENGINE_H
