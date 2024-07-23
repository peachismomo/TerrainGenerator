#include "Renderer.h"
#include "OGLWrapper.h"
#include <glm/ext/matrix_transform.hpp>

#include "Engine.h"

#include "CustomMath.h"

void Renderer::Init()
{
	// m_mesh_loader.LoadMesh("models/bunny_high_poly.obj");
	// m_mesh_loader.LoadMesh("models/cube_high_poly.obj");
	// m_mesh_loader.LoadMesh("models/cube_low_poly.obj");
	// m_mesh_loader.LoadMesh("models/horse_high_poly.obj");
	// m_mesh_loader.LoadMesh("models/menger_sponge_level_1_high_poly.obj");
	// m_mesh_loader.LoadMesh("models/menger_sponge_level_1_low_poly.obj");
	// m_mesh_loader.LoadMesh("models/plane_low_poly.obj");
	// m_mesh_loader.LoadMesh("models/sphere_high_poly.obj");
	// m_mesh_loader.LoadMesh("models/sphere_mid_poly.obj");
	// m_mesh_loader.LoadMesh("models/teapot_mid_poly.obj");
	// 
	// m_mesh_loader.LoadMesh("models/part_a/g0.obj");
	// m_mesh_loader.LoadMesh("models/part_a/g1.obj");
	// m_mesh_loader.LoadMesh("models/part_a/g2.obj");
	// m_mesh_loader.LoadMesh("models/part_b/g0.obj");
	// m_mesh_loader.LoadMesh("models/part_b/g1.obj");

	std::string vtx_shader
	{
		R"(
			#version 450 core
		
			layout (location = 0) in vec3 aPos;
			layout (location = 1) in vec3 aNml;
		
			uniform mat4 u_mdl;
			uniform mat4 u_view;
			uniform mat4 u_projection;

			layout (location = 0) out vec3 vNml;
			layout (location = 1) out vec3 vFragPos;

			void main()
			{
				mat4 u_mdl_view = u_view * u_mdl;
				gl_Position = u_projection * u_mdl_view * vec4(aPos, 1.0);
				vFragPos = vec3(u_mdl * vec4(aPos, 1.0));
				vNml = mat3(transpose(inverse(u_mdl))) * aNml;
			}
		)"
	};

	std::string line_vtx_shader
	{
		R"(
			#version 450 core
			
			layout (location = 0) in vec3 aPos;
			layout (location = 1) in vec3 aClr;
			
			uniform mat4 u_view;
			uniform mat4 u_projection;

			layout (location = 0) out flat vec3 vClr;

			void main()
			{
				gl_Position = u_projection * u_view * vec4(aPos, 1.0);
				vClr = aClr;
			}
		)"
	};

	std::string frag_shader
	{
		R"(
			#version 450 core
		
			layout (location = 0) in vec3 vNml;
			layout (location = 1) in vec3 vFragPos;

			layout (location = 0) out vec4 fFragColor;			
			
			uniform vec3 u_debug_clr;
			uniform int u_debug_flag = 0;
			uniform vec3 u_cam_pos;
		
			void main()
			{
				vec3 lightDir = normalize(u_cam_pos - vFragPos);
				float diff = max(dot(normalize(vNml), lightDir), 0.0);
				vec3 diffuse = diff * vec3(1.0, 1.0, 1.0); // Assuming white light for simplicity

				if (u_debug_flag == 0)
					fFragColor = vec4(diffuse, 1.0);
				else
					fFragColor = vec4(u_debug_clr, 1.0);
			}
		)"
	};


	std::string line_frag_shader
	{
		R"(
			#version 450 core
			
			layout (location = 0) in vec3 vClr;
			
			layout (location = 0) out vec4 fFragColor;		
	
			uniform vec3 u_debug_clr;
			uniform int u_debug_flag = 0;

			void main()
			{
				if (u_debug_flag == 0)
					fFragColor = vec4(vClr, 1.0);
				else
					fFragColor = vec4(u_debug_clr, 1.0);
			}
		)"
	};


	std::string map_vtx_shader
	{
		R"(
			#version 450 core
			layout (location = 0) in vec2 aPos;
			layout (location = 1) in vec2 aTex;
			layout (location = 0) out vec2 vTex;
			void main() {
				gl_Position = vec4(aPos, 0.0, 1.0);
				vTex = aTex;
			}
		)"
	};

	std::string map_frag_shader
	{
		R"(
			#version 450 core
			layout (location = 0) in vec2 vTex;
			layout (location = 0) out vec4 fFragColor;
			uniform sampler2D u_tex;
			void main() {
				fFragColor = texture(u_tex, vTex);
			}
		)"
	};

	m_shdr_id		= OGLWRAPPER::CreateShaderPGM(frag_shader, vtx_shader);
	m_line_shdr_id	= OGLWRAPPER::CreateShaderPGM(line_frag_shader, line_vtx_shader);
	m_map_shdr_id	= OGLWRAPPER::CreateShaderPGM(map_frag_shader, map_vtx_shader);

	m_mesh_loader.LoadDebugMesh();

	m_camera.m_position = glm::vec3(2.809f, 6.976f, 7.705f);
	m_camera.m_dir		= glm::vec3(-0.363f, -0.377f, -0.852f);
	m_camera.m_up		= glm::vec3(-0.148f, 0.926f, -0.347f);
	m_camera.m_world_up = glm::vec3(0.f, 1.f, 0.f);

	m_camera.m_fov	= glm::radians(45.f);
	m_camera.m_ar	= 16.f / 9.f;
	m_camera.m_near = 0.1f;
	m_camera.m_far	= 1000.f;

	m_camera.m_yaw		= -108.159607f;
	m_camera.m_pitch	= -16.4400616f;

	m_camera.UpdateDirection(0.f, 0.f);

	m_camera.CalculateProj();
	m_camera.CalculateView(true);

	Object* new_obj = new Object();
	new_obj->SetMesh("models/part_a/g0.obj");
	m_objects.emplace_back(new_obj);

	new_obj = new Object();
	new_obj->SetMesh("models/part_a/g1.obj");
	m_objects.emplace_back(new_obj);

	new_obj = new Object();
	new_obj->SetMesh("models/part_a/g2.obj");
	m_objects.emplace_back(new_obj);

	new_obj = new Object();
	new_obj->SetMesh("models/part_b/g0.obj");
	m_objects.emplace_back(new_obj);

	new_obj = new Object();
	new_obj->SetMesh("models/part_b/g1.obj");
	m_objects.emplace_back(new_obj);

	OGLWRAPPER::SetClearClr(glm::vec3(.25f));
	OGLWRAPPER::EnableDepthTest();

	OGLWRAPPER::CreateFBO(m_map_framebuffer, m_map_texture, m_map_depth);
	OGLWRAPPER::BindFBO(m_map_framebuffer);
	OGLWRAPPER::EnableDepthTest();
	OGLWRAPPER::BindFBO();

	m_map_camera.m_position = glm::vec3(0.f, 15.f, 0.f);
	m_map_camera.m_dir		= glm::vec3(0.f, 0.f, 0.001f);
	m_map_camera.m_up		= glm::vec3(0.f, 0.f, -1.f);

	m_map_camera.m_fov	= glm::radians(45.f);
	m_map_camera.m_ar	= 1600.f / 900.f;
	m_map_camera.m_near	= 0.1f;
	m_map_camera.m_far	= 100.f;

	m_map_camera.CalculateProj();
	m_map_camera.CalculateView(true);

	for (auto& obj : m_objects)
		obj->Transform();
}

void Renderer::Update()
{
	/*PROCESS CAMERA*/
	m_camera.ProcessKeyboard();
	m_camera.CalculateView();

	OGLWRAPPER::BindFBO(m_map_framebuffer);
	OGLWRAPPER::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	OGLWRAPPER::SetViewport(0, 0, 1600, 900);
	RenderScene(m_map_camera, true);

	OGLWRAPPER::BindFBO();
	OGLWRAPPER::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	OGLWRAPPER::SetViewport(0, 0, 1600, 900);
	RenderScene(m_camera);

	OGLWRAPPER::BindVAO(m_mesh_loader.GetMesh("debug_map")->vao);
	OGLWRAPPER::UseShader(m_map_shdr_id);

	OGLWRAPPER::SetTexUniform(m_map_shdr_id, "u_tex", m_map_texture, 0);

	OGLWRAPPER::DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	OGLWRAPPER::BindVAO();
	OGLWRAPPER::UseShader();
}

void Renderer::End()
{
	OGLWRAPPER::DeleteShader(m_shdr_id);

	m_BVH_topdown.ClearBVH(m_BVH_topdown.GetRoot());
	m_BVH_botup.ClearBVH(m_BVH_botup.GetRoot());

	for (auto& obj : m_objects)
		delete obj;
}

void Renderer::RenderScene(Camera& camera, bool thicken)
{
	DebugRenderer::RenderDebugPlane(m_mesh_loader.GetMesh("debug_plane"), glm::vec4(0.f, 1.f, 0.f, 0.f), m_shdr_id, camera, glm::vec3(0.5f), true, thicken ? 7.f : 1.f);
	DebugRenderer::RenderDebugAxis(m_mesh_loader.GetMesh("debug_axis"), m_line_shdr_id, camera, 2.f);

	//for (auto& obj : m_objects)
	//	obj->Render(camera, m_shdr_id, m_mesh_loader);
	//
	//if (engine.GetEditor().m_render_bh)
	//{
	//	if (engine.GetEditor().m_top_down && m_BVH_topdown.GetRoot())
	//		RenderBVH(camera, m_BVH_topdown.GetRoot(), engine.GetEditor().m_type ? T_AABB : T_BS);
	//	else if (!engine.GetEditor().m_top_down && m_BVH_botup.GetRoot())
	//		RenderBVH(camera, m_BVH_botup.GetRoot(), engine.GetEditor().m_type ? T_AABB : T_BS);
	//}
}

void Renderer::RenderBVH(Camera& camera, BVHNode* root, BVTYPE type, int depth, bool thicken)
{
	static const std::vector<glm::vec3> colors_of_the_rainbow =
	{
		{ 1.f,		0.f,		0.f },
		{ 1.f,		0.5f,		0.f },
		{ 1.f,		1.f,		0.f },
		{ 0.f,		1.f,		0.f },
		{ 0.f,		1.f,		1.f },
		{ 0.f,		0.f,		1.f },
		{ 0.5f,		0.f,		1.f },
	};
	if (root == nullptr)
		return;

	// Process the current node
	if (type == T_AABB)
	{
		if (AABBNode* aabbNode = dynamic_cast<AABBNode*>(root))
		{
			AABB aabb = reinterpret_cast<AABBNode*>(root)->aabb;
			DebugRenderer::RenderDebugAABB(m_mesh_loader.GetMesh("debug_cube"), aabb.center, aabb.half_extent, m_shdr_id, camera, colors_of_the_rainbow[depth % 7]);
		}
	}
	else
	{
		if (BSNode* aabbNode = dynamic_cast<BSNode*>(root))
		{
			BoundingSphere sphere = reinterpret_cast<BSNode*>(root)->bs;
			DebugRenderer::RenderDebugSphere(m_mesh_loader.GetMesh("debug_icosphere"), sphere.position, Point3D(glm::vec3(0.f)), Point3D(glm::vec3(sphere.radius)), m_shdr_id, camera, colors_of_the_rainbow[depth % 7]);
		}
	}

	// Recursively iterate through child nodes
	RenderBVH(camera, root->left, type, depth + 1, thicken);
	RenderBVH(camera, root->right, type, depth + 1, thicken);
}

void DebugRenderer::RenderDebugSphere(Mesh const* mesh, Point3D const& position, Point3D const& rotation, Point3D const& scale, unsigned int shdr_id, Camera const& camera, glm::vec3 clr)
{
	OGLWRAPPER::BindVAO(mesh->vao);
	OGLWRAPPER::UseShader(shdr_id);

	camera.SetUniforms(shdr_id);

	glm::mat4 scalemtx = glm::scale(glm::mat4(1.f), scale.p);
	glm::mat4 rotateX = glm::rotate(glm::mat4(1.f), rotation.p.x, glm::vec3(1.f, 0.f, 0.f));
	glm::mat4 rotateY = glm::rotate(glm::mat4(1.f), rotation.p.y, glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 rotateZ = glm::rotate(glm::mat4(1.f), rotation.p.z, glm::vec3(0.f, 0.f, 1.f));
	glm::mat4 transmtx = glm::translate(glm::mat4(1.f), position.p);
	glm::mat4 rotmtx = rotateZ * rotateY * rotateX;
	glm::mat4 xform = transmtx * rotmtx * scalemtx;

	OGLWRAPPER::SetMat4Uniform(shdr_id, "u_mdl", xform);

	OGLWRAPPER::SetPolygonMode(GL_LINE);
	OGLWRAPPER::SetFloat3Uniform(shdr_id, "u_debug_clr", clr);
	OGLWRAPPER::SetIntUniform(shdr_id, "u_debug_flag", 1);
	OGLWRAPPER::DrawArrays(GL_TRIANGLES, 0, static_cast<unsigned int>(mesh->m_position_buffer.size()));
	OGLWRAPPER::SetPolygonMode(GL_FILL);

	if (engine.GetEditor().m_render_vertices)
	{
		OGLWRAPPER::SetPointSize(5.f);
		OGLWRAPPER::SetFloat3Uniform(shdr_id, "u_debug_clr", glm::vec3(1.f, 0.f, 1.f));
		OGLWRAPPER::SetIntUniform(shdr_id, "u_debug_flag", 1);
		OGLWRAPPER::DrawArrays(GL_POINTS, 0, static_cast<unsigned int>(mesh->m_position_buffer.size()));
		OGLWRAPPER::SetPointSize(1.f);
	}

	OGLWRAPPER::BindVAO();
	OGLWRAPPER::UseShader();
}

void DebugRenderer::RenderDebugSphere(Mesh const* mesh, Point3D const& position, glm::mat3 const& rotation, Point3D const& scale, unsigned int shdr_id, Camera const& camera, glm::vec3 clr)
{
	OGLWRAPPER::BindVAO(mesh->vao);
	OGLWRAPPER::UseShader(shdr_id);

	camera.SetUniforms(shdr_id);

	glm::mat4 transmtx = glm::translate(glm::mat4(1.f), position.p);
	glm::mat4 rotmtx = glm::mat4(rotation);
	glm::mat4 scalemtx = glm::scale(glm::mat4(1.f), scale.p);
	glm::mat4 xform = transmtx * rotmtx * scalemtx;

	OGLWRAPPER::SetMat4Uniform(shdr_id, "u_mdl", xform);

	OGLWRAPPER::SetPolygonMode(GL_LINE);
	OGLWRAPPER::SetFloat3Uniform(shdr_id, "u_debug_clr", clr);
	OGLWRAPPER::SetIntUniform(shdr_id, "u_debug_flag", 1);
	OGLWRAPPER::DrawArrays(GL_TRIANGLES, 0, static_cast<unsigned int>(mesh->m_position_buffer.size()));
	OGLWRAPPER::SetPolygonMode(GL_FILL);

	if (engine.GetEditor().m_render_vertices)
	{
		OGLWRAPPER::SetPointSize(5.f);
		OGLWRAPPER::SetFloat3Uniform(shdr_id, "u_debug_clr", glm::vec3(1.f, 0.f, 1.f));
		OGLWRAPPER::SetIntUniform(shdr_id, "u_debug_flag", 1);
		OGLWRAPPER::DrawArrays(GL_POINTS, 0, static_cast<unsigned int>(mesh->m_position_buffer.size()));
		OGLWRAPPER::SetPointSize(1.f);
	}

	OGLWRAPPER::BindVAO();
	OGLWRAPPER::UseShader();
}

void DebugRenderer::RenderDebugAABB(Mesh const* mesh, Point3D const& position, Point3D const& scale, unsigned int shdr_id, Camera const& camera, glm::vec3 clr)
{
	OGLWRAPPER::BindVAO(mesh->vao);
	OGLWRAPPER::UseShader(shdr_id);

	camera.SetUniforms(shdr_id);

	glm::mat4 scalemtx = glm::scale(glm::mat4(1.f), scale.p);
	glm::mat4 transmtx = glm::translate(glm::mat4(1.f), position.p);
	glm::mat4 xform = transmtx * scalemtx;

	OGLWRAPPER::SetMat4Uniform(shdr_id, "u_mdl", xform);

	for (auto& entry : mesh->m_mesh_entries)
	{
		RenderWireframeLines(clr, shdr_id, entry);
		RenderPoint(glm::vec3(1.f, 0.f, 1.f), shdr_id, entry);
	}

	OGLWRAPPER::BindVAO();
	OGLWRAPPER::UseShader();
}

void DebugRenderer::RenderDebugPlane(Mesh const* mesh, glm::vec4 normal, unsigned int shdr_id, Camera const& camera, glm::vec3 clr, bool origin, float thickness)
{
	glm::mat4 xform(1.f);
	glm::vec3 target_nml = glm::normalize(glm::vec3(normal));

	if (target_nml == glm::vec3(0.f, 1.f, 0.f))
		origin = true;

	if (!origin)
	{
		glm::vec3 rot_axis = glm::cross(glm::vec3(0.f, 1.f, 0.f), target_nml);
		float angle = acosf(glm::dot(glm::vec3(0.f, 1.f, 0.f), target_nml));

		glm::mat4 rot_mtx = glm::rotate(glm::mat4(1.f), angle, rot_axis);
		glm::mat4 trans_mtx = glm::translate(glm::mat4(1.f), normal.w * target_nml);
		glm::mat4 scale_mtx = glm::scale(glm::mat4(1.f), glm::vec3(0.4f));

		xform = trans_mtx * rot_mtx * scale_mtx;
	}

	OGLWRAPPER::BindVAO(mesh->vao);
	OGLWRAPPER::UseShader(shdr_id);

	camera.SetUniforms(shdr_id);

	OGLWRAPPER::SetFloat3Uniform(shdr_id, "u_debug_clr", clr);
	OGLWRAPPER::SetIntUniform(shdr_id, "u_debug_flag", 1);
	OGLWRAPPER::SetMat4Uniform(shdr_id, "u_mdl", xform);

	OGLWRAPPER::SetLineSize(thickness);
	OGLWRAPPER::SetPointSize(5.f);

	OGLWRAPPER::DrawElements(GL_LINES, static_cast<unsigned int>(mesh->m_indices.size()), GL_UNSIGNED_INT, 0);
	//OGLWRAPPER::DrawArrays(GL_POINTS, 0, static_cast<unsigned int>(mesh->m_position_buffer.size()));
	OGLWRAPPER::SetPointSize(1.f);

	OGLWRAPPER::SetLineSize(1.f);

	OGLWRAPPER::BindVAO();
	OGLWRAPPER::UseShader();
}

void DebugRenderer::RenderDebugRay(Mesh const* mesh, Point3D const& p1, Point3D const& p2, unsigned int shdr_id, Camera const& camera)
{
	OGLWRAPPER::BindVAO(mesh->vao);
	OGLWRAPPER::UseShader(shdr_id);

	camera.SetUniforms(shdr_id);

	std::vector<glm::vec3> line = { p1.p, p2.p };

	OGLWRAPPER::ModifyBuffer(mesh->pos_vbo, line, sizeof(glm::vec3));
	OGLWRAPPER::SetIntUniform(shdr_id, "u_debug_flag", 0);
	OGLWRAPPER::DrawArrays(GL_LINES, 0, 2);

	OGLWRAPPER::SetPointSize(5.f);
	OGLWRAPPER::DrawArrays(GL_POINTS, 0, 2);
	OGLWRAPPER::SetPointSize(1.f);

	OGLWRAPPER::BindVAO();
	OGLWRAPPER::UseShader();
}

void DebugRenderer::RenderDebugAxis(Mesh const* mesh, unsigned int shdr_id, Camera const& camera, float size)
{
	OGLWRAPPER::BindVAO(mesh->vao);
	OGLWRAPPER::UseShader(shdr_id);

	camera.SetUniforms(shdr_id);

	//OGLWRAPPER::SetFloat3Uniform(shdr_id, "u_debug_clr", glm::vec3(0.f, 1.f, 0.f));
	//OGLWRAPPER::SetIntUniform(shdr_id, "u_debug_flag", 1);

	OGLWRAPPER::SetLineSize(size);

	OGLWRAPPER::DrawArrays(GL_LINES, 0, 4);

	OGLWRAPPER::SetLineSize(1.f);

	OGLWRAPPER::BindVAO();
	OGLWRAPPER::UseShader();
}

void DebugRenderer::RenderDebugTriangle(Mesh const* mesh, Point3D const& p1, Point3D const& p2, Point3D const& p3, unsigned int shdr_id, Camera const& camera, glm::vec3 clr)
{
	OGLWRAPPER::BindVAO(mesh->vao);
	OGLWRAPPER::UseShader(shdr_id);

	camera.SetUniforms(shdr_id);

	std::vector<glm::vec3> line = { p1.p, p2.p, p3.p };

	OGLWRAPPER::SetFloat3Uniform(shdr_id, "u_debug_clr", clr);
	OGLWRAPPER::SetIntUniform(shdr_id, "u_debug_flag", 1);
	OGLWRAPPER::ModifyBuffer(mesh->pos_vbo, line, sizeof(glm::vec3));
	OGLWRAPPER::SetPolygonMode(GL_LINE);
	OGLWRAPPER::DrawArrays(GL_TRIANGLES, 0, 3);
	OGLWRAPPER::SetPolygonMode(GL_FILL);

	OGLWRAPPER::SetFloat3Uniform(shdr_id, "u_debug_clr", glm::vec3(1.f, 0.f, 1.f));
	OGLWRAPPER::SetIntUniform(shdr_id, "u_debug_flag", 1);
	OGLWRAPPER::SetPointSize(5.f);
	OGLWRAPPER::DrawArrays(GL_POINTS, 0, 3);
	OGLWRAPPER::SetPointSize(1.f);

	OGLWRAPPER::BindVAO();
	OGLWRAPPER::UseShader();
}

void DebugRenderer::RenderWireframe(glm::vec3 clr, unsigned int shdr_id, Mesh::MeshEntry const& entry)
{
	OGLWRAPPER::SetPolygonMode(GL_LINE);
	OGLWRAPPER::SetFloat3Uniform(shdr_id, "u_debug_clr", clr);
	OGLWRAPPER::SetIntUniform(shdr_id, "u_debug_flag", 1);
	OGLWRAPPER::DrawElementsBaseVertex(GL_TRIANGLES, entry.indices_cnt, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * entry.base_index), entry.base_vertex);
	OGLWRAPPER::SetPolygonMode(GL_FILL);
}

void DebugRenderer::RenderWireframeLines(glm::vec3 clr, unsigned int shdr_id, Mesh::MeshEntry const& entry)
{
	OGLWRAPPER::SetPolygonMode(GL_LINE);
	OGLWRAPPER::SetFloat3Uniform(shdr_id, "u_debug_clr", clr);
	OGLWRAPPER::SetIntUniform(shdr_id, "u_debug_flag", 1);
	OGLWRAPPER::DrawElements(GL_LINES, entry.indices_cnt, GL_UNSIGNED_INT, 0);
	OGLWRAPPER::SetPolygonMode(GL_FILL);
}

void DebugRenderer::RenderPoint(glm::vec3 clr, unsigned int shdr_id, Mesh::MeshEntry const& entry)
{
	OGLWRAPPER::SetPointSize(5.f);
	OGLWRAPPER::SetFloat3Uniform(shdr_id, "u_debug_clr", clr);
	OGLWRAPPER::SetIntUniform(shdr_id, "u_debug_flag", 1);
	OGLWRAPPER::DrawElementsBaseVertex(GL_POINTS, entry.indices_cnt, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * entry.base_index), entry.base_vertex);
	OGLWRAPPER::SetPointSize(1.f);
}

float DebugRenderer::CalculateSphereRadius(Mesh* mesh)
{
	glm::vec3 min = glm::vec3(FLT_MAX);
	glm::vec3 max = glm::vec3(FLT_MIN);

	for (auto& it : mesh->m_position_buffer)
	{
		if (it.x < min.x)
			min.x = it.x;
		if (it.y < min.y)
			min.y = it.y;
		if (it.z < min.z)
			min.z = it.z;

		if (it.x > max.x)
			max.x = it.x;
		if (it.y > max.y)
			max.y = it.y;
		if (it.z > max.z)
			max.z = it.z;
	}
	glm::vec3 diff = max - min;

	float radius = CMAX(diff.x, CMAX(diff.y, diff.z));
	return radius;
}

glm::vec3 DebugRenderer::CalculateAABBDimensions(Mesh* mesh)
{
	glm::vec3 min = glm::vec3(FLT_MAX);
	glm::vec3 max = glm::vec3(FLT_MIN);

	for (auto& it : mesh->m_position_buffer)
	{
		if (it.x < min.x)
			min.x = it.x;
		if (it.y < min.y)
			min.y = it.y;
		if (it.z < min.z)
			min.z = it.z;

		if (it.x > max.x)
			max.x = it.x;
		if (it.y > max.y)
			max.y = it.y;
		if (it.z > max.z)
			max.z = it.z;
	}
	glm::vec3 diff = max - min;
	return diff;
}