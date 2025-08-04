#pragma once

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glad.h>
#include <Public.h>
// #include <excpt.h>

// #ifndef GLFW_INCLUDE_VULKAN
//     #define GLFW_INCLUDE_VULKAN
// #endif

#define win_t win
#define argb_t Color4
#define pointf_t point_f

typedef struct win{
    GLFWwindow *window;
    char *name;
    uint32_t x, y, w, h;
}win;

typedef struct point_f{float x, y;}point_f;

typedef struct Color4{
    /// @brief a: 1:= Solid, 0:= Transparent.
    float a, r, g, b;
}Color4;

win_t *win_init(char *name, uint32_t w, uint32_t h);
void win_poll(win_t *win);
bool win_shouldclose(win_t *win);
void win_kill(win_t *win);
void win_flood(win_t *win, const argb_t c);
void win_draw(win_t *win, pointf_t *points, size_t len);