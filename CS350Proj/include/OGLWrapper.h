#ifndef OGLWRAPPER_H
#define OGLWRAPPER_H

#include "includes.h"
#include "GLAssert.h"

namespace OGLWRAPPER
{
	// GENERAL
	void Clear(unsigned int flags);
	void SetClearClr(glm::vec3 clr);

	void BindVAO(unsigned int vao = 0);
	void EnableDepthTest(bool cond = true);

	// BUFFERS
	unsigned int CreateVAO();
	unsigned int CreateVBO();
	void PopulateEBO(unsigned int ebo_id, std::vector<unsigned int> const& buffer);

	template <typename T>
	void PopulateBuffer(unsigned int id, std::vector<T> const& buffer, size_t size, unsigned int val_type, unsigned int attrib_ptr, unsigned int cnt)
	{
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, id));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, size * buffer.size(), &buffer[0], GL_STATIC_DRAW));
		GL_CALL(glEnableVertexAttribArray(attrib_ptr));
		GL_CALL(glVertexAttribPointer(attrib_ptr, cnt, val_type, GL_FALSE, 0, 0));
	}

	template <typename T>
	unsigned int CreateDynamicBuffer(unsigned int buffer_cnt, unsigned int val_type, unsigned int attrib_ptr, unsigned int cnt)
	{
		unsigned int id{};
		GL_CALL(glCreateBuffers(1, &id));
		GL_CALL(glNamedBufferStorage(id,
			sizeof(T) * buffer_cnt,
			NULL,
			GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT));
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, id));
		GL_CALL(glEnableVertexAttribArray(attrib_ptr));
		GL_CALL(glVertexAttribPointer(attrib_ptr, cnt, val_type, GL_FALSE, 0, 0));
		return id;
	}

	template <typename T>
	void ModifyBuffer(unsigned int id, std::vector<T> const& buffer, size_t size)
	{
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, id));
		GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, buffer.size() * size, buffer.data()));
		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	// SHADERS
	void SetFloatUniform(unsigned int shdr_id, std::string name, float val);
	void SetFloat3Uniform(unsigned int shdr_id, std::string name, glm::vec3 const& val);
	void SetMat4Uniform(unsigned int shdr_id, std::string name, glm::mat4 const& val);
	void SetIntUniform(unsigned int shdr_id, std::string name, int val);
	void SetTexUniform(unsigned int shdr_id, std::string name, int tex_id, int binding);

	void UseShader(unsigned int shdr_id = 0);
	unsigned int CreateShaderPGM(std::string frag_shdr, std::string vtx_shdr);
	bool CreateShader(unsigned int shdr_id, std::string src, unsigned int shdr_type);

	void DeleteShader(unsigned int shdr_id);

	// DRAW
	void DrawElementsBaseVertex(unsigned int primitive, unsigned int cnt, unsigned int val_type, void* base_idx, unsigned int base_vertex);
	void SetPolygonMode(unsigned int type);
	void SetPointSize(float size);
	void SetLineSize(float size);
	void DrawElements(unsigned int primitive, unsigned int cnt, unsigned int val_type, void* offset);
	void DrawArrays(unsigned int primitive, unsigned int offset, unsigned int count);

	// FRAMEBUFFERS
	void CreateFBO(unsigned int& fbo_id, unsigned int& tex_id, unsigned int& depth_id);
	void BindFBO(unsigned int fbo_id = 0);

	// VIEWPORTS
	void SetViewport(int px, int py, int pw, int ph);
}

#endif // !OGLWRAPPER_H
