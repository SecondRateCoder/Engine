#include "../engine/Public.h"
// #include "../engine/_3D.h"

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
void poll_draw(void *self, size_t polls, uint32_t pollcycles){
    win_t *win = (win_t *)self;
    
    mat4 out, view, proj_screen;
    glm_mat4_identity(out);
    glm_mat4_identity(view);
    glm_mat4_identity(proj_screen);
    
    // CORRECTED: The variable 'g' was undeclared.
    // Declaring as a static float makes it persist between calls, creating an animation.
    float g = (polls* 10)/(360* 1.00000005f);
    glm_translate(view, (vec3){0.0f, 0.0f, -5.0f}); // Adjusted translation for better view
    glm_perspective(glm_rad(45.0f), (float)win->w / (float)win->h, 0.1f, 100.0f, proj_screen);
    glm_mat4_mul(proj_screen, view, out);
    uint32_t cc =0;
    do{
        glm_rotate(out, g, (vec3){0.5f, .75f, 0.25f}); // Rotate on X and Y axis
        ++cc;
    }while(cc < pollcycles);
    // GLuint tex0_uni = glGetUniformLocation(win->shaders->shaderProgram, "tex0");
    // GLUseProgram(win->shaders[win->shaders_curr].shaderProgram);
    // glUniform1i(tex0_uni, 0);
    const vec3 offs_temp = {-1.5f, sin(polls), -0.5f};
    uniform_write(win->shaders, "vec3", "offs", NULL, true, &offs_temp, 3);
    uniform_write(win->shaders, "mat4", "matrix", NULL, true, out, 1);

    // Note: Passing strings like "mat4\0" is redundant. "mat4" is sufficient.
    // uniform_write(win->shaders, "mat4", "matrices", "\0", true, out, 9);
    // uniform_write(win->shaders, "inputvectors_bounds", "bounds", "start", true, 0, 1);
    // const int _100 = 100;
    // uniform_write(win->shaders, "inputvectors_bounds", "bounds", "end", true, &_100, 1);
    
    return;
}

const GLfloat mesh_data[48] = {
    -0.5f,   (-0.5f * sqrt3) / 3,    0,      8.0f, 0.3f, 0.2f,    0.0f, 5.0f,
    0.5f,    (-0.5f * sqrt3) / 3,    0,      8.0f, 0.3f, 0.2f,    5.0f, 0.0f,
    -0.5f,   (-0.5f * sqrt3 * 2) / 3,0,      8.0f, 0.3f, 0.2f,    0.0f, 0.0f,
    -0.5f/2, (0.5f * sqrt3) / 6,     0,      8.0f, 0.3f, 0.2f,    2.5f, 5.0f,
    0.5f/2,  (0.5f * sqrt3) / 6,     0,      8.0f, 0.3f, 0.2f,    2.5f, 5.0f,
    0.0f,    (0.5f * sqrt3) / 3,     0,      8.0f, 0.3f, 0.2f,    2.5f, 10.0f // Example texture coords
};

const GLuint mesh_index[18] = {
    0, 1, 2,
    0, 2, 3,
    0, 1, 4,
    1, 2, 4,
    2, 3, 4,
    3, 0, 4
};

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
    shaderblock_t *shader = NULL;
    shaderblock_handle(shader, true, true);
    buffer->buffer_[0] = false;
    buffer->buffer_[1] = false;
    buffer->buffer_[2] = false;

    mesh = calloc(sizeof(mesh), 1);
    mesh->mesh_data = malloc(sizeof(GLfloat)* 48);
    memcpy(mesh->mesh_data, mesh_data, 48* sizeof(GLfloat));
    mesh->vertex_index = malloc(sizeof(GLuint)* 18);
    memcpy(mesh->vertex_index, mesh_index, 18* sizeof(GLuint));

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
    mesh->texture = malloc(sizeof(image_t));
    GLCall(glGenTextures(1, &(mesh->texture[0].ID)));
    mesh->texture[0].unit = 0;
    GLCall(glActiveTexture(GL_TEXTURE0 + mesh->texture[0].unit));
    GLCall(glBindTexture(GL_TEXTURE_2D, mesh->texture[0].ID));
    
    const float _temp = 0.5f;
    const GLenum _temp___ = GL_TEXTURE_2D;
    uniform_write(shader, "float", "scale", NULL, false, &_temp, 1);
    // GLCall(glGenTextures())
    uniform_write(shader, "sampler2D", "tex0", &_temp___, false, &mesh->texture[0].unit, 1);
    const vec3 _temp_ = {0, -2, -1};
    uniform_write(shader, "vec3", "offs", NULL, true, &_temp_, 3);

    // Your rendering loop here...

    size_t pollcycles = 0;
    while(!glfwWindowShouldClose(window)){
	
        glViewport(0, 0, width, height);
        glClearColor(201.0f/255.0f, 74.0f/255.0f, 153.0f/255.0f, 253.0f/255.0f);
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
        glBindVertexArray(buffer->VAO);
        glDrawElements(GL_TRIANGLES, mesh->index_len, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        pollcycles++;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return;

}

int main(){
    cwd_init();
    // test_win_init();
    // return EXIT_SUCCESS;
    win_t *mainw = win_init("MainWindow", NULL, poll_draw, NULL, 1000, 800);
    mainw->shaders = shaderblock_gen(true, true);
    
    mesh = calloc(sizeof(mesh), 1);
    mesh->mesh_data = malloc(sizeof(GLfloat)* 48);
    memcpy(mesh->mesh_data, mesh_data, 48* sizeof(GLfloat));
    mesh->vertex_index = malloc(sizeof(GLuint)* 18);
    memcpy(mesh->vertex_index, mesh_index, 18* sizeof(GLuint));
    mesh->texture = calloc(1, sizeof(image_t));
    
    mesh->data_len = 48;
    mesh->index_len = 18;
    mesh->vertex_stride = 3;
    mesh->color_stride = 3;
    mesh->dpi_stride = 2;
    mesh->pos_layoutindex = 0;
    mesh->color_layoutindex = 1;
    mesh->local_texcoordinates_layoutindex = 2;

    mesh->texture->width = 800;
    mesh->texture->height = 200;
    mesh->texture->border[0] = 1;
    mesh->texture->border[1] = 1;
    mesh->texture->border[2] = 1;
    mesh->texture->border[3] = 1;
    mesh->texture->color_channels = 4;
    mesh->texture->format = calloc(1, sizeof(texformat_t));
    *mesh->texture->format = (texformat_t){
        .target = GL_TEXTURE_2D,
        .pixel_format = GL_RGBA,
        .pixel_type = GL_UNSIGNED_BYTE,
        .level = 0,
        .internalFormat = GL_RGBA,
        .depth = 0,
    };
    char *temp = strdup(cwd);
    temp = realloc(temp, strlen(cwd)+ strlen("\\Resources\\Textures\\no_texture.png"));
    strncat(temp, "\\Resources\\Textures\\no_texture.png", strlen("\\Resources\\Textures\\no_texture.png"));
    mesh->texture->img = stbi_load(
        temp,
        &mesh->texture->width,
        &mesh->texture->height,
        &mesh->texture->color_channels,
        4
    );
    free(temp);
    mesh_bindtexture(mesh, mesh->texture);

    mainw->buffers = bufferobj_gen(mesh, GL_STATIC_DRAW, 0, 1, 2);
    mainw->buffer_len = 1;
    mesh->texture = malloc(sizeof(image_t));

    const float _temp = 0.5f;
    const GLenum _temp_ = GL_TEXTURE_2D;
    uniform_write(mainw->shaders, "float", "scale", NULL, false, &_temp, 1);
    // GLCall(glGenTextures())
    uniform_write(mainw->shaders, "sampler2D", "tex0", &_temp_, false, &mesh->texture[0].unit, 1);
    vec3 offs_temp = {-1.5f, -2.0f, -0.5f};
    uniform_write(mainw->shaders, "vec3", "offs", NULL, true, &offs_temp, 3);

    mainw->vert_count = 8;
    glEnable(GL_DEPTH);
    win_poll(mainw, NULL);
    if(mainw->g_window != NULL){win_kill(mainw);}
    
    return EXIT_SUCCESS; // Returning 0 is standard for a successful execution
}
