#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include <./engine/Public.h>
#include <drawingprotocol.h>
#include <string.h>

// Assuming 'cwd' is a global variable populated by cwd_init() from your Public.h
// If it's not global, you would need to adjust how it's accessed.
extern char* cwd;

// Forward declaration for poll_draw to make it visible to main()
void poll_draw(win_t *win, size_t pollcycles);

int main() {
    cwd_init();
    const float sqrt3 = sqrtf(3);
    win_t *mainw = win_init("MainWindow", poll_draw, 800, 200);
    SHADERBLOCK_HANDLE(mainw->shaders, true);
    win_flood(mainw, (argb_t){.9, .7, .03, 1});

    // --- REPLACED UNSAFE STRING HANDLING ---
    // Using a fixed-size buffer and snprintf is much safer than manual
    // allocation, realloc, and concatenation. It prevents buffer overflows.
    #define MAX_PATH_LEN 256
    char path_buffer[MAX_PATH_LEN];

    // Safely build the shader path and pull shaders
    // The (cwd ? cwd : "") part handles case where cwd might be NULL after init
    snprintf(path_buffer, MAX_PATH_LEN, "%s/Resources/Shaders/Shaders.txt", cwd ? cwd : "");
    shaders_pull(path_buffer);

    // Safely build the texture path and append the image
    snprintf(path_buffer, MAX_PATH_LEN, "%s/Resources/Textures/AnotherBar.jpeg", cwd ? cwd : "");
    winimage_append(mainw, 800, 800, 4, path_buffer, (argb_t){0, 0, 0, 1});
    // No 'free' is needed because we used a stack-allocated array (path_buffer)

    win_draw(mainw,
        (GLfloat[]){
            // CORRECTED: Invalid octal '08.f' changed to '8.0f'
            -0.5f,   (-0.5f * sqrt3) / 3,    0,      8.0f, 0.3f, 0.2f,    0.0f, 5.0f,
            0.5f,    (-0.5f * sqrt3) / 3,    0,      8.0f, 0.3f, 0.2f,    5.0f, 0.0f,
            -0.5f,   (-0.5f * sqrt3 * 2) / 3,0,      8.0f, 0.3f, 0.2f,    0.0f, 0.0f,
            -0.5f/2, (0.5f * sqrt3) / 6,     0,      8.0f, 0.3f, 0.2f,    2.5f, 5.0f,
            0.5f/2,  (0.5f * sqrt3) / 6,     0,      8.0f, 0.3f, 0.2f,    2.5f, 5.0f,
            0.0f,    (0.5f * sqrt3) / 3,     0,      8.0f, 0.3f, 0.2f,    2.5f, 10.0f // Example texture coords
        },
        // CORRECTED: The array has 48 floats (6 vertices * 8 attributes), not 46
        48,
        (GLuint[]){
            0, 1, 2,
            0, 2, 3,
            0, 1, 4,
            1, 2, 4,
            2, 3, 4,
            3, 0, 4
        }, 18);

    glBindTextureEXT(GL_TEXTURE_2D, mainw->textures[mainw->textures_curr].img);

    win_poll(mainw);
    win_kill(mainw);
    return 0; // Returning 0 is standard for a successful execution
}

void poll_draw(win_t *win, size_t pollcycles){
    mat4 out, view, proj_screen;
    glm_mat4_identity(out);
    glm_mat4_identity(view);
    glm_mat4_identity(proj_screen);

    // CORRECTED: The variable 'g' was undeclared.
    // Declaring as a static float makes it persist between calls, creating an animation.
    static float g = 0.0f;
    g += 0.005f; // Increment rotation angle each frame for a spinning effect

    glm_translate(view, (vec3){0.0f, 0.0f, -5.0f}); // Adjusted translation for better view
    glm_perspective(glm_rad(45.0f), (float)win->w / (float)win->h, 0.1f, 100.0f, proj_screen);
    glm_mat4_mul(proj_screen, view, out);
    glm_rotate(out, g, (vec3){0.5f, 1.0f, 0.0f}); // Rotate on X and Y axis

    GLuint tex0_uni = glGetUniformLocation(win->shaders->shaderProgram, "tex0");
    glUniform1i(tex0_uni, 0);

    // Note: Passing strings like "mat4\0" is redundant. "mat4" is sufficient.
    uniform_write(win->shaders, "mat4", "matrices", "\0", true, out, 9);
    uniform_write(win->shaders, "inputvectors_bounds", "bounds", "start", true, 0, 1);
    uniform_write(win->shaders, "inputvectors_bounds", "bounds", "end", true, 100, 1);

    win_attrblink(win, 0, 3, GL_FLOAT, 8 * sizeof(float), (void *)0);
    win_attrblink(win, 1, 3, GL_FLOAT, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    win_attrblink(win, 2, 2, GL_FLOAT, 8 * sizeof(float), (void *)(6 * sizeof(float)));

    return;
}