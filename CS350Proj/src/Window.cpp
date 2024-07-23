#include "Window.h"

#include "includes.h"
#include "Input.h"

int Window::Init()
{
	const char* error_msg{};

	if (!glfwInit())
		return 0;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_width = 1600;
	m_height = 900;

	m_window = glfwCreateWindow(
		m_width,
		m_height,
		"CS350 Assignment 1",
		NULL,
		NULL);

	if (!m_window)
		return 0;

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(0);

	int glad_load{ gladLoadGL() };
	if (!glad_load) 
		return 0;

	/*Callbacks*/
	glfwSetKeyCallback(m_window, Input::SetKeyCB);
	glfwSetMouseButtonCallback(m_window, Input::SetMouseButtonCB);
	glfwSetCursorPosCallback(m_window, Input::SetCursorPosCB);
	glfwSetScrollCallback(m_window, Input::ScrollCB);
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	m_running = true;

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);

	std::cout << "Renderer: " << renderer << std::endl;
	std::cout << "Version: " << version << std::endl;


	return 1;
}

void Window::SwapBuffers()
{
	if (glfwWindowShouldClose(m_window))
		m_running = false;

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void Window::End()
{
	glfwTerminate();
}
