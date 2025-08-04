#pragma once

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <gl/GL.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <io.h>
// #include <excpt.h>
#include <stdint.h>

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