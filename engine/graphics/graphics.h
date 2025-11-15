#pragma once

#include "../Public.h"
#include "../_3D.h"
// #include "../_3D.h"
// #include "./window/window.h"
// #include <stdint.h>
#include <string.h>
// #include <stdbool.h>


//Header Guard.
#ifndef _GRAPHICS_H
#define _GRAPHICS_H

/// @brief Handle a shaderblock_t *pointer, Handling errors and compiling it with available Shaders,
/// As well as printing the necessary error Messages with printf.
// #define SHADERBLOCK_HANDLE(SB, CLEAN, DO_UNIFORMS)	\
// if(!SB->compiled_[7]){   \
// 	if(vertexshader == NULL || fragmentshader == NULL){ \
// 		printf("Vertex or Fragment Shader not set, resolving.\n");  \
// 		char *default_dir = cwd;    \
// 		strncat(default_dir, "engine/Graphics/Shaders.txt", 30);    \
// 		shaders_pull(default_dir);  \
// 	}   \
// 	if(SB->compiled_[7] == false){SB =shader_compile(CLEAN);}    \
// 	if(DO_UNIFORMS){uniform_init(SB);}	\
// }	\

#define SHADERBLOCK_HANDLE(SB_, CLEAN_, DO_UNIFORMS_)	\
	shaderblock_t *SB = SB_;	\
	bool CLEAN = CLEAN_;	\
	bool DO_UNIFORMS = DO_UNIFORMS_;	\
	SB = shaderblock_handle(CLEAN, DO_UNIFORMS);	\

/*
/*Ensure BO is not NULL \
	if (BO == NULL) { \
		BO = (bufferobj_t*)malloc(sizeof(bufferobj_t)); \
		if (BO == NULL){break;} /* Exit on allocation failure \
		BO->VBO = (GLuint*)malloc(sizeof(GLuint)); \
		if (BO->VBO == NULL){break;} \
		BO->EBO = (GLuint*)malloc(sizeof(GLuint)); \
		if (BO->EBO == NULL){break;} \
		success = true;	\
	}else{success = true;} \
*/

// #define BUFFEROBJECT_HANDLE(BO_, VERTICES_, len_, INDEX_ORDER_, ilen_, DRAW_FORMAT, MAX_) \
// size_t counter = 3;	\
// bool success = false;	\
// bufferobj_t *BO = BO_;	\
// GLfloat *VERTICES = VERTICES_;	\
// size_t len = len_;	\
// GLuint *INDEX_ORDER = INDEX_ORDER_;	\
// size_t *ilen = ilen_;	\
// do { \
// 	if(BO == NULL){break;}	\
// 	success = true;	\
// 	/*VAO not set-up*/ \
// 	if (BO->buffer_[0] == false) { \
// 	if (BO->VAO == 0){glGenVertexArrays(1, &BO->VAO);}	\
// 	glBindVertexArray(BO->VAO); \
// 	} \
// 	/*VBO not set-up*/ \
// 	if(BO->buffer_[1] == false && VERTICES != NULL){	\
// 			if(BO->VBO_len == 0){	\
// 				glGenBuffers(1, BO->VBO); \
// 				BO->VBO_len = 1; \
// 			} \
// 			glBindBuffer(GL_ARRAY_BUFFER, *BO->VBO); \
// 			glBufferData(GL_ARRAY_BUFFER, len * sizeof(GLfloat), VERTICES, DRAW_FORMAT); \
// 	} \
// 	/*EBO not set-up*/ \
// 	if(BO->buffer_[2] == false && INDEX_ORDER != NULL){	\
// 		GLuint* actual_indexes = NULL;  \
// 		bool free_indexes_new = false;	\
// 		if((*ilen) < len / 3){*ilen = 0;}else{*ilen = len / 3;}   \
// 			/*Handle when INDEX_ORDER is NULL, using an array of {1, 2, 3, 4... (len-1)}*/\
// 			if (INDEX_ORDER == NULL || (*ilen) == 0) {	\
// 				*ilen = len / 3;   \
// 				actual_indexes = (GLuint*)malloc((*ilen) * sizeof(GLuint)); \
// 				if (actual_indexes){for (size_t cc = 0; cc < len; ++cc) { actual_indexes[cc] = cc; }}   \
// 				free_indexes_new = true;	\
// 			}else{actual_indexes = INDEX_ORDER;}   \
// 			/*Finish handling EBO*/	\
// 			if (BO->EBO == 0) {glGenBuffers(1, BO->EBO);}   \
// 			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *BO->EBO);    \
// 			glBufferData(GL_ELEMENT_ARRAY_BUFFER, (*ilen) * sizeof(GLuint), actual_indexes, DRAW_FORMAT);   \
// 			if(free_indexes_new){free(actual_indexes);}	\
// 	} \
// }while (counter < MAX_ || success == false);

#define BUFFEROBJECT_HANDLE(BO_, VERTICES_, len_, INDEX_ORDER_, ilen_, DRAW_FORMAT_, MAX_)	\
	bufferobj_t *BO = BO_;	\
	GLfloat *VERTICES = VERTICES_;	\
	size_t len = len_;	\
	GLuint *INDEX_ORDER = INDEX_ORDER_;	\
	size_t ilen = ilen_;	\
	GLenum DRAW_FORMAT = DRAW_FORMAT_;	\
	uint8_t MAX = MAX_;	\
	bufferobject_handle(BO, VERTICES, len, INDEX_ORDER, ilen, DRAW_FORMAT, MAX);	\
	BO_ = BO;	\

typedef struct arrkey{
	GLint Location;
	char* name, * type;
}arrkey;
#define arrk_t arrkey

typedef struct ComputeShaderBlock {
	//Length 8.
	/*
	[0]: Is Shader Program compiled?
	[1]: Is Vertex Shader compiled?
	[2]: Is Fragment Shader compiled?
	[3]: Is Geometry Shader compiled?
	! DEPRECATED, [6]: Is Compute Shader compiled?
	[6]: Is ShaderBlock usable?
	*/
	bool compiled_[7];
	size_t uniform_len;
	/// @brief A List of (size_t, char *) to support uniform access in @ref uniforms.
	arrk_t* uniforms;
	char *vertex, *fragment, *geometry;
	GLuint shaderProgram,
		vertexshader,
		fragmentshader,
		geometryshader
		// *computeShader
		;
}ComputeShaderBlock;
#define shaderblock_t ComputeShaderBlock

// #include <excpt.h> // This is a Windows-specific header and may not be portable.


// // A Function Pointer for doing custom functions with the Polling of the Window.
// // The `poll_do` function signature should be `void (*poll_do)(struct window *, size_t);`
// // to ensure the `window` struct is a known type within the typedef.
// // It's also better to use `const size_t` for the size parameter if it's not modified.
// typedef void (*poll_do)(struct win_t*, size_t);

// /// @brief A function called when the Window should be killed.
// typedef void (*poll_kill)(struct win_t*);

typedef struct scene_header{
	char *name;
}scene_header;
typedef struct scene_type{
	shaderblock_t* shaders;
	size_t shader_num, meshes_num;
	int32_t shader_curr;
	scene_header header;
	mesh_t *meshes;
}scene_type;
#define scene_t scene_type

// Define your function pointer types using the forward-declared struct
typedef void (*poll_do)(void *self, size_t polls, uint32_t pollcycles);
typedef void (*poll_kill)(void* self);

#define wscene_curr(WIN) ((scene_t)WIN->scenes[*WIN->loaded_scenes])
typedef struct window{
	GLFWwindow* g_window;
	char* name;
	poll_do polld;
	poll_kill pollk;
	size_t scene_num, *loaded_scenes;
	scene_t *scenes;
	uint32_t x, y, w, h;
}window;
#define win_t window

typedef struct poll_intermediary{
	win_t *window;
	size_t poll_cycles;
}poll_intermediary;
#define ipoll_t poll_intermediary



// The function pointer parameter should be a pointer to the `poll_do` type, not a pointer to a pointer.
// Also, the return type should be `window *`
win_t* win_init(char* name, GLFWerrorfun error_handle, poll_do poll_do_, poll_kill poll_kill_, uint32_t w, uint32_t h);
void win_poll(win_t *win);
bool win_shouldclose(win_t* win);
void win_kill(win_t* win);

// The function signature for `win_attrblink` is a bit strange, `GLuint` for layout and `GLuint` for `component_num` is fine, but it's not clear what `win_attrblink` is doing. It's likely a typo and should be `glVertexAttribPointer`. I've left the signature as is, but it may require further correction in the implementation file. The `offset` parameter is usually a `const void *`.
// void win_attrblink(window *win, size_t curr_vbo, GLuint layout, GLuint component_num, GLenum type, GLsizeiptr stride, const void *offset);

#define NUM_BUILTIN_TYPES 37


/*
OLD_TYPE_NAME_HASHES:
	0x17a615d41, 0xba24308, {0x3c498b715c77a344d}, 0x3105a2e583,
	0x17a626b175, 0x17a626b176, 0x17a626b177,
	0x310600109a, 0x310600109b, 0x310600109c,
	0x3107b71892, 0x3107b71893, 0x3107b71894,
	0x3105822e17, 0x3105822e18, 0x3105822e19,
	0x17a61e7a59, 0x17a61e7a5a, 0x17a61e7a5,
	0x6513470716c, 0x6513470764c, 0x6513470716d, 0x65134707ba5, 0x6513470764e, 0x65134707ba6,
	0x3616641d8e1837e, 0x3616641d8e183a3, 0x3616641d8e183c4,
	0x3c498327d63654478, //SamplerCube
	0x47781b0f5de1e434f0d364, 0x47781b0f5de1e435a29795,
	0x163479a4714d88e0e6433e, 0x2c98144203a96c279e53412b264,
	0x122485303c7395017, 0x122485303c7395038,
	0x1224905d4f23b7b63, 0x1224905d4f23b7b84};

*/

#define SHADER_INDEX_LEN 4
/*
Shader setting conventions:
	[0 - 4]: OpenGL Version,
	[5]: Core or Compatibility?
	[6 - 10]: Vertex Shader index
	[11 - 15]: Fragment Shader index
	[16 - 20]: Geometry Shader index
	[21 - 25]: Tesselation Eval index
	[26 - 30]: Tess Control index
*/
typedef struct decoded_settings{
	const uint32_t vsindex,
		fsindex,
		gsindex,
		tesindex,
		tcsindex;
	uint32_t version_len;
	char *version;
}decoded_settings;
#define dsetting_t decoded_settings


bool cwd_init();
void shaders_pull(const char* filepath);
void shader_pull(const char *filepath, const bool redo_shaders[5]);
bool shader_error(shaderblock_t* sb_t, const char* type);
char* shadersettings_rw(const char* filepath, char* write);
shaderblock_t* shader_compile(bool delete_shaders_on_link);
void win_draw(win_t *win, mesh_t *_mesh);
void win_flood(win_t *win, const argb_t c);
// void winimage_append(win_t *win, const char *filepath, const argb_t *border_color);
void destroy_arrkey(arrk_t *arrk);
arrk_t *uniform_init(size_t *uniform_len, const GLuint shaderProgram);
bool uniform_write(shaderblock_t* shader, const char* type, const char* name, const void* property_, const bool transpose, void* value, const size_t num_elements);
void uniform_free(shaderblock_t * shader);
void* buffer_bufferdo(bufferobj_t * buffer, const size_t len, const BUFFER_OPTIONS option);
void handle_glfw_error_default(int error_code, const char *msg);
mesh_t *mesh_gen(GLfloat *vertices, GLuint *indices, size_t len[2], uint8_t strides[4], uint32_t layout_indices[3], GLenum format, image_t *texture);
image_t *image_gen(uint8_t color_channels, char *image_path, float border[4], texformat_t format);

shaderblock_t *shaderblock_gen(bool clean, bool do_uniforms);
void shaderblock_handle(shaderblock_t *sb, bool clean, bool do_uniforms);
bufferobj_t *bufferobj_gen(mesh_t *mesh, const GLenum draw_format, const int pos_layout, int col_layout, int tex_layout);
void bufferobject_handle(bufferobj_t *buffer, GLfloat *vertices, size_t v_len, GLuint *index_order, size_t index_len, GLenum draw_format, uint8_t max_tries);

scene_t *scene_load(const char *path);
void scene_draw(scene_t *scene);
bool scene_save(scene_t *scene, char *target_file);
void scene_kill(scene_t *scene, bool save);
void **scene_bufferdo(scene_t *scene, const BUFFER_OPTIONS option);
scene_t *scene_gen(char *name, mesh_t *meshes, shaderblock_t *shaders, size_t len[2]);
#endif