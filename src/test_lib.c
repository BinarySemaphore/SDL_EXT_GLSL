/**
 * Library testing source (not to be built with library)
 * Build executable with self + library source to run a test with test_resources
 * 
 * Controls: See README.md
 */

#include <stdlib.h>
#include <stdio.h>

#include <SDL.h>

#include "sdl_gl.h"
#include "glsl_shader.h"
#include "glsl_ext.h"

void drawGLScene(SDL_Window* window, Texture** textures, Shader* shaders);
void setDrawGLTexturesSmooth(bool smooth);

#define NUM_TEXTURES 4
#define NUM_SHADERS 4

int current_shader = 0;
int current_texture = 0;
float angle = 0.0f;

int main(int argc, char** argv) {
	bool smooth_texture = false;
	bool spin_enabled = false;
	int i;
	float shader_time = 0.0f;
	float angle_speed = 45.0f;  // Deg per sec
	const char TEXTURE_FILENAMES[NUM_TEXTURES][100] = {
		"resources/test_corners_colors_oddres_32bit_alpha.bmp",
		"resources/test_corners_colors_oddres_24bit.bmp",
		"resources/test_corners_colors_oddres_16bit.bmp",
		"resources/test_corners_colors_oddres_16bit_alpha.bmp"
	};
	const char SHADER_VERT_FILENAME[100] = "resources/common_color_textcoord.vert";
	const char SHADER_FRAG_FILENAMES[NUM_SHADERS][100] = {
		"resources/shader_texture.frag",
		"resources/shader_color_texcoord_alpha.frag",
		"resources/shader_noise_mask.frag",
		"resources/shader_color.frag"
	};
	Texture* textures[NUM_TEXTURES];
	Shader shaders[NUM_SHADERS];
	GLint uniform_loc;
	Uint8* keys;
	SDL_Window* window;
	SDL_Event event;

	// Create SDL GL Window
	window = createSDLGLWindow(
		"SDL_GLSL_LIB Test",
		640, 480,
		75.0f, false
	);

	// Load Texture(s)
	for (i = 0; i < NUM_TEXTURES; i++) {
		textures[i] = loadTextureBMP(TEXTURE_FILENAMES[i], false);
		if (textures[i] == NULL) {
			printf("Unable to load texture: %s\n", SDL_GetError());
			SDL_DestroyWindow(window);
			SDL_Quit();
			return 1;
		}
	}

	// Setup Shaders
	initShaders();
	if (!SDL_GLSL_SUPPORTED) {
		printf("[WARN] Shaders are unsupported on this system\n");
	} else {
		// Load and Compile Shaders
		for (i = 0; i < NUM_SHADERS; i++) {
			if (loadGLSLFile(&shaders[i], GLSL_VERT, SHADER_VERT_FILENAME) != GLSL_SUCCESS) {
				printf("Unable to load shader vert: %s\n", SDL_GetError());
			}
			if (loadGLSLFile(&shaders[i], GLSL_FRAG, SHADER_FRAG_FILENAMES[i]) != GLSL_SUCCESS) {
				printf("Unable to load shader frag: %s\n", SDL_GetError());
			}
		}
		compileShaders(shaders, NUM_SHADERS);
		if (SDL_GLSL_SUPPORTED && SDL_GLSL_READY) printf("Shaders supported and ready\n");
		else printf("[WARN] Shaders are unsupported or not ready\n");
	}
	printf("OpenGL Version: %s\nGLSL Version: %s\n", SDL_GL_VERSION, SDL_GLSL_VERSION);

	// Set OpenGL initial draw types
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	setDrawGLTexturesSmooth(smooth_texture);

	// Get Keys for later input handling
	keys = (Uint8*)SDL_GetKeyboardState(NULL);
	if (keys == NULL) {
		printf("Failed to get keyboard bindings.\n");
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	while (true) {
		// Draw GL Scene(s)
		drawGLBegin();
		drawGLScene(window, textures, shaders);
		drawGLEnd(window);

		// Updates
		if (spin_enabled) angle += angle_speed * DELTA_TIME;

		// Update Mandelbrot uniforms if specific shader in use
		if (SDL_GLSL_SUPPORTED && SDL_GLSL_READY && current_shader == 2) {
			shader_time += DELTA_TIME;
			glUseProgramObject(shaders[current_shader].program);
			uniform_loc = glGetUniformLocation(shaders[current_shader].program, "u_time");
			if (uniform_loc >= 0) glUniform1f(uniform_loc, shader_time);
			glUseProgramObject(0);
		}

		// Event Handling
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) break;
			// Input Handling (resticted)
			if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) break;

				// Control Toggle Texture Smooth or Blocky
				else if (event.key.keysym.scancode == SDL_SCANCODE_L) smooth_texture = !smooth_texture;

				// Control Toggle quad spin
				else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) spin_enabled = !spin_enabled;

				// Control Texture and Shader selections
				else if (event.key.keysym.scancode == SDL_SCANCODE_SEMICOLON) current_texture -= 1;
				else if (event.key.keysym.scancode == SDL_SCANCODE_APOSTROPHE) current_texture += 1;
				else if (event.key.keysym.scancode == SDL_SCANCODE_LEFTBRACKET) current_shader -= 1;
				else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHTBRACKET) current_shader += 1;

				// Keep selection within range by wrapping
				if (current_texture < 0) current_texture = NUM_TEXTURES - 1;
				else if (current_texture >= NUM_TEXTURES) current_texture = NUM_TEXTURES - current_texture;
				if (current_shader < 0) current_shader = NUM_SHADERS - 1;
				else if (current_shader >= NUM_SHADERS) current_shader = NUM_SHADERS - current_shader;

				// Call here to update all textures for both smooth and selection changes
				for (i = 0; i < NUM_TEXTURES; i++) {
					glBindTexture(GL_TEXTURE_2D, textures[i]->data);
					setDrawGLTexturesSmooth(smooth_texture);
				}
			}
		}

		// Input Handling (fast)
		if (keys != NULL) {
			debugCameraControl(keys, 5.0f, 60.0f);
		}
	}

	// Clean Up
	for (i=0; i < NUM_TEXTURES; i++) {
		freeTexture(textures[i]);
	}
	freeShaders(shaders);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

void drawTriangle(Texture** textures) {
	glBegin(GL_POLYGON);

	// Draw clockwise starting from top
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);

	glEnd();
}

void drawQuad(Texture** textures) {
	// Setup Texturing
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, textures[current_texture]->data);

	glBegin(GL_QUADS);

	// Drawn clockwise starting at top left
	glColor3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);

	glEnd();
}

void drawGLScene(SDL_Window* window, Texture** textures, Shader* shaders) {
	// Setup scene 4 units in front of viewport/camera
	glTranslatef(0.0f, 0.0f, -4.0f);

	// Draw triangle
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -1.0f);
	drawTriangle(NULL);
	glPopMatrix();

	// Enable transparency blending and shader
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glslShaderDraw(&shaders[current_shader], true);

	// Draw quad
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(angle, 1.0f, 0.0f, 0.0f);
	drawQuad(textures);
	glPopMatrix();

	// Disable shaders
	glslShaderDraw(NULL, false);
}

void setDrawGLTexturesSmooth(bool smooth) {
	if (smooth) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
}