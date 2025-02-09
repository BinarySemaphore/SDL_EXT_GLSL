#include "sdl_gl.h"

#include <stdio.h>

// Externs
char SDL_GL_VERSION[150];
float DELTA_TIME;
float CAMERA_MATRIX[16];

Uint64 LAST_TICKS;

int powerOfTwo(int input) {
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

Texture* loadTextureBMP(const char* filename) {
    int w, h;
    SDL_Surface* original;
    SDL_Surface* glcompat;
    SDL_Rect orig_area, comp_area;
    Texture* texture;

    // Load BMP file onto SDL surface
    original = SDL_LoadBMP(filename);
    if (!original) return NULL;

    // Create a OpenGL compatible surface to take loaded surface
    w = powerOfTwo(original->w);
    h = powerOfTwo(original->h);
    glcompat = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGB24);
    if (!glcompat) {
        SDL_FreeSurface(original);
        return NULL;
    }

    // Copy original into OpenGL compatible
    orig_area.x = 0;
    orig_area.y = 0;
    orig_area.w = original->w;
    orig_area.h = original->h;
    comp_area.x = 0;
    comp_area.y = 0;
    comp_area.w = w;
    comp_area.h = h;
    SDL_BlitSurface(original, &orig_area, glcompat, &comp_area);

    // Setup return Texture
    texture = (Texture*)malloc(sizeof(Texture));
    if (texture == NULL) {
        SDL_FreeSurface(glcompat);
        SDL_FreeSurface(original);
        SDL_SetError("Failed to allocate memory for texture container");
        return NULL;
    }
    texture->coords[0] = 0.0f;
    texture->coords[1] = 0.0f;
    texture->coords[2] = 1.0f; //(float)surface->w / w;
    texture->coords[3] = 1.0f; //(float)surface->h / h;

    // Fill and bind texture.data via OpenGL
    glGenTextures(1, &texture->data);
    glBindTexture(GL_TEXTURE_2D, texture->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Alt: GL_LINEAR - slower but smoother textures
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, glcompat->pixels);

    SDL_FreeSurface(glcompat);
    SDL_FreeSurface(original);
    return texture;
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