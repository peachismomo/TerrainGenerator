#include "Camera.h"
#include "OGLWrapper.h"
#include "Engine.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

void Camera::SetUniforms(unsigned int shdr_id) const
{
	OGLWRAPPER::SetMat4Uniform(shdr_id, "u_projection", m_proj);
	OGLWRAPPER::SetMat4Uniform(shdr_id, "u_view", m_view);
	OGLWRAPPER::SetFloat3Uniform(shdr_id, "u_cam_pos", m_position);
}

void Camera::CalculateView(bool still)
{
	m_view = glm::lookAt(m_position, (still ? m_dir : m_position + m_dir), m_up);
}

void Camera::CalculateProj()
{
	m_proj = glm::perspective(m_fov, m_ar, m_near, m_far);
}

void Camera::UpdateDirection(float offsetx, float offsety)
{
	m_yaw += offsetx * 0.1f;
	m_pitch += offsety * 0.1f;

	if (m_pitch > 89.f)
		m_pitch = 89.f;
	if (m_pitch < -89.f)
		m_pitch = -89.f;

	glm::vec3 front{};
	front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front.y = sin(glm::radians(m_pitch));
	front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_dir = glm::normalize(front);

	m_right = glm::normalize(glm::cross(m_dir, m_world_up));
	m_up = glm::normalize(glm::cross(m_right, m_dir));

	CalculateView();
}

void Camera::ProcessKeyboard()
{
	if (engine.GetInput().IsKeyDown(GLFW_KEY_W))
		m_position += m_dir * 8.f * delta;
	if (engine.GetInput().IsKeyDown(GLFW_KEY_A))
		m_position -= m_right * 8.f * delta;
	if (engine.GetInput().IsKeyDown(GLFW_KEY_S))
		m_position -= m_dir * 8.f * delta;
	if (engine.GetInput().IsKeyDown(GLFW_KEY_D))
		m_position += m_right * 8.f * delta;
	if (engine.GetInput().IsKeyDown(GLFW_KEY_LEFT_CONTROL))
		m_position.y -= 8.f * delta;
	if (engine.GetInput().IsKeyDown(GLFW_KEY_SPACE))
		m_position.y += 8.f * delta;
}
