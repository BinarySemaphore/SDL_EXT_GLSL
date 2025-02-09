#pragma once
#ifdef __cplusplus
extern "C"{
#endif

#include <stdbool.h>

#include <SDL.h>
#include <SDL_opengl.h>

extern char SDL_GL_VERSION[150];
extern float DELTA_TIME;
extern float CAMERA_MATRIX[16];

typedef struct {
    GLuint data;
    GLfloat coords[4];
} Texture;

extern SDL_Window* createSDLGLWindow(const char* title, int width, int height, double fov, bool orthographic);

/**
 * Loads BMP from file for usee with OpenGL texturing.
 *
 * \param filname BMP file to load from
 * \returns Texture structure usable with OpenGL or NULL on failure;
 *          call SDL_GetError() for more information.
 * 
 * \remark User must free returned Texture when done with it
 * 
 * \see SDL_LoadBMP
 * \see Texture
 */
extern Texture* loadTextureBMP(const char* filename);

extern void drawGLBegin();

extern void drawGLEnd(SDL_Window* window);

extern void debugCameraControl(const Uint8* keys, float translation_speed, float rotation_speed);

#ifdef __cplusplus
}
#endif
