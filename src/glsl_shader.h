#pragma once
#ifdef __cplusplus
extern "C"{
#endif

#include <stdbool.h>

#include <SDL_opengl.h>

extern bool SDL_GLSL_SUPPORTED;
extern bool SDL_GLSL_READY;
extern char SDL_GLSL_VERSION[10];

typedef struct {
	GLhandleARB program;
	GLhandleARB vert_shader;
	GLhandleARB frag_shader;
	char* name;
	char* vert_source;
	char* frag_source;
} Shader;

extern bool initShaders();
extern bool compileShaders(Shader* shaders, int shaders_count);
extern void freeShaders(Shader* shaders);
extern void glslShaderDraw(Shader* shader, bool enable);
extern Shader loadGLSLFile(const char* filename);

#ifdef __cplusplus
}
#endif
