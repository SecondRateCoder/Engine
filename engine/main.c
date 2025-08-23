#include "../engine/Public.h"
// #include "../engine/_3D.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "./graphics/graphics.h"

const float sqrt3 =  1.7320508075688772935274463415059f;
mesh_t *mesh;

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

void test_win_init(){
    glfwSetErrorCallback(handle_glfw_error_default);
    if (!glfwInit()) {
        fprintf(stderr, "GLFW init failed\n");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Optional: comment out if unsupported
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Main Window", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Window creation failed\n");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    // Your rendering loop here...

    glfwDestroyWindow(window);
    glfwTerminate();
    return;

}
int main(){
    // test_win_init();
    // return EXIT_SUCCESS;
    cwd_init();
    mesh = malloc(sizeof(_mesh));
    mesh->mesh_data = malloc(sizeof(GLfloat)* 48);
    mesh->mesh_data = (GLfloat[48]){
        -0.5f,   (-0.5f * sqrt3) / 3,    0,      8.0f, 0.3f, 0.2f,    0.0f, 5.0f,
        0.5f,    (-0.5f * sqrt3) / 3,    0,      8.0f, 0.3f, 0.2f,    5.0f, 0.0f,
        -0.5f,   (-0.5f * sqrt3 * 2) / 3,0,      8.0f, 0.3f, 0.2f,    0.0f, 0.0f,
        -0.5f/2, (0.5f * sqrt3) / 6,     0,      8.0f, 0.3f, 0.2f,    2.5f, 5.0f,
        0.5f/2,  (0.5f * sqrt3) / 6,     0,      8.0f, 0.3f, 0.2f,    2.5f, 5.0f,
        0.0f,    (0.5f * sqrt3) / 3,     0,      8.0f, 0.3f, 0.2f,    2.5f, 10.0f // Example texture coords
    };
    mesh->vertex_index = malloc(sizeof(GLuint)* 18);
    mesh->vertex_index = (GLuint[]){
        0, 1, 2,
        0, 2, 3,
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };
    mesh->index_len = 18;
    mesh->index_len = 18;
    mesh->vertex_stride = 3;
    mesh->color_stride = 3;
    mesh->dpi_stride = 2;
    mesh->pos_layoutindex = 0;
    mesh->color_layoutindex = 1;
    mesh->local_texcoordinates_layoutindex = 2;
    win_t *mainw = win_init("Main Window\0", NULL, poll_draw, NULL, 800, 200);
    mainw->shaders = malloc(sizeof(shaderblock_t));
    for(uint8_t cc =0; cc < 7; ++cc){mainw->shaders[0].compiled_[cc] -= mainw->shaders[0].compiled_[cc];}
    // mainw->shaders[0].compiled_ = (bool[7]){0};
    SHADERBLOCK_HANDLE(mainw->shaders, true, true);
    win_flood(mainw, (argb_t){.9, .7, .03, 1});
    //Enable Depth testing, So Triangles behind other Triangles are not drawn.
	glEnable(GL_DEPTH_TEST);

    // --- REPLACED UNSAFE STRING HANDLING ---
    // Using a fixed-size buffer and snprintf is much safer than manual
    // allocation, realloc, and concatenation. It prevents buffer overflows.
    #define MAX_PATH_LEN 256
    char path_buffer[MAX_PATH_LEN];
    mesh->textures = malloc(sizeof(image_t));
    mesh->textures[0] = (image_t){
        .width = 800,
        .height = 200,
        .color_channels =4,
    };
    mesh->textures[0].img = stbi_load(
        "C:\\Users\\olusa\\OneDrive\\Documents\\GitHub\\Engine\\Resources\\Textures\\AnotherBar.png",
        &(mesh->textures[0].width),
        &(mesh->textures[0].height),
        &(mesh->textures[0].color_channels),
        mesh->textures[0].color_channels
    );
    glGenTextures(1, &(mesh->textures[0].ID));
    glBindTexture(GL_TEXTURE_2D, mesh->textures[0].ID);

    // Safely build the shader path and pull shaders
    // The (cwd ? cwd : "") part handles case where cwd might be NULL after init
    snprintf(path_buffer, MAX_PATH_LEN, "%s/Resources/Shaders/Shaders.txt", cwd ? cwd : (cwd_init() == true? cwd: NULL));
    shaders_pull(path_buffer);

    // Safely build the texture path and append the image
    snprintf(path_buffer, MAX_PATH_LEN, "%s/Resources/Textures/AnotherBar.jpeg", cwd ? cwd : "");
    // winimage_append(mainw, path_buffer, &(argb_t){0, 0, 0, 1});
    // No 'free' is needed because we used a stack-allocated array (path_buffer)

    win_draw(mainw, mesh);
    win_poll(mainw);
    win_kill(mainw);
    scanf("Skibidi...");
    return EXIT_SUCCESS; // Returning 0 is standard for a successful execution
}
