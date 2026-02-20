#include "Input.h"
#include "Engine.h"

std::unordered_map<unsigned int, int>	Input::m_curr_keystates;
std::unordered_map<unsigned int, int>	Input::m_prev_keystates;
glm::vec2								Input::m_cursor_pos;
glm::vec2								Input::m_cursor_delta;
int										Input::m_scroll_delta;
bool									Input::m_first_mouse = true;
float									Input::m_last_x;
float									Input::m_last_y;
float									Input::m_sensitivity = 0.1f;
float									Input::m_off_x;
float									Input::m_off_y;

void Input::Update()
{
	m_prev_keystates = m_curr_keystates;
	m_scroll_delta = 0;
	m_cursor_delta = { 0.f, 0.f };
}

bool Input::IsKeyTriggered(const unsigned int key) const
{
	return m_curr_keystates[key] == GLFW_RELEASE && m_prev_keystates[key] == GLFW_PRESS;
}

bool Input::IsKeyDown(const unsigned int key) const
{
	return m_curr_keystates[key] == GLFW_REPEAT || (m_curr_keystates[key] == GLFW_PRESS && m_prev_keystates[key] == GLFW_PRESS);
}

bool Input::IsKeyReleased(const unsigned int key) const
{
	return m_curr_keystates[key] = GLFW_RELEASE;
}

glm::vec2 Input::GetCursorPos() const
{
	return m_cursor_pos;
}

glm::vec2 Input::GetCursorDelta() const
{
	return m_cursor_delta;
}

int Input::GetScrollState() const
{
	return m_scroll_delta;
}

void Input::SetKeyCB(GLFWwindow* window, int key, int scancode, int action, int mod)
{
	UNREFERENCED_PARAMETER(scancode);
	UNREFERENCED_PARAMETER(window);
	UNREFERENCED_PARAMETER(mod);

	m_curr_keystates[key] = action;
}

void Input::SetMouseButtonCB(GLFWwindow* window, int button, int action, int mod)
{
	UNREFERENCED_PARAMETER(window);
	UNREFERENCED_PARAMETER(mod);

	unsigned char ui_key{};
	bool b_valid_key{ true };

	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		ui_key = VK_LBUTTON;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		ui_key = VK_RBUTTON;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		ui_key = VK_MBUTTON;
		break;
	default:
		b_valid_key = false;
		break;
	}

	if (b_valid_key)
		m_curr_keystates[ui_key] = action;
}

void Input::SetCursorPosCB(GLFWwindow* window, double xoffset, double yoffset)
{
	UNREFERENCED_PARAMETER(window);

	m_cursor_delta = { static_cast<float>(xoffset) - m_cursor_pos.x, static_cast<float>(yoffset) - m_cursor_pos.y };
	m_cursor_pos = { static_cast<float>(xoffset), static_cast<float>(yoffset) };

	if (m_first_mouse)
	{
		m_last_x = static_cast<float>(xoffset);
		m_last_y = static_cast<float>(yoffset);
		m_first_mouse = false;
	}

	float off_x = static_cast<float>(xoffset) - m_last_x;
	float off_y = m_last_y - static_cast<float>(yoffset);
	m_last_x = static_cast<float>(xoffset);
	m_last_y = static_cast<float>(yoffset);

	if (!engine.GetEditor().MouseInGUI() && engine.GetInput().IsKeyDown(VK_LBUTTON))
		engine.GetRenderer().GetCamera().UpdateDirection(off_x, off_y);
}

void Input::ScrollCB(GLFWwindow* window, double xoffset, double yoffset)
{
	UNREFERENCED_PARAMETER(xoffset);
	UNREFERENCED_PARAMETER(window);

	m_scroll_delta = static_cast<int>(yoffset);
}

glm::vec2 Input::GetMouseOffset() const
{
	return glm::vec2(m_off_x, m_off_y);
}