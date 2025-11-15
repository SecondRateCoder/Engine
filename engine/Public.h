#pragma once

#ifndef _PUBLIC_H
#define _PUBLIC_H

#include "../include/stb/stb_image.h"

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

#define _DEBUG_

#define GLUseProgram(prog) _GLUseprogram(prog);

	
#ifdef _DEBUG_
	#define GLCall(x) \
		x; \
		CheckGLError(__FILE__, __LINE__, #x);
#else
	#define GLCall(x)	\
		x;
#endif

// 3. Utility macros
#define IS_NUM(c)    ( ((int)(c) >= '0') && ((int)(c) <= '9') )
#define IS_EVEN(x)   ( ((x) % 2) == 0 )
#define INT_SIMP(x)  ( ((x) < 0) ? -1 : 1 )
#define IS_SPACE(c)  ( (c) == ' ' )
#define glDeleteBuffer(BUFFER) glDeleteBuffers(1, &BUFFER)
#define glGenBuffer(BUFFER) glGenBuffers(1, BUFFER)
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_RED(TEXT) "\n" ANSI_COLOR_RED TEXT ANSI_COLOR_RESET
#define ANSI_GREEN(TEXT) "\n" ANSI_COLOR_GREEN TEXT ANSI_COLOR_RESET
#define ANSI_YELLOW(TEXT) "\n" ANSI_COLOR_YELLOW TEXT ANSI_COLOR_RESET
#define ANSI_BLUE(TEXT) "\n" ANSI_COLOR_BLUE TEXT ANSI_COLOR_RESET
#define ANSI_MAGENTA(TEXT) "\n" ANSI_COLOR_MAGENTA TEXT ANSI_COLOR_RESET
#define ANSI_CYAN(TEXT) "\n" ANSI_COLOR_CYAN  TEXT ANSI_COLOR_RESET


// 4. String utils
char   *str_normalise(const char *str, bool handle_spaces, bool handle_upper);
size_t *str_hash(const char *str);
void    str_tolower(char *str);

// 5. 128-bit compare
typedef size_t uint128_t[2];
bool uint128_t_comps(const uint128_t a, const size_t b);
bool uint128_t_comp (const uint128_t a, const uint128_t b);

// 6. stb_image

// 7. OpenGL / GLFW / cglm
#include "glad.h"
#include "khrplatform.h"
#include "../include/glfw3.h"
#ifdef _WIN32
#include "../include/glfw3native.h"
#endif
#include <cglm/cglm.h>

// 8. Engine headers
#include "../graphics/graphics.h"   // defines image_t, bufferobj_t
#include "../_3D.h"                 // uses mesh_t, image_t, bufferobj_t

// 10. Mesh helpers
void mesh_attrlink(bufferobj_t *buffer, const int pos_layout,  const int col_layout,  const int tex_layout, mesh_t *_mesh);
void mesh_bindtexture(mesh_t *m, image_t *texture);
void CheckGLError(const char* file, int line, const char* call);
void draw_debug_trace(const char* file, int line);
void _GLUseprogram(GLuint prog);

extern char *cwd;
extern size_t cwd_len;
bool cwd_init();

extern const float sqrt3;
extern const char vertexshader_default[142];
extern const char fragmentshader_default[118];
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
	** shader_typenames
	/**computeshader*/
;
extern const char* builtin_shader_typenames[];
extern const uint128_t builtin_shader_typehash[37];
void GLAPIENTRY debug_callback(
	GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length,
	const GLchar *message, const void *userParam
);

#endif // _PUBLIC_H

