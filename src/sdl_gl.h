#pragma once
#ifdef __cplusplus
extern "C"{
#endif

#include <stdbool.h>

#include <SDL.h>
#include <SDL_opengl.h>
#include <gl/GL.h>

extern char SDL_GL_VERSION[150];
extern float DELTA_TIME;
extern float CAMERA_MATRIX[16];

typedef struct {
	GLuint data;
} Texture;

extern SDL_Window* createSDLGLWindow(const char* title, int width, int height, double fov, bool orthographic);

/**
 * Loads BMP from file for usee with OpenGL texturing
 *
 * \param filname BMP file to load from
 * \param forceNrSqr Forces respective width/height resolution
*                    to nearest square numbers (eg: 900 -> 1024)
 *                   (default: false)
 * \returns Texture structure usable with OpenGL or NULL on failure;
 *          call SDL_GetError() for more information.
 * 
 * \warning User must free returned Texture with freeTexture() before losing scope
 * 
 * \sa SDL_LoadBMP
 * \sa Texture
 * \sa freeTexture
 */
extern Texture* loadTextureBMP(const char* filename, bool forceNrSqr);

/**
 * Free texture memory
 * 
 * \param texture Texture pointer to be freed
 * 
 * \sa loadTextureBMP
 * \sa Texture
 */
extern void freeTexture(Texture* texture);

extern void drawGLBegin();

extern void drawGLEnd(SDL_Window* window);

extern void debugCameraControl(const Uint8* keys, float translation_speed, float rotation_speed);

/**
 * Reset OpenGL target's entire parameter set to OpenGL defaults
 * 
 * \param target OpenGL target parameter category; supported targets:
 *              GL_TEXTURE_2D
 * 
 * \warning Full reset of all respective supported target parameters;
 *          Do not call frequently (only context resets not per-frame operations).
 */
void glResetParameter(GLenum target);

#ifdef __cplusplus
}
#endif
