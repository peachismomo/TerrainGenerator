#ifndef INPUT_H
#define INPUT_H

#include "includes.h"

class Input
{
public:
    void        Update();
    bool        IsKeyTriggered(const unsigned int key) const;
    bool        IsKeyDown(const unsigned int key) const;
    bool        IsKeyReleased(const unsigned int key) const;
    glm::vec2   GetCursorPos() const;
    glm::vec2   GetCursorDelta() const;
    int         GetScrollState() const;
    glm::vec2   GetMouseOffset() const;

    static void SetKeyCB(GLFWwindow* window, int key, int scancode, int action, int mod);
    static void SetMouseButtonCB(GLFWwindow* window, int button, int action, int mod);
    static void SetCursorPosCB(GLFWwindow* window, double xoffset, double yoffset);
    static void ScrollCB(GLFWwindow* window, double xoffset, double yoffset);

private:
    static std::unordered_map<unsigned int, int>    m_curr_keystates;
    static std::unordered_map<unsigned int, int>    m_prev_keystates;
    static glm::vec2                                m_cursor_pos;
    static glm::vec2                                m_cursor_delta;
    static int                                      m_scroll_delta;

    static bool     m_first_mouse;
    static float    m_last_x;
    static float    m_last_y;
    static float    m_sensitivity;
    static float    m_off_x;
    static float    m_off_y;
};

#endif // !INPUT_H
