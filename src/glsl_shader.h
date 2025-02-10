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
	bool ready;
	GLhandleARB program;
	GLhandleARB vert_shader;
	GLhandleARB frag_shader;
	char* name;
	char* vert_source;
	char* frag_source;
} Shader;

#define GLSL_SUCCESS 0x0000
#define GLSL_FAILURE 0x0001
#define GLSL_VERT 0x0100
#define GLSL_FRAG 0x0101

extern bool initShaders();
extern bool compileShaders(Shader* shaders, int shaders_count);
extern void freeShaders(Shader* shaders);
extern void glslShaderDraw(Shader* shader, bool enable);
extern int loadGLSLFile(Shader* target, int type, const char* filename);

#ifdef __cplusplus
}
#endif
