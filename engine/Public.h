#pragma once

#ifndef _PUBLIC_H
#define _PUBLIC_H

#include <Libraries/include/KHR/khrplatform.h>
#include <Libraries/include/GLFW/glfw3.h>
#include <Libraries/include/GLFW/glfw3native.h>
#include <Libraries/include/glad/glad.h>
#include <engine/graphics/drawingprotocol.h>
#include <engine/graphics/window/window.h>
#include <cglm/cglm.h>
#include <immintrin.h>
#include <xmmintrin.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define IS_NUM(X) ((int)X < (int)'9') && ((int)X > (int)'0')
#define IS_EVEN(X) (X%2 != 0)
#define INT_SIMP(X) (X< 0? -1: 1)
#define str_hash(str) _str_hash_(str)


#if defined(__APPLE__)
#include <stdlib.h>
#define memsize(BLOCK) malloc_good_size(BLOCK)
#elif defined(__unix__)
// code for Unix-like systems
#  if defined(__linux__)
#include <malloc.h>
#define memsize(BLOCK) malloc_usable_size(BLOCK)
#  endif
#elif defined(_WIN32) || defined(_WIN64)
#include <malloc.h>
#define memsize(BLOCK) _msize(BLOCK)
#else
#error "Unknown operating system"
#endif


char *str_normalise(char *str, bool handle_spaces, bool handle_upper);
const size_t str_hash_(char *str);
#endif