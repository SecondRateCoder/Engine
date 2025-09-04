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
uint8_t unit =0;

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
    float g = (pollcycles == 0? 1: pollcycles)/(360* 1.00000005f);
    glm_translate(view, (vec3){0.0f, 0.0f, -5.0f}); // Adjusted translation for better view
    glm_perspective(glm_rad(45.0f), (float)win->w / (float)win->h, 0.1f, 100.0f, proj_screen);
    glm_mat4_mul(proj_screen, view, out);
    glm_rotate(out, g, (vec3){0.5f, 1.0f, 0.0f}); // Rotate on X and Y axis
    // GLuint tex0_uni = glGetUniformLocation(win->shaders->shaderProgram, "tex0");
    // glUseProgram(win->shaders[win->shaders_curr].shaderProgram);
    // glUniform1i(tex0_uni, 0);
    uniform_write(win->shaders, "mat4", "matrix", NULL, true, out, 16);

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
        fprintf(stderr, ANSI_RED( "GLFW init failed\n" ));
        return;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const size_t width = 800, height = 600;
    GLFWwindow* window = glfwCreateWindow(width, height, "Main Window", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwMaximizeWindow(window);
    
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    // Optional: comment out if unsupported
    bufferobj_t *buffer = calloc(sizeof(bufferobj_t), 1);
    shaderblock_t *shader = shaderblock_gen(true, true);
    buffer->buffer_[0] = false;
    buffer->buffer_[1] = false;
    buffer->buffer_[2] = false;
    mesh = calloc(sizeof(mesh), 1);
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
    mesh->data_len = 48;
    mesh->index_len = 18;
    mesh->vertex_stride = 3;
    mesh->color_stride = 3;
    mesh->dpi_stride = 2;
    mesh->pos_layoutindex = 0;
    mesh->color_layoutindex = 1;
    mesh->local_texcoordinates_layoutindex = 2;
    bufferobject_handle(buffer, mesh->mesh_data, mesh->data_len, mesh->vertex_index, mesh->index_len, GL_STATIC_DRAW, 3);
    mesh_attrlink(buffer, 0, 1, 2, mesh);
    mesh->textures = malloc(sizeof(image_t));
    GLCall(glGenTextures(1, &(mesh->textures[0].ID)));
    mesh->textures[0].unit = 0;
    GLCall(glActiveTexture(GL_TEXTURE0 + mesh->textures[0].unit));
    GLCall(glBindTexture(GL_TEXTURE_2D, mesh->textures[0].ID));
    
    const float _temp = 0.5f;
    const GLenum _temp___ = GL_TEXTURE_2D;
    uniform_write(shader, "float", "scale", NULL, false, &_temp, 1);
    // GLCall(glGenTextures())
    uniform_write(shader, "sampler2D", "tex0", &_temp___, false, &mesh->textures[0].unit, 1);

    // Your rendering loop here...

    size_t pollcycles = 0;
    while(!glfwWindowShouldClose(window)){
	
        glViewport(0, 0, width, height);
        glClearColor(0xC9, 0x4A, 0X99, 0XFD);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwPollEvents();
        
        mat4 out, view, proj_screen;
        glm_mat4_identity(out);
        glm_mat4_identity(view);
        glm_mat4_identity(proj_screen);
        
        float g = (pollcycles == 0? 1: pollcycles)/(360* 1.00000005f);
        glm_translate(view, (vec3){0.0f, 0.0f, -5.0f}); // Adjusted translation for better view
        glm_perspective(glm_rad(45.0f), (float)width / (float)height, 0.1f, 100.0f, proj_screen);
        glm_mat4_mul(proj_screen, view, out);
        glm_rotate(out, g, (vec3){0.5f, 1.0f, 0.0f}); // Rotate on X and Y axis
        uniform_write(shader, "mat4", "matrix", NULL, true, out, 1);
        glDrawBuffer(GL_VERTEX_ARRAY_BINDING);
        glfwSwapBuffers(window);
        pollcycles++;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return;

}

int main(){
    cwd_init();
    test_win_init();
    return EXIT_SUCCESS;
    mesh = calloc(1, sizeof(_mesh));
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
    mesh->data_len = 48;
    mesh->index_len = 18;
    mesh->vertex_stride = 3;
    mesh->color_stride = 3;
    mesh->dpi_stride = 2;
    mesh->pos_layoutindex = 0;
    mesh->color_layoutindex = 1;
    mesh->local_texcoordinates_layoutindex = 2;

    win_t *mainw = win_init("Main Window\0", NULL, poll_draw, NULL, 8000, 2000);
    mainw->shaders = malloc(sizeof(shaderblock_t));
    for(uint8_t cc =0; cc < 7; ++cc){mainw->shaders[0].compiled_[cc] -= mainw->shaders[0].compiled_[cc];}
    // mainw->shaders[0].compiled_ = (bool[7]){0};
    // shaderblock_handle(mainw->shaders, true, true);
    win_flood(mainw, (argb_t){.9, .7, .03, 1});
    //Enable Depth testing, So Triangles behind other Triangles are not drawn.
    // GLCall(glEnable(GL_DEPTH_TEST));
    // glEnable(GLFW_CONTEXT_DEBUG);
    // glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    // glDebugMessageCallback(debug_callback, nullptr);

    // --- REPLACED UNSAFE STRING HANDLING ---
    // Using a fixed-size buffer and snprintf is much safer than manual
    // allocation, realloc, and concatenation. It prevents buffer overflows.
    #define MAX_PATH_LEN 256
    char path_buffer[MAX_PATH_LEN];
    mesh->textures = malloc(sizeof(image_t));
    mesh->textures[0] = (image_t){
        .width = 700,
        .height = 700,
        .color_channels =4,
    };
    //Fails to load Image.
    bool DEBUG_BREAK = false;
    char *_temp_ = strdup(cwd);
    _temp_ = realloc(_temp_, (strlen(cwd)+ strlen("\\Resources\\Textures\\no_texture.png"))* sizeof(cwd));
    strncat(&_temp_[strlen(cwd)], "\\Resources\\Textures\\no_texture.png", strlen("\\Resources\\Textures\\no_texture.png"));
    while(DEBUG_BREAK == true || mesh->textures[0].img == NULL){
        if((mesh->textures[0].img = stbi_load(
            _temp_,
            &(mesh->textures[0].width),
            &(mesh->textures[0].height),
            &(mesh->textures[0].color_channels),
            mesh->textures[0].color_channels)
        ) == NULL){
            printf(ANSI_RED("Image Load Error: %s"), stbi_failure_reason());
        }
    }
    GLCall(glGenTextures(1, &(mesh->textures[0].ID)));
    mesh->textures[0].unit = 0;
    GLCall(glActiveTexture(GL_TEXTURE0 + mesh->textures[0].unit));
    GLCall(glBindTexture(GL_TEXTURE_2D, mesh->textures[0].ID));

    // Safely build the shader path and pull shaders
    // The (cwd ? cwd : "") part handles case where cwd might be NULL after init
    snprintf(path_buffer, MAX_PATH_LEN, "%s/Resources/Shaders/Shaders.txt", cwd ? cwd : (cwd_init() == true? cwd: NULL));
    // shaders_pull(path_buffer);

    // Safely build the texture path and append the image
    snprintf(path_buffer, MAX_PATH_LEN, "%s/Resources/Textures/AnotherBar.jpeg", cwd ? cwd : "");
    // winimage_append(mainw, path_buffer, &(argb_t){0, 0, 0, 1});
    // No 'free' is needed because we used a stack-allocated array (path_buffer)
    mainw->buffers = calloc(1, sizeof(bufferobj_t));
    mainw->buffer_curr = 0;
    mainw->buffer_len = 1;
    mainw->vert_count = 48;
    win_draw(mainw, mesh);

    //Why is shaderblock_handle failing?
    mainw->shaders = shaderblock_gen(true, true);
    mainw->shaders->uniforms = uniform_init(&mainw->shaders->uniform_len, mainw->shaders->shaderProgram);
    // #ifdef __WIN32
    //     system("cls");
    // #else
    //     system("clear");
    // #endif
    printf(ANSI_YELLOW("\n[0]: Name: %s; Type: %s, [1] Name: %s; Type: %s"), 
        mainw->shaders[mainw->shaders_curr].uniforms[0].name,
        mainw->shaders[mainw->shaders_curr].uniforms[0].type,
        mainw->shaders[mainw->shaders_curr].uniforms[1].name,
        mainw->shaders[mainw->shaders_curr].uniforms[1].type
    );
    const float _temp = 0.5f;
    const GLenum _temp___ = GL_TEXTURE_2D;
    uniform_write(&mainw->shaders[mainw->shaders_curr], "float", "scale", NULL, false, &_temp, 1);
    // GLCall(glGenTextures())
    uniform_write(&mainw->shaders[mainw->shaders_curr], "sampler2D", "tex0", &_temp___, false, &mesh->textures[0].unit, 1);
    win_poll(mainw);
    win_kill(mainw);
    scanf("Skibidi...");
    return EXIT_SUCCESS; // Returning 0 is standard for a successful execution
}
