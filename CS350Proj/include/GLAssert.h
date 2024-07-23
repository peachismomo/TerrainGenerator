#ifndef GLASSERT_H
#define GLASSERT_H

#include "includes.h"

#ifdef _DEBUG
#define GLASSERT(x) if (!(x)) __debugbreak();
#else
#define GLASSERT(x) if (!(x)) ((void)0)
#endif

#ifdef _DEBUG
#define GL_CALL(x) ClearOGLError();\
    x;\
    GLASSERT(LogOGLCall(#x, __FILE__, __LINE__))

inline void ClearOGLError() { while (glGetError() != GL_NO_ERROR); }
inline bool LogOGLCall(const char* function, const char* file, int line)
{
    while (GLenum err = glGetError())
    {
        std::string err_name = "";
        switch (err)
        {
        case GL_INVALID_ENUM:
            err_name = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            err_name = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            err_name = "GL_INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            err_name = "GL_STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            err_name = "GL_STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            err_name = "GL_OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            err_name = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        case GL_CONTEXT_LOST:
            err_name = "GL_CONTEXT_LOST";
            break;
        default:
            err_name = "UNKNOWN ERROR";
            break;
        }
        std::cout << "[OpenGL Error]: " << err_name << " " << function << " " << file << " " << line << std::endl;
        return false;
    }
    return true;
}
#else
#define GL_CALL(x) x;
#endif

#endif // !GLASSERT_H
