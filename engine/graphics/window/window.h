#pragma once
#include "Public.h"
#include "drawingprotocol.h"

// Header Guard
#ifndef _WIN_H
#define _WIN_H


#define win_t window
#define argb_t Color4

// #include <excpt.h> // This is a Windows-specific header and may not be portable.

// Using a macro to define a type alias is generally discouraged in C++
// It's better to use a `typedef`.
// typedef struct window win_t;
// typedef struct Color4 argb_t;


typedef struct window{
    GLFWwindow *window;
    char *name;
    poll_do polld;
    poll_kill pollk;
    shaderblock_t *shaders;
    size_t textures_len, textures_curr, vert_count;
    
    size_t buffer_len, buffer_curr;
    bufferobj_t *buffers;
    image_t *textures;
    uint32_t x, y, w, h;
}window;

// A Function Pointer for doing custom functions with the Polling of the Window.
// The `poll_do` function signature should be `void (*poll_do)(struct window *, size_t);`
// to ensure the `window` struct is a known type within the typedef.
// It's also better to use `const size_t` for the size parameter if it's not modified.
typedef void (*poll_do)(win_t *, const size_t);

/// @brief A function called when the Window should be killed.
typedef void (*poll_kill)(win_t *);

// It is common practice to define the structs before their aliases.
typedef struct Color4 {
    // @brief a: 1.0f := Solid, 0.0f := Transparent.
    float a, r, g, b;
} Color4;


// The function pointer parameter should be a pointer to the `poll_do` type, not a pointer to a pointer.
// Also, the return type should be `window *`
win_t *win_init(char *name, poll_do *polld, poll_kill *poolk, uint32_t w, uint32_t h);
void win_poll(win_t *win);
bool win_shouldclose(win_t *win);
void win_kill(win_t *win);

// The function signature for `win_attrblink` is a bit strange, `GLuint` for layout and `GLuint` for `component_num` is fine, but it's not clear what `win_attrblink` is doing. It's likely a typo and should be `glVertexAttribPointer`. I've left the signature as is, but it may require further correction in the implementation file. The `offset` parameter is usually a `const void *`.
void win_attrblink(window *win, GLuint layout, GLuint component_num, GLenum type, GLsizeiptr stride, const void *offset);


#endif // _WIN_H