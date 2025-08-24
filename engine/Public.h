#pragma once

#ifndef _PUBLIC_H
#define _PUBLIC_H

// 1. Standard C
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

// 2. Cross-platform memsize
#if defined(__APPLE__)
#  include <malloc/malloc.h>
#  define memsize(p) malloc_good_size(p)
#elif defined(__linux__)
#  include <malloc.h>
#  define memsize(p) malloc_usable_size(p)
#elif defined(_WIN32) || defined(_WIN64)
#  include <malloc.h>
#  define memsize(p) _msize(p)
#else
#  error "Unknown platform for memsize"
#endif

// 3. Utility macros
#define IS_NUM(c)    ( ((int)(c) >= '0') && ((int)(c) <= '9') )
#define IS_EVEN(x)   ( ((x) % 2) == 0 )
#define INT_SIMP(x)  ( ((x) < 0) ? -1 : 1 )
#define IS_SPACE(c)  ( (c) == ' ' )

// 4. String utils
char   *str_normalise(const char *str, bool handle_spaces, bool handle_upper);
size_t *str_hash(const char *str);
void    str_tolower(char *str);

// 5. 128-bit compare
typedef size_t uint128_t[2];
bool uint128_t_comps(const uint128_t a, const size_t b);
bool uint128_t_comp (const uint128_t a, const uint128_t b);

// 6. stb_image
#include "../include/stb/stb_image.h"

// 7. OpenGL / GLFW / cglm
#include "glad.h"
#include "khrplatform.h"
#include "../include/glfw3.h"
#ifdef _WIN32
#include "../include/glfw3native.h"
#endif
#include <cglm/cglm.h>

// 8. Engine headers
#include "../engine/graphics/graphics.h"   // defines image_t, bufferobj_t
#include "../engine/_3D.h"                 // uses mesh_t, image_t, bufferobj_t


// 9. Global engine object
extern mesh_t *mesh;


// 10. Mesh helpers
void mesh_attrlink(bufferobj_t *buffer,
                   uint32_t        pos_layout,
                   uint32_t        color_layout,
                   uint32_t        tex_layout,
                   mesh_t         *_mesh);

void mesh_addtexture(mesh_t *m, image_t *texture);



extern char *cwd;
extern size_t cwd_len;
bool cwd_init();

extern const char vertexshader_default[135];
extern const char fragmentshader_default[111];
extern const unsigned int settings_len;

/// @brief The Constant Shader keywords for Understanding Shader text.
extern const char vs_start[10],
fs_start[10],gs_start[10],
tcs_start[10], tes_start[10],// cs_start[10],
shader_end[18];

extern size_t len_typenames;
/// @brief Shader RAWs in case any part of the Program needs them.
extern char* vertexshader,
	* fragmentshader,
	* geometryshader,
	** shader_typenames,
	*tessellation_controlshader,
	*tessellation_evaluationshader
	/**computeshader*/
;
extern const char* builtin_shader_typenames[];
extern const uint128_t builtin_shader_typehash[37];

#endif // _PUBLIC_H

