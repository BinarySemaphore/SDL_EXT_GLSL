/**
 * Library testing source (not to be built with library)
 * Build executable with self + library source to run a test with test_resources
 * 
 * Controls:
 *  Move: W A S D Shift CTRL
 *  Rotate: Arrow Keys
 *  Select Shader: [ ]
 *  Select Texture: ; '
 *  Toggle Nearest/Linear Texture Mag: L
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
#define NUM_SHADERS 6

int current_shader = 0;
int current_texture = 0;
float angle = 0.0f;

int main(int argc, char** argv) {
    bool smoothTexture = false;
    int i;
    float angle_speed = 0.0f;  // Deg per sec
    float m_coords[4] = { -3, 1.5, -2.5, 2.5 };
    const char TEXTURE_FILENAMES[NUM_TEXTURES][100] = {
        "resources/test_corners_colors_oddres_32bit_alpha.bmp",
        "resources/test_corners_colors_oddres_24bit.bmp",
        "resources/test_corners_colors_oddres_16bit.bmp",
        "resources/test_corners_colors_oddres_16bit_alpha.bmp"
    };
    const char SHADER_FILENAMES[NUM_SHADERS][100] = {
        "resources/shader_texture.sdr",
        "resources/shader_texcoords.sdr",
        "resources/shader_color.sdr",
        "resources/shader_noise.sdr",
        "resources/shader_noise_mask.sdr",
        "resources/shader_mandelbrot.sdr"
    };
    Texture* textures[NUM_TEXTURES];
    Shader shaders[NUM_SHADERS];
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
            SDL_Quit();
            return 1;
        }
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

    // Set OpenGL initial draw types
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    setDrawGLTexturesSmooth(smoothTexture);

    while (true) {
        // Draw GL Scene(s)
        drawGLBegin();
        drawGLScene(window, textures, shaders);
        drawGLEnd(window);

        // Updates
        angle += angle_speed * DELTA_TIME;

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

                // Control Texture Smooth or Blocky
                else if (event.key.keysym.scancode == SDL_SCANCODE_L) smoothTexture = !smoothTexture;

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
                    setDrawGLTexturesSmooth(smoothTexture);
                }
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