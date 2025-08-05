#pragma once

#include <Public.h>
// #include <excpt.h>

// #ifndef GLFW_INCLUDE_VULKAN
//     #define GLFW_INCLUDE_VULKAN
// #endif

//Header Guard
#ifndef _WIN_H
#define _WIN_H



#define win_t win
#define argb_t Color4

typedef struct win{
	GLFWwindow *window;
	char *name;
	shaderblock_t *shaders;
	size_t VAO_len, VAO_curr, VBO_len, VBO_curr, vert_count;
	/*
	[0]: Is VAO set-up?
	[1]: Is VBO set-up?
	[2]: Is EBO set-up?
	*/
	bool buffer_[3];
	GLuint *VAO, *VBO, EBO;
	uint32_t x, y, w, h;
}win;



typedef struct Color4{
	/// @brief a: 1:= Solid, 0:= Transparent.
	float a, r, g, b;
}Color4;

win_t *win_init(char *name, uint32_t w, uint32_t h);
void win_poll(win_t *win);
bool win_shouldclose(win_t *win);
void win_kill(win_t *win);
void win_flood(win_t *win, const argb_t c);

void win_draw(win_t *win, GLfloat *points, size_t len, GLuint *indexes, size_t ilen);
void win_attrblink(win_t *win, GLuint layout, GLuint component_num, GLenum type, GLsizeiptr stride, void *offset);

#endif
