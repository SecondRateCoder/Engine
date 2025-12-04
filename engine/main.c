#include "../engine/Public.h"

const float sqrt3 =  1.7320508075688772935274463415059f;
uint8_t unit =0;

// Assuming 'cwd' is a global variable populated by cwd_init() from your Public.h
// If it's not global, you would need to adjust how it's accessed.

// Forward declaration for poll_draw to make it visible to main()
void poll_draw(void *self, size_t polls, uint32_t pollcycles){
    win_t *win = (win_t *)self;

	GLUseProgram(win->shaders[win->shaders_curr].shaderProgram);
    glUniform1i(glGetUniformLocation(win->shaders->shaderProgram, "tex0"), 0);
    
    mat4 out, view, proj;
    glm_mat4_identity(out);
    glm_mat4_identity(view);
    glm_mat4_identity(proj_screen);

    float g = (360* 1.05f)/(polls* 10);
    glm_translate(view, win->scenes[win->scene_curr].cameras[*win->scenes[win->scene_curr].loaded_cams].pos);
    glm_perspective(glm_rad(45.0f + (45 * g)), (float)win->w / (float)win->h, 0.1f, 100.0f, proj_screen);
    glm_mat4_mul(proj_screen, view, out);
	glm_rotate(out, g, win->scenes[win->scene_curr].cameras[*win->scenes[win->scene_curr].loaded_cams].rot); // Rotate on X and Y axis
	
	GLUseProgram(win->shaders[win->shaders_curr].shaderProgram);
    GLCall(glUniform1i(glGetUniformLocation(win->shaders[win->shaders_curr].shaderProgram, "view_m4"), 0));
    return;
}

const GLfloat vertdata[48] = {
    // Position coords                      // Color data           // Texture coords
    -0.5f,   (-0.5f * sqrt3) / 3,    0,      8.0f, 0.3f, 0.2f,    0.0f, 5.0f,
    0.5f,    (-0.5f * sqrt3) / 3,    0,      8.0f, 0.3f, 0.2f,    5.0f, 0.0f,
    -0.5f,   (-0.5f * sqrt3 * 2) / 3,0,      8.0f, 0.3f, 0.2f,    0.0f, 0.0f,
    -0.5f/2, (0.5f * sqrt3) / 6,     0,      8.0f, 0.3f, 0.2f,    2.5f, 5.0f,
    0.5f/2,  (0.5f * sqrt3) / 6,     0,      8.0f, 0.3f, 0.2f,    2.5f, 5.0f,
    0.0f,    (0.5f * sqrt3) / 3,     0,      8.0f, 0.3f, 0.2f,    2.5f, 10.0f // Example texture coords
};

const GLuint indexdata[18] = {
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

    mesh_t *mesh = calloc(1, sizeof(mesh_t));
    mesh->vertex_data = calloc(48, sizeof(GLfloat));
    memcpy(mesh->vertex_data, vertdata, 48* sizeof(GLfloat));
    mesh->index_data = calloc(18, sizeof(GLuint));
    memcpy(mesh->index_data, indexdata, 18* sizeof(GLuint));

    mesh->data_len = 48;
    mesh->index_len = 18;
    mesh->strides = 3 + (3 << 8) + (2 << 16);
    mesh->layouts = 0 + (1 << 8) + (2 << 16);
    bufferobject_handle(buffer, mesh->vertex_data, mesh->data_len, mesh->index_data, mesh->index_len, GL_STATIC_DRAW, 3);
    mesh_attrlink(buffer, mesh);
    mesh->texture = calloc(1, sizeof(image_t));
    GLCall(glGenTextures(1, &(mesh->texture[0].ID)));
    mesh->texture[0].unit = 0;
    GLCall(glActiveTexture(GL_TEXTURE0 + mesh->texture[0].unit));
    GLCall(glBindTexture(GL_TEXTURE_2D, mesh->texture[0].ID));
    
    const float _temp = 0.5f;
    const GLenum _temp___ = GL_TEXTURE_2D;
    uniform_write(shader, "float", "scale", NULL, false, &_temp, 1);
    // GLCall(glGenTextures())
    uniform_write(shader, "sampler2D", "tex0", &_temp___, false, &mesh->texture[0].unit, 1);
    const vec3 _temp_ = {0, 0, -11.4};
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

#ifdef _WIN32
#include <windows.h>
void enableANSI() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (hOut == INVALID_HANDLE_VALUE){return;}
    if (!GetConsoleMode(hOut, &dwMode)){return;}
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

#endif

int main(){
#ifdef _WIN32
    enableANSI();
#endif
    cwd_init();
    win_t *mainw = win_init("MainWindow", NULL, poll_draw, NULL, 1000, 800);
    // glViewport(0, 0, 1000, 800);
    // glEnable(GL_SCISSOR_TEST);
	// glScissor(0, 0, 1000, 800);
    // mesh = calloc(sizeof(mesh), 1);
    // mesh->vertex_data = malloc(sizeof(GLfloat)* 48);
    // memcpy(mesh->vertex_data, vertex_data, 48* sizeof(GLfloat));
    // mesh->index_data = malloc(sizeof(GLuint)* 18);
    // memcpy(mesh->index_data, mesh_index, 18* sizeof(GLuint));
    // mesh->texture = calloc(1, sizeof(image_t));
    
    // mesh->data_len = 48;
    // mesh->index_len = 18;
    // mesh->vertex_stride = 3;
    // mesh->color_stride = 3;
    // mesh->uv_stride = 2;
    // mesh->pos_layoutindex = 0;
    // mesh->color_layoutindex = 1;
    // mesh->local_texcoordinates_layoutindex = 2;

    // mesh->texture->width = 800;
    // mesh->texture->height = 200;
    // mesh->texture->border[0] = 1;
    // mesh->texture->border[1] = 1;
    // mesh->texture->border[2] = 1;
    // mesh->texture->border[3] = 1;
    // mesh->texture->color_channels = 4;
    // // mesh->texture->format = calloc(1, sizeof(texformat_t));
    // mesh->texture->format = (texformat_t){
    //     .target = GL_TEXTURE_2D,
    //     .pixel_format = GL_RGBA,
    //     .pixel_type = GL_UNSIGNED_BYTE,
    //     .level = 0,
    //     .internalFormat = GL_RGBA,
    //     .depth = 0,
    // };
    // mesh->texture->img = stbi_load(
    //     temp,
    //     &mesh->texture->width,
    //     &mesh->texture->height,
    //     &mesh->texture->color_channels,
    //     4
    // );
    // free(temp);
    mesh_bindtexture(wscene_curr(mainw).meshes, wscene_curr(mainw).meshes->texture);

    // mainw->buffers = bufferobj_gen(mesh, GL_STATIC_DRAW, 0, 1, 2);
    // mainw->buffer_len = 1;
    // mesh->texture = malloc(sizeof(image_t));

    // mainw->vert_count = 8;
    glEnable(GL_DEPTH);
    scene_inputh_regm(
        mainw->scenes + *mainw->loaded_scenes, 
        (GLenum[7]){GLFW_KEY_PAGE_UP, GLFW_KEY_PAGE_DOWN, GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT, GLFW_KEY_MOUSE_MOVE},
        (GLenum[7]){GLFW_PRESS, GLFW_PRESS, GLFW_PRESS, GLFW_PRESS, GLFW_PRESS, GLFW_PRESS, GLFW_PRESS},
        7, INPUTH_CAMHandle_0, true
    );
    //!NOT GOOD MATE
    uniform_write(mainw->scenes->shaders, "vec3", "offs", NULL, true, (float[3]){-5, 0, 0}, 3);
    uniform_write(mainw->scenes->shaders, "float", "scale", NULL, true, (float[1]){0}, 1);
    win_poll(mainw);
    if(mainw->g_window != NULL){win_kill(mainw);}
    
    return EXIT_SUCCESS; // Returning 0 is standard for a successful execution
}
