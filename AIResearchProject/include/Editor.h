#ifndef EDITOR_H
#define EDITOR_H

#include "includes.h"

#include "Object.h"

class Editor
{
public:
	void Init(GLFWwindow* window_ptr);
	void Update();
	void End();

	bool MouseInGUI() const { return m_mouse_in_gui; }

	bool m_type				{ false };
	bool m_top_down			{ false };
	bool m_render_bv		{ false };
	bool m_render_bh		{ true };
	bool m_render_vertices	{ true };
#pragma region TOPDOWN
	bool m_use_extents	{ true };
	bool m_use_centers	{ false };
	bool m_k_even_split	{ false };
	int k_split			{ 2 };
#pragma endregion TOPDOWN

#pragma region BOTUP
	bool m_nearest_neighbor	{ true };
	bool m_min_comb_vol		{ false };
	bool m_relative_increase{ false };
#pragma endregion BOTUP

#pragma region EXTRACREDIT
	bool m_ellipse{ false };
	bool m_obb{ false };
#pragma endregion EXTRACREDIT

	void RecalculateBVH() const;

	Object::METHOD m_bs_method{ Object::M_RITTERS };

	int				seed{ 1234 };
	int				no_points{ 10000 };
	glm::vec3		map_scale{ glm::vec3(10.f) };
	int				perlin_oct{ 4 };
	float			perlin_persistance{ 0.5f };
	float			perlin_freq{ 10.f };

private:
	bool ObjAttribEditor(Object* obj, Object::ATTRIBUTES attrib, const char* attrib_name);
	bool m_mouse_in_gui;
};

#endif // !EDITOR_H
