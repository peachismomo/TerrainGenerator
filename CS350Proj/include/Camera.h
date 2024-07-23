#ifndef CAMERA_H
#define CAMERA_H

#include "includes.h"

class Camera
{
public:
	void SetUniforms(unsigned int shdr_id) const;
	void CalculateView(bool moving = false);
	void CalculateProj();
	void UpdateDirection(float offsetx, float offsety);
	void ProcessKeyboard();

	glm::mat4 m_view;
	glm::mat4 m_proj;

	glm::vec3 m_position;
	glm::vec3 m_dir;
	glm::vec3 m_right;
	glm::vec3 m_up;
	glm::vec3 m_world_up{ 0.f, 1.f, 0.f };

	float m_fov;
	float m_ar;
	float m_near;
	float m_far;

	float m_yaw{ -89.f };
	float m_pitch{ 1.f };
};

#endif // !CAMERA_H
