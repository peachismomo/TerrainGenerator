#ifndef WINDOW_H
#define WINDOW_H

#include "includes.h"

class Window
{
public:
	int Init();
	void SwapBuffers();
	void End();
	bool IsRunning() const { return m_running; }
	GLFWwindow* GetWinPtr() { return m_window; }

private:
	int m_height;
	int m_width;

	GLFWwindow* m_window;

	bool m_running;
};

#endif // !WINDOW_H
