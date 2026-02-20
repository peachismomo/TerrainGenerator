#include "MeshLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "OGLWrapper.h"

#include "Utils.h"
#include <Terrain.h>
#include "Perlin.h"

void MeshLoader::LoadMesh(std::string path)
{
	Mesh& mesh = m_meshes[path];

	mesh.vao		= OGLWRAPPER::CreateVAO();
	mesh.pos_vbo	= OGLWRAPPER::CreateVBO();
	mesh.nml_vbo	= OGLWRAPPER::CreateVBO();
	mesh.ebo_vbo	= OGLWRAPPER::CreateVBO();

	Assimp::Importer importer;
	const aiScene* ai_scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

	if (!ai_scene)
	{
		std::cout << "Failed to load model" << std::endl;
		exit(0);
	}

	mesh.m_mesh_entries.resize(ai_scene->mNumMeshes);

	unsigned int base_vertex{};
	unsigned int base_index{};

	for (unsigned int i{}; i < mesh.m_mesh_entries.size(); ++i)
	{
		mesh.m_mesh_entries[i].indices_cnt = ai_scene->mMeshes[i]->mNumFaces * 3;
		mesh.m_mesh_entries[i].base_vertex = base_vertex;
		mesh.m_mesh_entries[i].base_index = base_index;
		mesh.m_mesh_entries[i].mat_index = ai_scene->mMeshes[i]->mMaterialIndex;

		base_vertex += ai_scene->mMeshes[i]->mNumVertices;
		base_index += mesh.m_mesh_entries[i].indices_cnt;
	}

	for (unsigned int i{}; i < mesh.m_mesh_entries.size(); ++i)
	{
		const aiMesh* ai_mesh = ai_scene->mMeshes[i];
		for (unsigned int i{}; i < ai_mesh->mNumVertices; ++i)
		{
			const aiVector3D& pos = ai_mesh->mVertices[i];
			const aiVector3D& nml = ai_mesh->mNormals[i];

			mesh.m_position_buffer.emplace_back(glm::vec3(pos.x, pos.y, pos.z));
			mesh.m_normal_buffer.emplace_back(glm::vec3(nml.x, nml.y, nml.z));
		}

		for (unsigned int i = 0; i < ai_mesh->mNumFaces; i++)
		{
			const aiFace& face = ai_mesh->mFaces[i];
			mesh.m_indices.push_back(face.mIndices[0]);
			mesh.m_indices.push_back(face.mIndices[1]);
			mesh.m_indices.push_back(face.mIndices[2]);
		}
	}

	if (path.find("part") != std::string::npos)
	{
		for (auto& x : mesh.m_position_buffer)
			x /= 10000.f;
	}

	OGLWRAPPER::PopulateBuffer(mesh.pos_vbo, mesh.m_position_buffer, sizeof(glm::vec3), GL_FLOAT, 0, 3);
	OGLWRAPPER::PopulateBuffer(mesh.nml_vbo, mesh.m_normal_buffer, sizeof(glm::vec3), GL_FLOAT, 1, 3);
	OGLWRAPPER::PopulateEBO(mesh.ebo_vbo, mesh.m_indices);

	OGLWRAPPER::BindVAO();
}

void MeshLoader::LoadTerrain(Terrain& terrain, unsigned int seed, unsigned int no_pts, glm::vec3 map_scale, unsigned int perlin_oct, float perlin_persistance, float perlin_freq)
{
	/*PLANE*/
	Mesh& mesh_plane = m_meshes["debug_terrain"];
	Mesh& mesh_poisson_plane = m_meshes["debug_poisson"];

	OGLWRAPPER::DeleteVAO(mesh_plane.vao);
	OGLWRAPPER::DeleteVBO(mesh_plane.pos_vbo);
	OGLWRAPPER::DeleteVBO(mesh_plane.nml_vbo);
	OGLWRAPPER::DeleteVBO(mesh_plane.ebo_vbo);
	OGLWRAPPER::DeleteVBO(mesh_plane.clr_vbo);


	OGLWRAPPER::DeleteVAO(mesh_poisson_plane.vao);
	OGLWRAPPER::DeleteVBO(mesh_poisson_plane.pos_vbo);
	OGLWRAPPER::DeleteVBO(mesh_poisson_plane.nml_vbo);
	OGLWRAPPER::DeleteVBO(mesh_poisson_plane.ebo_vbo);
	OGLWRAPPER::DeleteVBO(mesh_poisson_plane.clr_vbo);

	mesh_plane.vao = OGLWRAPPER::CreateVAO();
	mesh_plane.pos_vbo = OGLWRAPPER::CreateVBO();
	mesh_plane.nml_vbo = OGLWRAPPER::CreateVBO();
	mesh_plane.ebo_vbo = OGLWRAPPER::CreateVBO();
	mesh_plane.clr_vbo = OGLWRAPPER::CreateVBO();

	mesh_plane.m_mesh_entries.resize(1);

	terrain.GeneratePoints(seed, no_pts, map_scale, perlin_oct, perlin_persistance, perlin_freq);

	mesh_plane.m_position_buffer = terrain.GetVtx();
	mesh_plane.m_normal_buffer = terrain.GetNml();

	if (mesh_plane.m_position_buffer.size() % 3 != 0)
		throw std::runtime_error("The number of vertices is not a multiple of 3.");

	for (size_t i = 0; i < mesh_plane.m_position_buffer.size(); i += 3)
	{
		mesh_plane.m_indices.push_back(static_cast<unsigned int>(i));
		mesh_plane.m_indices.push_back(static_cast<unsigned int>(i + 1));
		mesh_plane.m_indices.push_back(static_cast<unsigned int>(i + 1));
		mesh_plane.m_indices.push_back(static_cast<unsigned int>(i + 2));
		mesh_plane.m_indices.push_back(static_cast<unsigned int>(i + 2));
		mesh_plane.m_indices.push_back(static_cast<unsigned int>(i));
	}

	OGLWRAPPER::PopulateBuffer(mesh_plane.pos_vbo, mesh_plane.m_position_buffer, sizeof(glm::vec3), GL_FLOAT, 0, 3);
	OGLWRAPPER::PopulateBuffer(mesh_plane.nml_vbo, mesh_plane.m_normal_buffer, sizeof(glm::vec3), GL_FLOAT, 1, 3);
	OGLWRAPPER::PopulateBuffer(mesh_plane.clr_vbo, terrain.GetClr(), sizeof(glm::vec3), GL_FLOAT, 2, 3);
	OGLWRAPPER::PopulateEBO(mesh_plane.ebo_vbo, mesh_plane.m_indices);

	mesh_poisson_plane.vao = OGLWRAPPER::CreateVAO();
	mesh_poisson_plane.pos_vbo = OGLWRAPPER::CreateVBO();

	mesh_poisson_plane.m_mesh_entries.resize(1);

	mesh_poisson_plane.m_position_buffer = terrain.GetPoisson();
	OGLWRAPPER::PopulateBuffer(mesh_poisson_plane.pos_vbo, mesh_poisson_plane.m_position_buffer, sizeof(glm::vec3), GL_FLOAT, 0, 3);
}

Mesh* MeshLoader::GetMesh(std::string name)
{
	if (m_meshes.find(name) == m_meshes.end())
		return nullptr;
	return &m_meshes.at(name);
}

void MeshLoader::SubdivideIcoSphere(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, int depth, std::vector<glm::vec3>& vertices) {
	if (depth == 0) {
		vertices.push_back(v1);
		vertices.push_back(v2);
		vertices.push_back(v3);
		return;
	}

	glm::vec3 v12 = glm::normalize(v1 + v2);
	glm::vec3 v23 = glm::normalize(v2 + v3);
	glm::vec3 v31 = glm::normalize(v3 + v1);

	SubdivideIcoSphere(v1, v12, v31, depth - 1, vertices);
	SubdivideIcoSphere(v2, v23, v12, depth - 1, vertices);
	SubdivideIcoSphere(v3, v31, v23, depth - 1, vertices);
	SubdivideIcoSphere(v12, v23, v31, depth - 1, vertices);
}

void MeshLoader::LoadDebugMesh()
{
	/*ICOSPHERE*/
	Mesh& sphere_mesh	= m_meshes["debug_icosphere"];
	sphere_mesh.vao		= OGLWRAPPER::CreateVAO();
	sphere_mesh.pos_vbo = OGLWRAPPER::CreateVBO();

	const float X = 0.525731112119133606f;
	const float Z = 0.850650808352039932f;
	const float N = 0.0f;

	const std::vector<glm::vec3> sphere_vtx = 
	{
		{ -X, N, Z }, {  X, N,  Z }, { -X,  N, -Z }, {  X,  N, -Z },
		{  N, Z, X }, {  N, Z, -X }, {  N, -Z,  X }, {  N, -Z, -X },
		{  Z, X, N }, { -Z, X,  N }, {  Z, -X,  N }, { -Z, -X,  N }
	};

	const std::vector<glm::ivec3> sphere_idx = 
	{
		{ 0,	4,	1  },	{ 0,	9,	4  },	{ 9,	5,		4 },	{ 4,	5,	8 },	{ 4, 8, 1  },
		{ 8,	10,	1  },	{ 8,	3,	10 },	{ 5,	3,		8 },	{ 5,	2,	3 },	{ 2, 7, 3  },
		{ 7,	10,	3  },	{ 7,	6,	10 },	{ 7,	11,		6 },	{ 11,	0,	6 },	{ 0, 1, 6  },
		{ 6,	1,	10 },	{ 9,	0,	11 },	{ 9,	11,		2 },	{ 9,	2,	5 },	{ 7, 2, 11 }
	};

	for (const auto& triangle : sphere_idx)
		SubdivideIcoSphere(sphere_vtx[triangle.x], sphere_vtx[triangle.y], sphere_vtx[triangle.z], 2, sphere_mesh.m_position_buffer);

	OGLWRAPPER::PopulateBuffer(sphere_mesh.pos_vbo, sphere_mesh.m_position_buffer, sizeof(glm::vec3), GL_FLOAT, 0, 3);
	OGLWRAPPER::BindVAO();

	/*LINES*/
	Mesh& mesh		= m_meshes["debug_line"];
	mesh.vao		= OGLWRAPPER::CreateVAO();
	mesh.pos_vbo	= OGLWRAPPER::CreateDynamicBuffer<glm::vec3>(2, GL_FLOAT, 0, 3);
	mesh.nml_vbo	= OGLWRAPPER::CreateVBO();

	mesh.m_normal_buffer = { glm::vec3(0.f, 1.f, 0.f), glm::vec3(1.f, 0.f, 0.f) };
	OGLWRAPPER::PopulateBuffer(mesh.nml_vbo, mesh.m_normal_buffer, sizeof(glm::vec3), GL_FLOAT, 1, 3);

	mesh.m_mesh_entries.resize(1);
	OGLWRAPPER::BindVAO();

	/*CUBE FOR AABB*/
	Mesh& mesh_cube = m_meshes["debug_cube"];
	mesh_cube.vao = OGLWRAPPER::CreateVAO();
	mesh_cube.pos_vbo = OGLWRAPPER::CreateVBO();
	mesh_cube.ebo_vbo = OGLWRAPPER::CreateVBO();
	mesh_cube.m_mesh_entries.resize(1);
	mesh_cube.m_position_buffer = 
	{
		glm::vec3(-1.f, -1.f, -1.f),
		glm::vec3( 1.f, -1.f, -1.f),
		glm::vec3( 1.f,  1.f, -1.f),
		glm::vec3(-1.f,  1.f, -1.f),
		glm::vec3(-1.f, -1.f,  1.f),
		glm::vec3( 1.f, -1.f,  1.f),
		glm::vec3( 1.f,  1.f,  1.f),
		glm::vec3(-1.f,  1.f,  1.f) 
	};

	mesh_cube.m_indices = {
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 3, 7,
		1, 5, 2, 6,
		3, 7, 2, 6,
		0, 4, 1, 5
	};
	mesh_cube.m_mesh_entries[0].indices_cnt = static_cast<unsigned int>(mesh_cube.m_indices.size());
	OGLWRAPPER::PopulateBuffer(mesh_cube.pos_vbo, mesh_cube.m_position_buffer, sizeof(glm::vec3), GL_FLOAT, 0, 3);
	OGLWRAPPER::PopulateEBO(mesh_cube.ebo_vbo, mesh_cube.m_indices);

	OGLWRAPPER::BindVAO();

	/*TRIANGLE*/
	Mesh& tri_mesh = m_meshes["debug_triangle"];
	tri_mesh.vao = OGLWRAPPER::CreateVAO();
	tri_mesh.pos_vbo = OGLWRAPPER::CreateDynamicBuffer<glm::vec3>(3, GL_FLOAT, 0, 3);
	tri_mesh.m_mesh_entries.resize(1);
	OGLWRAPPER::BindVAO();

	/*PLANE*/
	Mesh& mesh_plane	= m_meshes["debug_plane"];
	mesh_plane.vao		= OGLWRAPPER::CreateVAO();
	mesh_plane.pos_vbo	= OGLWRAPPER::CreateVBO();
	mesh_plane.nml_vbo	= OGLWRAPPER::CreateVBO();
	mesh_plane.ebo_vbo	= OGLWRAPPER::CreateVBO();
	mesh_plane.clr_vbo	= OGLWRAPPER::CreateVBO();

	mesh_plane.m_mesh_entries.resize(1);

	mesh_plane.m_normal_buffer;

	const int grid_size = 100;
	const int num_divisions = 100;
	const float spacing = (float)grid_size / num_divisions;
	const float half_size = grid_size / 2.0f;

	//const siv::PerlinNoise::seed_type seed = 123456u;
	//const siv::PerlinNoise perlin{ seed };

	//for (int i = 0; i <= num_divisions; ++i) 
	//{
	//	float x = -half_size + i * spacing;
	//	for (int j = 0; j <= num_divisions; ++j) 
	//	{
	//		float z = -half_size + j * spacing;
	//		
	//		glm::vec3 position = glm::vec3(x, perlin.octave2D_11Smooth(i, j, 2, 0.5, 50) * 50.f, z);
	//		mesh_plane.m_position_buffer.emplace_back(position);

	//		// Calculate normal
	//		glm::vec3 normal;
	//		if (i > 0 && i < num_divisions && j > 0 && j < num_divisions)
	//		{
	//			glm::vec3 left = glm::vec3(x - spacing, perlin.octave2D_11Smooth(x - spacing, z, 2, 0.5) * 10.f, z);
	//			glm::vec3 right = glm::vec3(x + spacing, perlin.octave2D_11Smooth(x + spacing, z, 2, 0.5) * 10.f, z);
	//			glm::vec3 down = glm::vec3(x, perlin.octave2D_11Smooth(x, z - spacing, 2, 0.5) * 10.f, z - spacing);
	//			glm::vec3 up = glm::vec3(x, perlin.octave2D_11Smooth(x, z + spacing, 2, 0.5) * 10.f, z + spacing);

	//			glm::vec3 tangent1 = right - left;
	//			glm::vec3 tangent2 = up - down;
	//			normal = glm::normalize(glm::cross(tangent1, tangent2));
	//		}
	//		else
	//			normal = glm::vec3(0.0f, 1.0f, 0.0f);

	//		mesh_plane.m_normal_buffer.emplace_back(normal);
	//	}
	//}

	//for (int i = 0; i <= num_divisions; ++i) 
	//{
	//	for (int j = 0; j < num_divisions; ++j) 
	//	{
	//		mesh_plane.m_indices.push_back(i * (num_divisions + 1) + j);
	//		mesh_plane.m_indices.push_back(i * (num_divisions + 1) + j + 1);
	//	}
	//}

	//for (int j = 0; j <= num_divisions; ++j) 
	//{
	//	for (int i = 0; i < num_divisions; ++i) 
	//	{
	//		mesh_plane.m_indices.push_back(i * (num_divisions + 1) + j);
	//		mesh_plane.m_indices.push_back((i + 1) * (num_divisions + 1) + j);
	//	}
	//}

	//mesh_plane.m_mesh_entries[0].indices_cnt = static_cast<unsigned int>(mesh_plane.m_indices.size());

	Terrain terrain;
	terrain.GeneratePoints();
	mesh_plane.m_position_buffer = terrain.GetVtx();
	mesh_plane.m_normal_buffer = terrain.GetNml();

	if (mesh_plane.m_position_buffer.size() % 3 != 0)
	{
		throw std::runtime_error("The number of vertices is not a multiple of 3.");
	}

	for (size_t i = 0; i < mesh_plane.m_position_buffer.size(); i += 3)
	{
		mesh_plane.m_indices.push_back(static_cast<unsigned int>(i));
		mesh_plane.m_indices.push_back(static_cast<unsigned int>(i + 1));
		mesh_plane.m_indices.push_back(static_cast<unsigned int>(i + 1));
		mesh_plane.m_indices.push_back(static_cast<unsigned int>(i + 2));
		mesh_plane.m_indices.push_back(static_cast<unsigned int>(i + 2));
		mesh_plane.m_indices.push_back(static_cast<unsigned int>(i));
	}

	OGLWRAPPER::PopulateBuffer(mesh_plane.pos_vbo, mesh_plane.m_position_buffer, sizeof(glm::vec3), GL_FLOAT, 0, 3);
	OGLWRAPPER::PopulateBuffer(mesh_plane.nml_vbo, mesh_plane.m_normal_buffer, sizeof(glm::vec3), GL_FLOAT, 1, 3);
	OGLWRAPPER::PopulateBuffer(mesh_plane.clr_vbo, terrain.GetClr(), sizeof(glm::vec3), GL_FLOAT, 2, 3);
	OGLWRAPPER::PopulateEBO(mesh_plane.ebo_vbo, mesh_plane.m_indices);

	OGLWRAPPER::BindVAO();

	/*AXIS*/
	Mesh& mesh_axis		= m_meshes["debug_axis"];
	mesh_axis.vao		= OGLWRAPPER::CreateVAO();
	mesh_axis.pos_vbo	= OGLWRAPPER::CreateVBO();
	mesh_axis.nml_vbo	= OGLWRAPPER::CreateVBO();

	mesh_axis.m_position_buffer = 
	{
		glm::vec3(-half_size,	0.f,	 0.0f),
		glm::vec3( half_size,	0.f,	 0.0f),
		glm::vec3( 0.0f,		0.f,	-half_size),
		glm::vec3( 0.0f,		0.f,	 half_size) 
	};

	mesh_axis.m_normal_buffer =
	{
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f) 
	};

	OGLWRAPPER::PopulateBuffer(mesh_axis.pos_vbo, mesh_axis.m_position_buffer, sizeof(glm::vec3), GL_FLOAT, 0, 3);
	OGLWRAPPER::PopulateBuffer(mesh_axis.nml_vbo, mesh_axis.m_normal_buffer, sizeof(glm::vec3), GL_FLOAT, 1, 3);
	OGLWRAPPER::BindVAO();

	/*POINT*/
	Mesh& mesh_point	= m_meshes["debug_point"];
	mesh_point.vao		= OGLWRAPPER::CreateVAO();
	mesh_point.pos_vbo	= OGLWRAPPER::CreateVBO();
	mesh_point.m_position_buffer = { glm::vec3(0.f) };
	OGLWRAPPER::PopulateBuffer(mesh_point.pos_vbo, mesh_point.m_position_buffer, sizeof(glm::vec3), GL_FLOAT, 0, 3);
	OGLWRAPPER::BindVAO();


	/*MINIMAP*/
	Mesh& mesh_map		= m_meshes["debug_map"];
	mesh_map.vao		= OGLWRAPPER::CreateVAO();
	mesh_map.pos_vbo	= OGLWRAPPER::CreateVBO();
	mesh_map.nml_vbo	= OGLWRAPPER::CreateVBO();
	mesh_map.ebo_vbo	= OGLWRAPPER::CreateVBO();

	mesh_map.m_2d_position_buffer = 
	{
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.5f, 1.0f),
		glm::vec2(0.5f, 0.5f),
		glm::vec2(1.0f, 0.5f),
	};

	mesh_map.m_2d_tex_buffer =
	{
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f)
	};

	mesh_map.m_indices =
	{
		0, 3, 2,
		2, 1, 0
	};

	OGLWRAPPER::PopulateBuffer(mesh_map.pos_vbo, mesh_map.m_2d_position_buffer, sizeof(glm::vec2), GL_FLOAT, 0, 2);
	OGLWRAPPER::PopulateBuffer(mesh_map.nml_vbo, mesh_map.m_2d_tex_buffer, sizeof(glm::vec2), GL_FLOAT, 1, 2);
	OGLWRAPPER::PopulateEBO(mesh_map.ebo_vbo, mesh_map.m_indices);
	OGLWRAPPER::BindVAO();
}