#include "glsl_shader.h"

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include <SDL.h>
#include <SDL_opengl_glext.h>

#include "glsl_ext.h" // Uncomment if gl<shader> functions missing from SDL_opengl* (also check initShaders() below)

#define MAX_LINE_SIZE (500)
#define MAX_SOURCE_SIZE (100000)
#define MAX_REASON_SIZE (10000)

// Externs
bool SDL_GLSL_SUPPORTED = false;
bool SDL_GLSL_READY = false;
char SDL_GLSL_VERSION[10];

int SHADER_COUNT;

bool checkGLSuccessStatus(GLhandleARB handle, GLenum status_key, char* reason, const size_t buffer_count) {
    int status;

    // Check link success or failure
    glGetObjectParameteriv(handle, status_key, &status);
    if (status == 0) {
        glGetShaderInfoLog(handle, buffer_count, NULL, reason);
        return false;
    }

    return true;
}

bool linkCompiledShadersWithProgram(Shader* shader) {
    char reason[MAX_REASON_SIZE];

    // Attach and link compiled shaders with GL program
    glAttachObject(shader->program, shader->vert_shader);
    glAttachObject(shader->program, shader->frag_shader);
    glLinkProgram(shader->program);

    // Check link success or failure
    if (!checkGLSuccessStatus(shader->program, GL_OBJECT_LINK_STATUS_ARB, reason, MAX_REASON_SIZE)) {
        printf("Failed to link program:\n%s", reason);
        return false;
    }
    
    return true;
}

bool compileShader(GLhandleARB compiled, const GLcharARB* source) {
    char reason[MAX_REASON_SIZE];

    // Compile from source
    glShaderSource(compiled, 1, &source, NULL);
    glCompileShader(compiled);

    // Check compiled success or failure
    if (!checkGLSuccessStatus(compiled, GL_OBJECT_COMPILE_STATUS_ARB, reason, MAX_REASON_SIZE)) {
        printf("Failed to compile shader:\n%s\n%s\n", source, reason);
        return false;
    }

    return true;
}

bool compileShaderProgram(Shader* shader) {
    int location;

    // Clear GL last error to detect any new error prior to final return
    glGetError();

    // Create GL program for shader
    shader->program = glCreateProgramObject();

    // Create and compile vertex shader
    shader->vert_shader = glCreateShaderObject(GL_VERTEX_SHADER_ARB);
    if (!compileShader(shader->vert_shader, shader->vert_source)) return false;

    // Create and compile fragment shader
    shader->frag_shader = glCreateShaderObject(GL_FRAGMENT_SHADER_ARB);
    if (!compileShader(shader->frag_shader, shader->frag_source)) return false;

    // Link compiled shaders with GL program
    if (!linkCompiledShadersWithProgram(shader)) return false;

    // Set texture uniform if tex0 used in shader
    glUseProgramObject(shader->program);
    location = glGetUniformLocation(shader->program, "tex0");
    if (location >= 0) glUniform1i(location, 0);
    glUseProgramObject(0);

    return glGetError() == GL_NO_ERROR;
}

void destroyShaderProgram(Shader* shader)
{
    // Free shaders and GL program if they exist
    if (SDL_GLSL_SUPPORTED && SDL_GLSL_READY) {
        glDeleteObject(shader->vert_shader);
        glDeleteObject(shader->frag_shader);
        glDeleteObject(shader->program);
    }
}

bool initShaders() {
    SDL_GLSL_READY = false;
    SDL_GLSL_SUPPORTED = true;

    // Set GLSL version for reference
    snprintf(SDL_GLSL_VERSION, 10, "%s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Placeholder for init if ever needed
    // Put code here...

    // Uncomment if-block createMissingGlShaderFunctions if gl<shader> functions missing from SDL_opengl*
    if (!createMissingGlShaderFunctions()) SDL_GLSL_SUPPORTED = false;

    return SDL_GLSL_SUPPORTED;
}

bool compileShaders(Shader* shaders, int num_shaders) {
	int i;
    SHADER_COUNT = num_shaders;
    SDL_GLSL_READY = false;

    if (!SDL_GLSL_SUPPORTED) {
        printf("Unable to compile shaders: shaders not supported");
        return SDL_GLSL_READY;
    }

	for (i = 0; i < SHADER_COUNT; i++) {
		if (!compileShaderProgram(&shaders[i])) {
			printf("Unable to compile shader: \"%s\"\n", shaders[i].name);
			return SDL_GLSL_READY;
		}
	}

    SDL_GLSL_READY = true;

    return SDL_GLSL_READY;
}

void freeShaders(Shader* shaders) {
    int i;

    for (i = 0; i < SHADER_COUNT; i++) {
        // Free shader sources
        free(shaders[i].name);
        free(shaders[i].vert_source);
        free(shaders[i].frag_source);

        // Free shaders and GL program
        destroyShaderProgram(&shaders[i]);
    }
}

void glslShaderDraw(Shader* shader, bool enable) {
    if (SDL_GLSL_SUPPORTED && SDL_GLSL_READY) {
        if (enable) glUseProgramObject(shader->program);
        else glUseProgramObject(0);
    }
}

char* readSectionFromFile(FILE* file, const char* start_tag, const char* end_tag) {
	int size;
	char line[MAX_LINE_SIZE];
	char* dest_str;
    void* realloc_ptr;

	// Init destination string
	dest_str = (char*)malloc(sizeof(char) * MAX_SOURCE_SIZE);
	dest_str[0] = '\0';

	// Find start tag
	while (fgets(line, MAX_LINE_SIZE, file) != NULL) {
		if (strcmp(line, start_tag) == 0) break;
	}

	// Get section
	while (fgets(line, MAX_LINE_SIZE, file) != NULL) {
		if (strcmp(line, end_tag) == 0) break;
		strncat(dest_str, line, MAX_SOURCE_SIZE);
	}

	// Cleanup destination string; NULL dest_str should be handled by calling function
	size = strlen(dest_str) + 1;
    realloc_ptr = realloc(dest_str, sizeof(char) * size);
    if (realloc_ptr == NULL) free(dest_str);
    dest_str = (char*)realloc_ptr;

	return dest_str;
}

Shader loadGLSLFile(const char* filename) {
    int filename_len;
	Shader shader;
    FILE* file;
	
	file = fopen(filename, "r");
	if (file == NULL) {
		printf("Failed to load file: \"%s\"\n", filename);
		exit(1);
	}

    filename_len = strlen(filename);
    shader.name = (char*)malloc(sizeof(char) * (filename_len + 1));
    strncpy(shader.name, filename, filename_len + 1);

	shader.vert_source = readSectionFromFile(file, "/* vertex shader start */\n", "/* vertex shader end */\n");
	if (shader.vert_source == NULL) {
		printf("Failed to load vertex source from file \"%s\"\n", filename);
		exit(1);
	}

	shader.frag_source = readSectionFromFile(file, "/* fragment shader start */\n", "/* fragment shader end */\n");
	if (shader.frag_source == NULL) {
		printf("Failed to load fragment source from file \"%s\"\n", filename);
		exit(1);
	}

	return shader;
}
