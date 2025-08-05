#include <engine/Graphics/Window/Window.h>

/// @brief Initialise an Un-Resizable GLFW Window.
/// @param name The Name of the Window.
/// @param w The Width of the Window.
/// @param h The Height of the Window.
/// @return An initialised Pointer to the Window.
win_t *win_init(char *name, uint32_t w, uint32_t h){
    glfwInit();
    // glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    //Using Legacy, OpenGL 3.3 for my 9800 GT GPU.
    glfwWindowHint(GLFW_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    win_t *win = alloca(sizeof(win_t));
    win->name = name;
    win->w =w;
    win->h =h;
    win->window = glfwCreateWindow(w, h, name, NULL, NULL);
    if(win->window == NULL){
        printf("Window failed to Initialise\nError!");
        glfwTerminate();
        return NULL;
    }else{
        glfwMakeContextCurrent(win->window);
        gladLoadGL();
    }
    return win;
}

/// @brief Poll the GLFW window's events.
/// @param win The Window to be Polled.
/// @remark If the Window is set to close, It will Kill it.
void win_poll(win_t *win){
    while(!win_shouldclose(win)){glfwPollEvents();}
    win_kill(win);
}

/// @brief Check if the specified Window was set to be closed.
/// @param win The Window to be checked.
/// @return Was the User trying to close the Window.
bool win_shouldclose(win_t *win){return glfwWindowShouldClose(win->window);}

/// @brief Terminate and free the specified Windw Pointer.
/// @param win The Window to be terminated.
void win_kill(win_t *win){
    glfwDestroyWindow(win->window);
    glfwTerminate();
	free(win->name);
	free(win);
}

/// @brief Fill the Window's front and back buffer with a specified Color constant.
/// @param win The Window.
/// @param c The Color to be applied.
void win_flood(win_t *win, const argb_t c){
    glViewport(0, 0, win->w, win->h);
    glClearColor(c.r, c.g, c.b, c.a);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(win->window);
}

