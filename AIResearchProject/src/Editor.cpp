#include "Editor.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_toggle.h>
#include <Engine.h>
#include <CustomMath.h>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <chrono>

#define ATTRIB_ACTIVE(attribute_name, attribute_type, attribute_var) \
    bool attribute_name##{ ObjAttribEditor(obj, Object::ATTRIB_##attribute_type, #attribute_var) }; \
    if (!attribute_name##) \
        ImGui::BeginDisabled();

#define ATTRIB_ACTIVE_END(attribute_name) \
    if (!attribute_name) \
        ImGui::EndDisabled();


void Editor::Init(GLFWwindow* window_ptr)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	// io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
	// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows

	ImGui_ImplGlfw_InitForOpenGL(window_ptr, true);
	ImGui_ImplOpenGL3_Init("#version 410");

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
	style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
	style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	style.GrabRounding = style.FrameRounding = 2.3f;
}

void Editor::Update()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("User Interface");
	std::string text = "FPS: " + std::to_string(1.f / delta);
	ImGui::Text(text.c_str());
	ImGui::Text("W/A/S/D to move, SPACE/CTRL to move up/down");

	ImGui::SeparatorText("Seed");
	ImGui::InputInt("#Seed", &seed);
	if (seed < 0)
		seed = 0;
	ImGui::SameLine();
	if (ImGui::Button("Random Seed"))
	{
		auto now = std::chrono::high_resolution_clock::now();
		auto duration = now.time_since_epoch();
		seed = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
	}
	ImGui::SeparatorText("Poisson Disk Sampling");
	ImGui::InputInt("Point Count", &no_points);
	if (no_points < 1)
		no_points = 1;
	ImGui::SeparatorText("Map");
	ImGui::InputFloat3("Map Scale", &map_scale.x);
	ImGui::SeparatorText("Perlin");
	ImGui::InputInt("Octave", &perlin_oct, 1, 2);
	if (perlin_oct < 1)
		perlin_oct = 1;
	ImGui::InputFloat("Persistance", &perlin_persistance);
	perlin_persistance = std::clamp(perlin_persistance, 0.f, 1.f);
	ImGui::InputFloat("Resolution", &perlin_freq, 1.f, 10.f);
	if (perlin_freq < 0.f)
		perlin_freq = 0.f;

	if (ImGui::Button("Generate"))
		engine.GetRenderer().GenerateTerrain(seed, no_points, map_scale, perlin_oct, perlin_persistance, perlin_freq);

	ImVec2 window_pos = ImGui::GetWindowPos();
	ImVec2 window_size = ImGui::GetWindowSize();
	ImVec2 mouse_pos = ImGui::GetMousePos();

	m_mouse_in_gui = mouse_pos.x >= window_pos.x &&
		mouse_pos.y >= window_pos.y &&
		mouse_pos.x < (window_pos.x + window_size.x) &&
		mouse_pos.y < (window_pos.y + window_size.y);

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Editor::End()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

bool Editor::ObjAttribEditor(Object* obj, Object::ATTRIBUTES attrib, const char* attrib_name)
{
	bool b_attrib{ obj->IsAttribActive(attrib) };
	if (ImGui::Checkbox(attrib_name, &b_attrib))
	{
		if (b_attrib)
		{
			obj->ClearAttribs();
			obj->SetAttribActive(attrib);
		}
		else
			obj->SetAttribInactive(attrib);
	}

	return b_attrib;
}

void Editor::RecalculateBVH() const
{
	engine.GetRenderer().GetBVHTopDown().ClearBVH(engine.GetRenderer().GetBVHTopDown().GetRoot());
	engine.GetRenderer().GetBVHBotUp().ClearBVH(engine.GetRenderer().GetBVHBotUp().GetRoot());

	engine.GetRenderer().GetBVHTopDown().GetRoot() = nullptr;
	engine.GetRenderer().GetBVHBotUp().GetRoot() = nullptr;

	if (m_top_down)
	{
		if (m_type)
			engine.GetRenderer().GetBVHTopDown().GetRoot() = engine.GetRenderer().GetBVHTopDown().Build(engine.GetRenderer().GetObjects(), T_AABB);
		else
			engine.GetRenderer().GetBVHTopDown().GetRoot() = engine.GetRenderer().GetBVHTopDown().Build(engine.GetRenderer().GetObjects(), T_BS);
	}
	else
	{
		if (m_type)
			engine.GetRenderer().GetBVHBotUp().GetRoot() = engine.GetRenderer().GetBVHBotUp().Build(engine.GetRenderer().GetObjects(), T_AABB);
		else
			engine.GetRenderer().GetBVHBotUp().GetRoot() = engine.GetRenderer().GetBVHBotUp().Build(engine.GetRenderer().GetObjects(), T_BS);
	}
}
