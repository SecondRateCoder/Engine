#pragma once

#ifndef _PUBLIC_H
#define _PUBLIC_H

#include "./Libraries/include/glad.h"
#include "./Libraries/include/khrplatform.h"
#include "./Libraries/include/glfw3.h"
#include "./Libraries/include/glfw3native.h"
/// @brief A type with a size_t lower and size_t upper, using these to describe 16 byte values.
typedef size_t uint128_t[2];  // Simulate 128-bit with two 64-bit chunks
#include "../engine/graphics/graphics.h"
#include "./Libraries/include/cglm/cglm.h"
#include "../engine/_3D.h"
#include <ctype.h>
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
#define IS_SPACE(chr) (int)chr == (int)' '


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

char *str_normalise(const char *str, bool handle_spaces, bool handle_upper);
size_t *str_hash(const char *str);
void str_tolower(char *str);
bool uint128_t_comps(const uint128_t a, const size_t b);
bool uint128_t_comp(const uint128_t a, const uint128_t b);
#endif