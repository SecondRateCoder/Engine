#pragma once

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <io.h>
#include <excpt.h>
#include <stdint.h>

#ifndef GLFW_INCLUDE_VULKAN
    #define GLFW_INCLUDE_VULKAN
#endif

#define win_t win

typedef struct win{
    GLFWwindow *window;
    char *name;
    uint32_t w, h;
}win;