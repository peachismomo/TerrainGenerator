#include "OGLWrapper.h"
#include "GLAssert.h"

#include <glm/gtc/type_ptr.hpp>

void OGLWRAPPER::Clear(unsigned int flags)
{
	GL_CALL(glClear(flags));
}

void OGLWRAPPER::SetClearClr(glm::vec3 clr)
{
	GL_CALL(glClearColor(clr.x, clr.y, clr.z, 1.f));
}

void OGLWRAPPER::BindVAO(unsigned int vao)
{
	GL_CALL(glBindVertexArray(vao));
}

void OGLWRAPPER::EnableDepthTest(bool cond)
{
	if (cond)
	{
		GL_CALL(glEnable(GL_DEPTH_TEST));
	}
	else
	{
		GL_CALL(glDisable(GL_DEPTH_TEST));
	}
}

unsigned int OGLWRAPPER::CreateVAO()
{
	unsigned int vao_id{};
	GL_CALL(glCreateVertexArrays(1, &vao_id));
	GL_CALL(glBindVertexArray(vao_id));
	return vao_id;
}

unsigned int OGLWRAPPER::CreateVBO()
{
	unsigned int buffer_id{};
	GL_CALL(glCreateBuffers(1, &buffer_id));
	return buffer_id;
}

void OGLWRAPPER::PopulateEBO(unsigned int ebo_id, std::vector<unsigned int> const& buffer)
{
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * buffer.size(), &buffer[0], GL_STATIC_DRAW));
}

void OGLWRAPPER::SetFloatUniform(unsigned int shdr_id, std::string name, float val)
{
	GL_CALL(int loc = glGetUniformLocation(shdr_id, name.c_str()));
	if (loc >= 0)
	{
		GL_CALL(glUniform1fv(loc, 1, &val));
	}
#ifdef _DEBUG
	else
	{
		std::cout << "Uniform not found: " << name << std::endl;
	}
#endif // _DEBUG
}

void OGLWRAPPER::SetFloat3Uniform(unsigned int shdr_id, std::string name, glm::vec3 const& val)
{
	GL_CALL(int loc = glGetUniformLocation(shdr_id, name.c_str()));
	if (loc >= 0)
	{
		GL_CALL(glUniform3fv(loc, 1, glm::value_ptr(val)));
	}
#ifdef _DEBUG
	else
	{
		std::cout << "Uniform not found: " << name << std::endl;
	}
#endif // _DEBUG
}

void OGLWRAPPER::SetMat4Uniform(unsigned int shdr_id, std::string name, glm::mat4 const& val)
{
	GL_CALL(int loc = glGetUniformLocation(shdr_id, name.c_str()));
	if (loc >= 0)
	{
		GL_CALL(glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val)));
	}
#ifdef _DEBUG
	else
	{
		std::cout << "Uniform not found: " << name << std::endl;
	}
#endif // !_DEBUG
}

void OGLWRAPPER::SetIntUniform(unsigned int shdr_id, std::string name, int val)
{
	GL_CALL(int loc = glGetUniformLocation(shdr_id, name.c_str()));
	if (loc >= 0)
	{
		GL_CALL(glUniform1i(loc, static_cast<int>(val)));
	}
#ifdef _DEBUG
	else
	{
		std::cout << "Uniform not found: " << name << std::endl;
	}
#endif // _DEBUG
}

void OGLWRAPPER::SetTexUniform(unsigned int shdr_id, std::string name, int tex_id, int binding)
{
	GL_CALL(glBindTextureUnit(binding, tex_id));

	GL_CALL(int loc = glGetUniformLocation(shdr_id, name.c_str()));
	if (loc >= 0)
	{
		GL_CALL(glUniform1i(loc, binding));
	}
#ifdef _DEBUG
	else
	{
		std::cout << "Uniform not found: " << name << std::endl;
	}
#endif // _DEBUG
}

void OGLWRAPPER::UseShader(unsigned int shdr_id)
{
	GL_CALL(glUseProgram(shdr_id));
}

unsigned int OGLWRAPPER::CreateShaderPGM(std::string frag_shdr, std::string vtx_shdr)
{
	GL_CALL(unsigned int shdr_id{ glCreateProgram() });

	if (shdr_id == 0)
		std::cout << "Unable to create GLProgram." << std::endl;

	if (!CreateShader(shdr_id, vtx_shdr, GL_VERTEX_SHADER) ||
		!CreateShader(shdr_id, frag_shdr, GL_FRAGMENT_SHADER))
	{
		GL_CALL(glDeleteProgram(shdr_id));
		return 0;
	}

	GL_CALL(glLinkProgram(shdr_id));

	GLint link_result;
	GL_CALL(glGetProgramiv(shdr_id, GL_LINK_STATUS, &link_result));

	if (GL_FALSE == link_result)
	{
		std::string log_string;
		int log_len;

		GL_CALL(glGetProgramiv(shdr_id, GL_INFO_LOG_LENGTH, &log_len));
		if (log_len > 0)
		{
			std::vector<char> log(log_len);
			int written_log_len;
			GL_CALL(glGetProgramInfoLog(shdr_id, log_len, &written_log_len, log.data()));
			log_string += log.data();
		}
		std::cout << log_string.c_str() << std::endl;

		GL_CALL(glDeleteProgram(shdr_id));
		std::cout << "Unable to link GLProgram." << std::endl;

	}

	return shdr_id;
}

bool OGLWRAPPER::CreateShader(unsigned int shdr_id, std::string src, unsigned int shdr_type)
{
	GL_CALL(unsigned int src_id{ glCreateShader(shdr_type) });
	const char* frag_src[]{ src.c_str() };
	int result{};

	GL_CALL(glShaderSource(src_id, 1, frag_src, NULL));
	GL_CALL(glCompileShader(src_id));

	GL_CALL(glGetShaderiv(src_id, GL_COMPILE_STATUS, &result));

	if (false == result)
	{
		std::string log_string = (shdr_type == GL_FRAGMENT_SHADER ? "Fragment" : "Vertex") + std::string(" shader compilation failed\n");
		int log_len;
		GL_CALL(glGetShaderiv(src_id, GL_INFO_LOG_LENGTH, &log_len));
		if (log_len > 0)
		{
			std::vector<char> log(log_len);
			int written_log_len;
			GL_CALL(glGetShaderInfoLog(src_id, log_len, &written_log_len, log.data()));
			log_string += log.data();
		}

		std::cout << log_string.c_str() << std::endl;

		return false;
	}

	GL_CALL(glAttachShader(shdr_id, src_id));

	return true;
}

void OGLWRAPPER::DeleteShader(unsigned int shdr_id)
{
	GL_CALL(glDeleteProgram(shdr_id));
}

void OGLWRAPPER::DrawElementsBaseVertex(unsigned int primitive, unsigned int cnt, unsigned int val_type, void* base_idx, unsigned int base_vertex)
{
	GL_CALL(glDrawElementsBaseVertex(primitive, cnt, val_type, base_idx, base_vertex));
}

void OGLWRAPPER::SetPolygonMode(unsigned int type)
{
	GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, type));
}

void OGLWRAPPER::SetPointSize(float size)
{
	GL_CALL(glPointSize(size));
}

void OGLWRAPPER::SetLineSize(float size)
{
	GL_CALL(glLineWidth(size));
}

void OGLWRAPPER::DrawElements(unsigned int primitive, unsigned int cnt, unsigned int val_type, void* offset)
{
	GL_CALL(glDrawElements(primitive, cnt, val_type, offset));
}

void OGLWRAPPER::DrawArrays(unsigned int primitive, unsigned int offset, unsigned int count)
{
	GL_CALL(glDrawArrays(primitive, offset, count););
}

void OGLWRAPPER::CreateFBO(unsigned int& fbo_id, unsigned int& tex_id, unsigned int& depth_id)
{
	// Create framebuffer
	GL_CALL(glCreateFramebuffers(1, &fbo_id));
	GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, fbo_id));

	// Create and bind texture
	GL_CALL(glCreateTextures(GL_TEXTURE_2D, 1, &tex_id));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, tex_id));
	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1600, 900, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	// Attach texture to framebuffer
	GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_id, 0));

	GL_CALL(glCreateRenderbuffers(1, &depth_id));
	GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, depth_id));
	GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1600, 900));
	GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_id));


#ifdef _DEBUG
	// Check framebuffer completeness
	GL_CALL(unsigned int fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
	if (fbo_status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Unable to create framebuffer!" << std::endl;
#endif

	unsigned int buffers[1]{ GL_COLOR_ATTACHMENT0 };
	GL_CALL(glDrawBuffers(1, buffers));

	GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void OGLWRAPPER::BindFBO(unsigned int fbo_id)
{
	GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, fbo_id));
}

void OGLWRAPPER::SetViewport(int px, int py, int pw, int ph)
{
	GL_CALL(glViewport(px, py, pw, ph));
}

void OGLWRAPPER::DeleteVAO(unsigned int vao)
{
	GL_CALL(GLboolean is_vertex_arr{ glIsVertexArray(vao) });
	if (is_vertex_arr)
		GL_CALL(glDeleteVertexArrays(1, &vao));
}

void OGLWRAPPER::DeleteVBO(unsigned int vbo)
{
	GL_CALL(GLboolean is_buffeer{ glIsBuffer(vbo) });
	if (is_buffeer)
		GL_CALL(glDeleteBuffers(1, &vbo));
}