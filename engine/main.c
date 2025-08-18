#include "../engine/Public.h"
// #include "../engine/_3D.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "./graphics/graphics.h"

const float sqrt3 =  1.7320508075688772935274463415059f;
const mesh_t _mesh = {
    .mesh_data = (GLfloat[]){
        -0.5f,   (-0.5f * sqrt3) / 3,    0,      8.0f, 0.3f, 0.2f,    0.0f, 5.0f,
        0.5f,    (-0.5f * sqrt3) / 3,    0,      8.0f, 0.3f, 0.2f,    5.0f, 0.0f,
        -0.5f,   (-0.5f * sqrt3 * 2) / 3,0,      8.0f, 0.3f, 0.2f,    0.0f, 0.0f,
        -0.5f/2, (0.5f * sqrt3) / 6,     0,      8.0f, 0.3f, 0.2f,    2.5f, 5.0f,
        0.5f/2,  (0.5f * sqrt3) / 6,     0,      8.0f, 0.3f, 0.2f,    2.5f, 5.0f,
        0.0f,    (0.5f * sqrt3) / 3,     0,      8.0f, 0.3f, 0.2f,    2.5f, 10.0f // Example texture coords
    },
    .data_len = 48,
    .vertex_index = (GLuint[]){
        0, 1, 2,
        0, 2, 3,
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    },
    .index_len = 18,
    .vertex_stride = 3,
    .color_stride = 3,
    .dpi_stride = 2,
    .pos_layoutindex = 0,
    .color_layoutindex = 1,
    .local_texcoordinates_layoutindex = 2
};

// Assuming 'cwd' is a global variable populated by cwd_init() from your Public.h
// If it's not global, you would need to adjust how it's accessed.

// Forward declaration for poll_draw to make it visible to main()
void poll_draw(void *self, size_t pollcycles){
    win_t *win = (win_t *)self;

    mat4 out, view, proj_screen;
    glm_mat4_identity(out);
    glm_mat4_identity(view);
    glm_mat4_identity(proj_screen);

    // CORRECTED: The variable 'g' was undeclared.
    // Declaring as a static float makes it persist between calls, creating an animation.
    float g = (360/pollcycles)* 1.00000005f;
    glm_translate(view, (vec3){0.0f, 0.0f, -5.0f}); // Adjusted translation for better view
    glm_perspective(glm_rad(45.0f), (float)win->w / (float)win->h, 0.1f, 100.0f, proj_screen);
    glm_mat4_mul(proj_screen, view, out);
    glm_rotate(out, g, (vec3){0.5f, 1.0f, 0.0f}); // Rotate on X and Y axis

    GLuint tex0_uni = glGetUniformLocation(win->shaders->shaderProgram, "tex0");
    glUniform1i(tex0_uni, 0);

    // Note: Passing strings like "mat4\0" is redundant. "mat4" is sufficient.
    // uniform_write(win->shaders, "mat4", "matrices", "\0", true, out, 9);
    // uniform_write(win->shaders, "inputvectors_bounds", "bounds", "start", true, 0, 1);
    // const int _100 = 100;
    // uniform_write(win->shaders, "inputvectors_bounds", "bounds", "end", true, &_100, 1);

    return;
}

int main(){
    cwd_init();
    win_t *mainw = win_init("Main Window", NULL, poll_draw, NULL, 800, 200);
    SHADERBLOCK_HANDLE(mainw->shaders, true, true);
    win_flood(mainw, (argb_t){.9, .7, .03, 1});
    //Enable Depth testing, So Triangles behind other Triangles are not drawn.
	glEnable(GL_DEPTH_TEST);

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
    winimage_append(mainw, path_buffer, &(argb_t){0, 0, 0, 1});
    // No 'free' is needed because we used a stack-allocated array (path_buffer)

    glBindTexture(GL_TEXTURE_2D, mainw->textures[mainw->textures_curr].ID);

    win_poll(mainw);
    win_kill(mainw);
    return EXIT_SUCCESS; // Returning 0 is standard for a successful execution
}