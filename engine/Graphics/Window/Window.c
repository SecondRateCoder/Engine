#include <engine/Graphics/Window/Window.h>


win_t *win_init(char *name, uint32_t w, uint32_t h){
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    win_t *win = alloca(sizeof(win_t));
    win->name = name;
    win->window =glfwCreateWindow(w, h, name, NULL, NULL);
    return win;
}

bool win_poll(win_t *win){
    while(!win_shouldclose(win)){
        glfwPollEvents();
    }
    win_kill(win);
}

bool win_shouldclose(win_t *win){return glfwWindowShouldClose(win->window);}

void win_kill(win_t *win){
    glfwDestroyWindow(win->window);
    glfwTerminate();
}