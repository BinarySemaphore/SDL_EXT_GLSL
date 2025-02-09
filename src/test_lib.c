#include <stdlib.h>
#include <stdio.h>

#include <SDL.h>

#include "sdl_gl.h"
#include "glsl_shader.h"
#include "glsl_ext.h"

void drawGLScene(SDL_Window* window, Texture* textures, Shader* shaders);

#define NUM_TEXTURES 1
#define NUM_SHADERS 6

int current_shader = 0;
float angle = 0.0f;

int main(int argc, char** argv) {
    int i;
    float m_coords[4] = { -3, 1.5, -2.5, 2.5 };
    const char TEXTURE_FILENAMES[NUM_TEXTURES][100] = {
        "resources/codezlybasictexturepack1(free)_128_7.bmp"
    };
    const char SHADER_FILENAMES[NUM_SHADERS][100] = {
        "resources/shader_color.sdr",
        "resources/shader_texture.sdr",
        "resources/shader_texcoords.sdr",
        "resources/shader_noise.sdr",
        "resources/shader_noise_mask.sdr",
        "resources/shader_mandelbrot.sdr"
    };
    Shader shaders[NUM_SHADERS];
    Texture* textures[NUM_TEXTURES];
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
    textures[0] = loadTextureBMP(TEXTURE_FILENAMES[0]);
    if (textures[0] == NULL) {
        printf("Unable to load texture: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Load and Compile Shaders
    for (i = 0; i < NUM_SHADERS; i++) {
        shaders[i] = loadGLSLFile(SHADER_FILENAMES[i]);
    }
    initShaders();
    compileShaders(shaders, NUM_SHADERS);
    if (SDL_GLSL_SUPPORTED && SDL_GLSL_READY) printf("Shaders supported and ready: SPACE to cycle\n");
    else printf("[WARN] Shaders are unsupported or not ready\n");
    printf("OpenGL Version: %s\nGLSL Version: %s\n", SDL_GL_VERSION, SDL_GLSL_VERSION);

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
        angle += 45.0f * DELTA_TIME;

        // Update Mandelbrot uniforms if specific shader in use
        if (SDL_GLSL_SUPPORTED && SDL_GLSL_READY && current_shader == 5) {
            glUseProgramObject(shaders[current_shader].program);
            GLint u_mcoords_loc = glGetUniformLocation(shaders[current_shader].program, "u_mcoords");
            if (u_mcoords_loc >= 0) glUniform4f(u_mcoords_loc, m_coords[0], m_coords[1], m_coords[2], m_coords[3]);
            glUseProgramObject(0);
        }

        // Event Handling
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) break;
            // Input Handling (resticted)
            if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) break;
                if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) current_shader = (current_shader + 1) % NUM_SHADERS;
            }
        }

        // Input Handling (fast)
        if (keys != NULL) {
            if (keys[SDL_SCANCODE_COMMA]) {
                m_coords[0] += 1.0f * DELTA_TIME;
                m_coords[1] += 1.0f * DELTA_TIME;
            }
            if (keys[SDL_SCANCODE_PERIOD]) {
                m_coords[0] -= 1.0f * DELTA_TIME;
                m_coords[1] -= 1.0f * DELTA_TIME;
            }
            debugCameraControl(keys, 5.0f, 60.0f);
        }
    }

    // Clean Up
    for (i=0; i < NUM_TEXTURES; i++) {
        free(textures[i]);
    }
    freeShaders(shaders);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void drawTriangle(Texture** textures) {
    glBegin(GL_POLYGON);

    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);

    glEnd();
}

void drawQuad(Texture** textures) {
    // Enable textures, set color combination type, and add texture(s)
    //glEnable(GL_TEXTURE_2D);  // Deprecated (use shader)
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, textures[0]->data);

    glBegin(GL_QUADS);

    // Quads drawn clockwise starting at top left
    glColor3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(textures[0]->coords[0], textures[0]->coords[1]);
    glVertex3f(-1.0f, 1.0f, 0.0f);

    glTexCoord2f(textures[0]->coords[2], textures[0]->coords[1]);
    glVertex3f(1.0f, 1.0f, 0.0f);

    glTexCoord2f(textures[0]->coords[2], textures[0]->coords[3]);
    glVertex3f(1.0f, -1.0f, 0.0f);

    glTexCoord2f(textures[0]->coords[0], textures[0]->coords[3]);
    glVertex3f(-1.0f, -1.0f, 0.0f);

    // Disable textures
    // glDisable(GL_TEXTURE_2D);  // Deprecated (use shader)

    glEnd();
}

void drawGLScene(SDL_Window* window, Texture** textures, Shader* shaders) {
    // Setup scene 4 units in front of viewport/camera
    glTranslatef(0.0f, 0.0f, -4.0f);

    // Draw triangle 1.5 units left
    glPushMatrix();
    glTranslatef(-1.5f, 0.0f, 0.0f);
    drawTriangle(NULL);
    glPopMatrix();

    // Enable transparency blending and shader
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glslShaderDraw(&shaders[current_shader], true);

    // Draw quad 1.5 units right and rotate based on angle
    glPushMatrix();
    glTranslatef(1.5f, 0.0f, 0.0f);
    glRotatef(angle, 1.0f, 0.0f, 0.0f);
    drawQuad(textures);
    glPopMatrix();

    // Disable blending and shader
    glslShaderDraw(NULL, false);
    glDisable(GL_BLEND);
}