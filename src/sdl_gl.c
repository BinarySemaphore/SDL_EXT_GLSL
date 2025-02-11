#include "sdl_gl.h"

#include <stdio.h>

// Externs
char SDL_GL_VERSION[150];
float DELTA_TIME;
float CAMERA_MATRIX[16];

Uint64 LAST_TICKS;

int nearestPowerOfTwo(int input) {
	int value = 1;
	while (value < input) value <<= 1;
	return value;
}

void getFrustrumValues(double view_width, double view_height, double fov, double clip_near, double* width, double* height) {
	double aspect;
	double vert_fov_rads;

	// Find vertical fov in rads to match height
	aspect = view_width / view_height;

	vert_fov_rads = fov * 3.1415926537 / 180.0;
	vert_fov_rads = tan(vert_fov_rads * 0.5) / aspect;

	// Calculate height and width
	*height = clip_near * vert_fov_rads;
	*width = *height * aspect;
}

void initializeGL(int view_width, int view_height, double fov, bool orthographic) {
	double inv_aspect;
	double clip_near = 0.001;
	double clip_far = 100000.0;
	double fwidth, fheight;

	// Setup viewport and background
	glViewport(0, 0, view_width, view_height);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);

	// Setup rendering style
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	// Setup perspective
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (orthographic) {
		fov *= 0.5f;
		inv_aspect = (float)view_height / (float)view_width;
		glOrtho(-fov, fov, -fov * inv_aspect, fov * inv_aspect, 0.0f, clip_far);
	}
	else {
		getFrustrumValues(view_width, view_height, fov, clip_near, &fwidth, &fheight);
		glFrustum(-fwidth, fwidth, -fheight, fheight, clip_near, clip_far);
	}

	// Ready for drawing geometry
	glMatrixMode(GL_MODELVIEW);
	glGetFloatv(GL_MODELVIEW_MATRIX, CAMERA_MATRIX);

	LAST_TICKS = SDL_GetTicks64();
}

SDL_Window* createSDLGLWindow(const char* title, int width, int height, double fov, bool orthographic) {
	SDL_Window* window;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Failed to initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	// Create OpenGL window
	window = SDL_CreateWindow(title, 100, 100, width, height, SDL_WINDOW_OPENGL);
	if (!window) {
		printf("Failed to create OpenGL window: %s\n", SDL_GetError());
		SDL_Quit();
		exit(2);
	}

	// OpenGL context for window
	if (!SDL_GL_CreateContext(window)) {
		printf("Failed to create OpenGL context: %s\n", SDL_GetError());
		SDL_Quit();
		exit(2);
	}

	// Initialize OpenGL
	initializeGL(width, height, fov, orthographic);

	// Set GL version for reference
	snprintf(SDL_GL_VERSION, 150, "%s", glGetString(GL_VERSION));

	return window;
}

Texture* loadTextureBMP(const char* filename, bool forceNrSqr) {
	int w, h;
	SDL_Surface* original;
	SDL_Surface* glcompat;
	Texture* texture;

	// Load BMP file onto SDL surface
	original = SDL_LoadBMP(filename);
	if (!original) {
		SDL_SetError("Failed to load BMP \"%s\" | SDL error: %s", filename, SDL_GetError());
		return NULL;
	}

	// Create a OpenGL compatible surface (OpenGL 1.x change res if needed)
	if (forceNrSqr || SDL_GL_VERSION[0] == '1') {
		w = nearestPowerOfTwo(original->w);
		h = nearestPowerOfTwo(original->h);
	} else {
		w = original->w;
		h = original->h;
	}
	glcompat = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
	if (!glcompat) {
		SDL_FreeSurface(original);
		SDL_SetError("Failed to create SDL format surface for BMP \"%s\" | SDL surface error: %s", filename, SDL_GetError());
		return NULL;
	}

	// Copy original into OpenGL compatible
	if (SDL_BlitSurface(original, NULL, glcompat, NULL) != 0) {
		SDL_FreeSurface(glcompat);
		SDL_FreeSurface(original);
		SDL_SetError("Failed to convert/transfer BMP \"%s\" for OpenGL | SDL blit surface error: %s", filename, SDL_GetError());
		return NULL;
	}

	// Setup return Texture
	texture = (Texture*)malloc(sizeof(Texture));
	if (texture == NULL) {
		SDL_FreeSurface(glcompat);
		SDL_FreeSurface(original);
		SDL_SetError("Failed to allocate texture memory for BMP \"%s\"", filename);
		return NULL;
	}

	// Fill and bind texture.data via OpenGL
	glGenTextures(1, &texture->data);
	glBindTexture(GL_TEXTURE_2D, texture->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, glcompat->pixels);

	SDL_FreeSurface(glcompat);
	SDL_FreeSurface(original);
	return texture;
}

void freeTexture(Texture* texture) {
	glDeleteTextures(1, &texture->data);
	free(texture);
}

void drawGLBegin() {
	// Update delta time
	DELTA_TIME = (SDL_GetTicks64() - LAST_TICKS) * 0.001f;
	LAST_TICKS = SDL_GetTicks64();

	// Enable depth buffer writting and clear screen
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Fresh matrix with camera applied
	glLoadIdentity();
	glMultMatrixf(CAMERA_MATRIX);
}

void drawGLEnd(SDL_Window* window) {
	// Swap buffer (double buffer handling)
	SDL_GL_SwapWindow(window);
}

void debugCameraControl(const Uint8* keys, float translation_speed, float rotation_speed) {
	// Sum key state values to check if any are used (zero = none pressed)
	Uint8 modify_camera_matrix = keys[SDL_SCANCODE_W] + keys[SDL_SCANCODE_A] + keys[SDL_SCANCODE_S] +
		keys[SDL_SCANCODE_D] + keys[SDL_SCANCODE_LSHIFT] + keys[SDL_SCANCODE_LCTRL] + keys[SDL_SCANCODE_UP] +
		keys[SDL_SCANCODE_LEFT] + keys[SDL_SCANCODE_DOWN] + keys[SDL_SCANCODE_RIGHT];
	
	if (modify_camera_matrix) {
		// Set speeds based on per second
		translation_speed *= DELTA_TIME;
		rotation_speed *= DELTA_TIME;

		// Fresh matrix
		glPushMatrix();
		glLoadIdentity();

		// Apply transforms to matrix based on key input
		if (keys[SDL_SCANCODE_A]) glTranslatef(translation_speed, 0.0f, 0.0f);
		if (keys[SDL_SCANCODE_D]) glTranslatef(-translation_speed, 0.0f, 0.0f);

		if (keys[SDL_SCANCODE_W]) glTranslatef(0.0f, 0.0f, translation_speed);
		if (keys[SDL_SCANCODE_S]) glTranslatef(0.0f, 0.0f, -translation_speed);

		if (keys[SDL_SCANCODE_LCTRL]) glTranslatef(0.0f, translation_speed, 0.0f);
		if (keys[SDL_SCANCODE_LSHIFT]) glTranslatef(0.0f, -translation_speed, 0.0f);

		if (keys[SDL_SCANCODE_RIGHT]) glRotatef(rotation_speed, 0.0f, 1.0f, 0.0f);
		if (keys[SDL_SCANCODE_LEFT]) glRotatef(-rotation_speed, 0.0f, 1.0f, 0.0f);
		if (keys[SDL_SCANCODE_DOWN]) glRotatef(rotation_speed, 1.0f, 0.0f, 0.0f);
		if (keys[SDL_SCANCODE_UP]) glRotatef(-rotation_speed, 1.0f, 0.0f, 0.0f);

		// Combine with previous camera matrix
		glMultMatrixf(CAMERA_MATRIX);

		// Save matrix to update camera matrix; free matrix
		glGetFloatv(GL_MODELVIEW_MATRIX, CAMERA_MATRIX);
		glPopMatrix();
	}
}

void glResetParameter(GLenum target) {
	GLint param_iv[4];
	GLfloat param_fv[4];

	if (target == GL_TEXTURE_2D) {
		glTexParameteri(target, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT);
		glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
		param_fv[0] = 0.0f;
		param_fv[1] = 0.0f;
		param_fv[2] = 0.0f;
		param_fv[3] = 0.0f;
		glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, param_fv);
		glTexParameteri(target, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glTexParameterf(target, GL_TEXTURE_LOD_BIAS, 0.0f);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MIN_LOD, -1000);
		glTexParameteri(target, GL_TEXTURE_MAX_LOD, 1000);
		glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, 1000);
		param_iv[0] = GL_RED;
		param_iv[1] = GL_GREEN;
		param_iv[2] = GL_BLUE;
		param_iv[3] = GL_ALPHA;
		glTexParameteriv(target, GL_TEXTURE_SWIZZLE_RGBA, param_iv);
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_REPEAT);
	}
}