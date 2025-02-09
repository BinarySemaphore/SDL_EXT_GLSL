#include "glsl_ext.h"

#include <stdbool.h>

#include <SDL.h>

// Externs
PFNGLATTACHOBJECTARBPROC glAttachObject;
PFNGLCOMPILESHADERARBPROC glCompileShader;
PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObject;
PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObject;
PFNGLDELETEOBJECTARBPROC glDeleteObject;
PFNGLGETINFOLOGARBPROC glGetShaderInfoLog;
PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameteriv;
PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocation;
PFNGLLINKPROGRAMARBPROC glLinkProgram;
PFNGLSHADERSOURCEARBPROC glShaderSource;
PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObject;
PFNGLUNIFORM1IARBPROC   glUniform1i;
PFNGLUNIFORM2IARBPROC   glUniform2i;
PFNGLUNIFORM3IARBPROC   glUniform3i;
PFNGLUNIFORM4IARBPROC   glUniform4i;
PFNGLUNIFORM1I64ARBPROC glUniform1i64;
PFNGLUNIFORM2I64ARBPROC glUniform2i64;
PFNGLUNIFORM3I64ARBPROC glUniform3i64;
PFNGLUNIFORM4I64ARBPROC glUniform4i64;
PFNGLUNIFORM1FARBPROC   glUniform1f;
PFNGLUNIFORM2FARBPROC   glUniform2f;
PFNGLUNIFORM3FARBPROC   glUniform3f;
PFNGLUNIFORM4FARBPROC   glUniform4f;
PFNGLUNIFORM1DPROC      glUniform1d;
PFNGLUNIFORM2DPROC      glUniform2d;
PFNGLUNIFORM3DPROC      glUniform3d;
PFNGLUNIFORM4DPROC      glUniform4d;

bool createMissingGlShaderFunctions() {
    // Build missing GL shader functions (add new ones here) and check if supported
    if (SDL_GL_ExtensionSupported("GL_ARB_shader_objects") &&
        SDL_GL_ExtensionSupported("GL_ARB_shading_language_100") &&
        SDL_GL_ExtensionSupported("GL_ARB_vertex_shader") &&
        SDL_GL_ExtensionSupported("GL_ARB_fragment_shader")) {
        glAttachObject = (PFNGLATTACHOBJECTARBPROC)SDL_GL_GetProcAddress("glAttachObjectARB");
        glCompileShader = (PFNGLCOMPILESHADERARBPROC)SDL_GL_GetProcAddress("glCompileShaderARB");
        glCreateProgramObject = (PFNGLCREATEPROGRAMOBJECTARBPROC)SDL_GL_GetProcAddress("glCreateProgramObjectARB");
        glCreateShaderObject = (PFNGLCREATESHADEROBJECTARBPROC)SDL_GL_GetProcAddress("glCreateShaderObjectARB");
        glDeleteObject = (PFNGLDELETEOBJECTARBPROC)SDL_GL_GetProcAddress("glDeleteObjectARB");
        glGetShaderInfoLog = (PFNGLGETINFOLOGARBPROC)SDL_GL_GetProcAddress("glGetInfoLogARB");
        glGetObjectParameteriv = (PFNGLGETOBJECTPARAMETERIVARBPROC)SDL_GL_GetProcAddress("glGetObjectParameterivARB");
        glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONARBPROC)SDL_GL_GetProcAddress("glGetUniformLocationARB");
        glLinkProgram = (PFNGLLINKPROGRAMARBPROC)SDL_GL_GetProcAddress("glLinkProgramARB");
        glShaderSource = (PFNGLSHADERSOURCEARBPROC)SDL_GL_GetProcAddress("glShaderSourceARB");
        glUseProgramObject = (PFNGLUSEPROGRAMOBJECTARBPROC)SDL_GL_GetProcAddress("glUseProgramObjectARB");
        glUniform1i   = (PFNGLUNIFORM1IARBPROC)SDL_GL_GetProcAddress("glUniform1iARB");
        glUniform2i   = (PFNGLUNIFORM2IARBPROC)SDL_GL_GetProcAddress("glUniform2iARB");
        glUniform3i   = (PFNGLUNIFORM3IARBPROC)SDL_GL_GetProcAddress("glUniform3iARB");
        glUniform4i   = (PFNGLUNIFORM4IARBPROC)SDL_GL_GetProcAddress("glUniform4iARB");
        glUniform1i64 = (PFNGLUNIFORM1I64ARBPROC)SDL_GL_GetProcAddress("glUniform1i64ARB");
        glUniform2i64 = (PFNGLUNIFORM2I64ARBPROC)SDL_GL_GetProcAddress("glUniform2i64ARB");
        glUniform3i64 = (PFNGLUNIFORM3I64ARBPROC)SDL_GL_GetProcAddress("glUniform3i64ARB");
        glUniform4i64 = (PFNGLUNIFORM4I64ARBPROC)SDL_GL_GetProcAddress("glUniform4i64ARB");
        glUniform1f   = (PFNGLUNIFORM1FARBPROC)SDL_GL_GetProcAddress("glUniform1fARB");
        glUniform2f   = (PFNGLUNIFORM2FARBPROC)SDL_GL_GetProcAddress("glUniform2fARB");
        glUniform3f   = (PFNGLUNIFORM3FARBPROC)SDL_GL_GetProcAddress("glUniform3fARB");
        glUniform4f   = (PFNGLUNIFORM4FARBPROC)SDL_GL_GetProcAddress("glUniform4fARB");
        glUniform1d  = (PFNGLUNIFORM1DPROC)SDL_GL_GetProcAddress("glUniform1d");
        glUniform2d  = (PFNGLUNIFORM2DPROC)SDL_GL_GetProcAddress("glUniform2d");
        glUniform3d  = (PFNGLUNIFORM3DPROC)SDL_GL_GetProcAddress("glUniform3d");
        glUniform4d  = (PFNGLUNIFORM4DPROC)SDL_GL_GetProcAddress("glUniform4d");
        if (glAttachObject &&
            glCompileShader &&
            glCreateProgramObject &&
            glCreateShaderObject &&
            glDeleteObject &&
            glGetShaderInfoLog &&
            glGetObjectParameteriv &&
            glGetUniformLocation &&
            glLinkProgram &&
            glShaderSource &&
            glUseProgramObject &&
            glUniform1i &&
            glUniform2i &&
            glUniform3i &&
            glUniform4i &&
            glUniform1i64 &&
            glUniform2i64 &&
            glUniform3i64 &&
            glUniform4i64 &&
            glUniform1f &&
            glUniform2f &&
            glUniform3f &&
            glUniform4f &&
            glUniform1d &&
            glUniform2d &&
            glUniform3d &&
            glUniform4d) {
            return true;
        }
    }

    return false;
}