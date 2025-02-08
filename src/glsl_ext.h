#pragma once
#ifdef __cplusplus
extern "C"{
#endif

#include <stdbool.h>

#include <SDL_opengl.h>

extern PFNGLATTACHOBJECTARBPROC glAttachObject;
extern PFNGLCOMPILESHADERARBPROC glCompileShader;
extern PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObject;
extern PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObject;
extern PFNGLDELETEOBJECTARBPROC glDeleteObject;
extern PFNGLGETINFOLOGARBPROC glGetShaderInfoLog;
extern PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameteriv;
extern PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocation;
extern PFNGLLINKPROGRAMARBPROC glLinkProgram;
extern PFNGLSHADERSOURCEARBPROC glShaderSource;
extern PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObject;
extern PFNGLUNIFORM1IARBPROC   glUniform1i;
extern PFNGLUNIFORM2IARBPROC   glUniform2i;
extern PFNGLUNIFORM3IARBPROC   glUniform3i;
extern PFNGLUNIFORM4IARBPROC   glUniform4i;
extern PFNGLUNIFORM1I64ARBPROC glUniform1i64;
extern PFNGLUNIFORM2I64ARBPROC glUniform2i64;
extern PFNGLUNIFORM3I64ARBPROC glUniform3i64;
extern PFNGLUNIFORM4I64ARBPROC glUniform4i64;
extern PFNGLUNIFORM1FARBPROC   glUniform1f;
extern PFNGLUNIFORM2FARBPROC   glUniform2f;
extern PFNGLUNIFORM3FARBPROC   glUniform3f;
extern PFNGLUNIFORM4FARBPROC   glUniform4f;
extern PFNGLUNIFORM1DPROC      glUniform1d;
extern PFNGLUNIFORM2DPROC      glUniform2d;
extern PFNGLUNIFORM3DPROC      glUniform3d;
extern PFNGLUNIFORM4DPROC      glUniform4d;

extern bool createMissingGlShaderFunctions();

#ifdef __cplusplus
}
#endif
