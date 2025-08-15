#pragma once

#include "../Public.h"
//#include "./window/window.h"
#include <stdint.h>
#include <stdbool.h>


//Header Guard.
#ifndef _GRAPHICS_H
#define _GRAPHICS_H

/// @brief Handle a shaderblock_t *pointer, Handling errors and compiling it with available Shaders,
/// As well as printing the necessary error Messages with printf.
#define SHADERBLOCK_HANDLE(SB, CLEAN, DO_UNIFORMS) if(!SB->compiled_[7]){   \
	if(vertexshader == NULL || fragmentshader == NULL){ \
		printf("Vertex or Fragment Shader not set, resolving.\n");  \
		char *default_dir = cwd;    \
		strncat(default_dir, "engine/Graphics/Shaders.txt", 30);    \
		shaders_pull(default_dir);  \
	}   \
	if(SB->compiled_[7] == false){SB =shader_compile(CLEAN);}    \
	if(DO_UNIFORMS){uniform_init(SB);}	\
}	\


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

#define BUFFEROBJECT_HANDLE(BO, VERTICES, len, INDEX_ORDER, ilen, DRAW_FORMAT, MAX_) \
size_t counter = 3;	\
bool success = false;	\
do { \
	if(BO == NULL){break;}	\
	success = true;	\
	/*VAO not set-up*/ \
	if (BO->buffer_[0] == false) { \
	if (BO->VAO == 0){glGenVertexArrays(1, &BO->VAO);}	\
	glBindVertexArray(BO->VAO); \
	} \
	/*VBO not set-up*/ \
	if(BO->buffer_[1] == false && VERTICES != NULL){	\
			if(BO->VBO_len == 0){	\
				glGenBuffers(1, BO->VBO); \
				BO->VBO_len = 1; \
			} \
			glBindBuffer(GL_ARRAY_BUFFER, *BO->VBO); \
			glBufferData(GL_ARRAY_BUFFER, len * sizeof(GLfloat), VERTICES, DRAW_FORMAT); \
	} \
	/*EBO not set-up*/ \
	if(BO->buffer_[2] == false && INDEX_ORDER != NULL){	\
		GLuint* actual_indexes = NULL;  \
		bool free_indexes_new = false;	\
		if((*ilen) < len / 3){*ilen = 0;}else{*ilen = len / 3;}   \
			/*Handle when INDEX_ORDER is NULL, using an array of {1, 2, 3, 4... (len-1)}*/\
			if (INDEX_ORDER == NULL || (*ilen) == 0) {	\
				*ilen = len / 3;   \
				actual_indexes = (GLuint*)malloc((*ilen) * sizeof(GLuint)); \
				if (actual_indexes){for (size_t cc = 0; cc < len; ++cc) { actual_indexes[cc] = cc; }}   \
				free_indexes_new = true;	\
			}else{actual_indexes = (GLuint*)indexes;}   \
			/*Finish handling EBO*/	\
			if (BO->EBO == 0) { glGenBuffers(1, BO->EBO); }   \
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BO->EBO);    \
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, (*ilen) * sizeof(GLuint), actual_indexes, DRAW_FORMAT);   \
			if(free_indexes_new){free(actual_indexes);}	\
	} \
}while (counter < MAX_ || success == false);

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
	! DEPRECATED, [4]: Is Tessellation Control Shader compiled?
	! DEPRECATED, [5]: Is Tesselation Evaluation Shader compiled?
	! DEPRECATED, [6]: Is Compute Shader compiled?
	[7]: Is ShaderBlock usable?
	*/
	bool compiled_[7];
	size_t uniform_len;
	/// @brief A List of (size_t, char *) to support uniform access in @ref uniforms.
	struct arrk_t* uniforms;
	GLuint shaderProgram,
		vertexshader,
		fragmentshader,
		geometryshader
		// *tessellation_controlshader, 
		// *tessellation_evaluationshader, 
		// *computeShader
		;
}ComputeShaderBlock;
#define shaderblock_t ComputeShaderBlock

typedef struct buffer_object {
	/*
	[0]: Is VAO set-up?
	[1]: Is VBO set-up?
	[2]: Is EBO set-up?
	*/
	bool buffer_[3];

	/// @brief The Vertex Array Object for the Window, it contains Multiple States about the Window's OpenGL state.
	/// It describes the VBO and EBO.
	/// @remark VAO is used to store the state of the OpenGL pipeline.
	GLuint VAO;

	/// @brief The number of initialised VBOs.
	size_t VBO_len;
	/// @brief The length of the initialised EBOs.
	size_t EBO_len;
	/// @brief An array of VBO GLuint IDs, for accessing each VBO element.
	/// As in, the Literal GLfloat vertices to be drawn.
	/// Accepts whole Model's, in the format of Vertex arrays.
	/// @remark VBO is used to store the vertex data for the OpenGL pipeline.
	GLuint* VBO;
	/// @brief An array of EBO GLuint IDs, for accessing each one.
	/// @remark The EBO is used to store the index data for the OpenGL pipeline, 
	/// if there is only one the same EBO will be applied to each VBO item.
	/// If EBO_curr isn't equal to VBO_curr, this buffer_oject is considered invalid.
	/// If it is invalid and there are no other valid buffer_objects then the first EBO item will be used.
	/// If there are no EBOs then a default will be attributed.
	GLuint* EBO;
}buffer_object;
#define bufferobj_t buffer_object

typedef struct texture_image {
	int width, height, color_channels;
	GLuint ID;
	unsigned char* img;
}texture_image;
#define image_t texture_image

typedef enum BUFFER_OPTIONS {
	//For single buffer objects.
	BUFFER_OPTIONS_CLEAR = 0,
	//For single buffer objects.
	BUFFER_OPTIONS_CLEAR_VBO = 1,
	//For single buffer objects.
	BUFFER_OPTIONS_CLEAR_EBO = 2,
	//For single buffer objects.
	BUFFER_OPTIONS_FREE = 3,

	//For multiple buffer objects.
	BUFFER_OPTIONS_FREE_ALL = 4,
	//For multiple buffer objects.
	BUFFER_OPTIONS_CLEAR_VAO = 5,
	//For multiple buffer objects.
	BUFFER_OPTIONS_COLLECT_VAO = 6,
	//For multiple buffer objects.
	BUFFER_OPTIONS_COLLECT_VBO_MULTIPLE = 7,
	//For multiple buffer objects.
	BUFFER_OPTIONS_COLLECT_EBO_MULTIPLE = 8,
}BUFFER_OPTIONS;


// #include <excpt.h> // This is a Windows-specific header and may not be portable.


// // A Function Pointer for doing custom functions with the Polling of the Window.
// // The `poll_do` function signature should be `void (*poll_do)(struct window *, size_t);`
// // to ensure the `window` struct is a known type within the typedef.
// // It's also better to use `const size_t` for the size parameter if it's not modified.
// typedef void (*poll_do)(struct win_t*, size_t);

// /// @brief A function called when the Window should be killed.
// typedef void (*poll_kill)(struct win_t*);

typedef struct window window;

#define win_t window

// Define your function pointer types using the forward-declared struct
typedef void (*poll_do)(window*, size_t);
typedef void (*poll_kill)(window*);

typedef struct window {
	GLFWwindow* g_window;
	char* name;
	poll_do polld;
	poll_kill pollk;
	shaderblock_t* shaders;
	size_t textures_len, textures_curr, vert_count;
	
	size_t buffer_len, buffer_curr;
	bufferobj_t* buffers;
	image_t* textures;
	uint32_t x, y, w, h;
};

// It is common practice to define the structs before their aliases.
typedef struct Color4 {
	// @brief a: 1.0f := Solid, 0.0f := Transparent.
	float a, r, g, b;
} Color4;
#define argb_t Color4


// The function pointer parameter should be a pointer to the `poll_do` type, not a pointer to a pointer.
// Also, the return type should be `window *`
win_t* win_init(char* name, void (*poll_do)(win_t*, size_t), void (*poll_kill)(win_t*), uint32_t w, uint32_t h);
void win_poll(win_t* win);
bool win_shouldclose(win_t* win);
void win_kill(win_t* win);

// The function signature for `win_attrblink` is a bit strange, `GLuint` for layout and `GLuint` for `component_num` is fine, but it's not clear what `win_attrblink` is doing. It's likely a typo and should be `glVertexAttribPointer`. I've left the signature as is, but it may require further correction in the implementation file. The `offset` parameter is usually a `const void *`.
void win_attrblink(window* win, GLuint layout, GLuint component_num, GLenum type, GLsizeiptr stride, const void* offset);




char* cwd;
#define MAX_PATHLENGTH 1024* 1024

//Length: 53
const char* vertexshader_default =
	"#version 330 core\n"
	"#define vs\n"
	"layout(location = 0) in vec3 position;\n"
	"void main(){\n"
	"    gl_Position = vec4(position, 1.0);\n"
	"}\n"
	"#shaderend\n\0";

//Length: 54
const char* fragmentshader_default =
	"#version 330 core\n"
	"#define fs\n"
	"out vec4 color;\n"
	"void main(){\n"
	"    color = vec4(1.0, 0.0, 0.0, 1.0);\n"
	"}\n"
	"#shaderend\n\0";

/// @brief The Constant Shader keywords for Understanding Shader text.
const char vs_start[10] = "#define vs",
	fs_start[10] = "#define fs",
	gs_start[10] = "#define gs",
	// tcs_start[10]= "#define tc", 
	// tes_start[10]= "#define te", 
	// cs_start[10]= "#define cs", 
	shader_end[10] = "#shaderend";

size_t len_typenames = 36;
/// @brief Shader RAWs in case any part of the Program needs them.
char* vertexshader = NULL,
	* fragmentshader = NULL,
	* geometryshader = NULL,
	** shader_typenames = NULL
	/* *tessellation_controlshader,
	*tessellation_evaluationshader,
	*computeshader*/;

static const char* builtin_shader_typenames[] = {
	"bool", "int", "unsignedint", "float", "vec2", "vec3", "vec4",
	"ivec2", "ivec3", "ivec4", "uvec2", "uvec3", "uvec4",
	"bvec2", "bvec3", "bvec4", "mat2", "mat3", "mat4",
	"mat2x3", "mat3x2", "mat2x4", "mat4x2", "mat3x4", "mat4x3",
	"sampler1D", "sampler2D", "sampler3D", "samplerCube",
	"sampler1DShadow", "sampler2DShadow", "sampler2DArray",
	"sampler2DArrayShadow", "isampler1D", "isampler2D",
	"usampler1D", "usampler2D" };


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


// extern typedef uint128_t;
static const uint128_t builtin_shader_typehash[37] = {
	{0, 3646476,}, {0, 118091}, {0, 184466353937349512}, {0, 124969334},
	{0, 4353872}, {0, 4353873},  {0, 4353874},
	{0, 128875577},  {0, 128875578},  {0, 128875579},
	{0, 143106629},  {0, 143106630},  {0, 143106631},
	{0, 120574130},  {0, 120574131},  {0, 120574132},
	{0, 4026644},  {0, 4026645},  {0, 4026646},
	{0, 4385019327},  {0, 4385020415},  {0, 4385019328},  {0, 4385021504},  {0, 4385020417},  {0, 4385021505},
	{0, 166016265074057},  {0, 166016265074090},  {0, 166016265074123},
	{0, 180791712666351635}, //SamplerCube
	{0, 16629051508994671055U},  {0, 16629051551613114032U},
	{0, 3857864121005407689},  {0, 12598728139851495951U},
	{0, 5039222127279122},  {0, 5039222127279155},
	{0, 5596159940102558},  {0, 5596159940102591} };

const size_t NUM_BUILTIN_TYPES = sizeof(builtin_shader_typenames) / sizeof(builtin_shader_typenames[0]);



bool cwd_init();
void shaders_pull(const char* filepath);
char* shadersettings_rw(const char* filepath, char* write);
shaderblock_t* shader_compile(bool delete_shaders_on_link);
void win_draw(win_t *win, const GLfloat *points, size_t len, GLuint *indexes, size_t ilen);
void win_flood(win_t *win, const argb_t c);
void winimage_append(win_t *win, const char *filepath, const argb_t *border_color);
void destroy_arrkey(arrk_t *arrk);
void uniform_init(shaderblock_t * sb_t);
bool uniform_write(shaderblock_t* shader, const char* type, const char* name, const char* property, bool transpose, const void* value, size_t num_elements);
void uniform_free(shaderblock_t * shader);
void* buffer_bufferdo(bufferobj_t * buffer, const size_t len, const BUFFER_OPTIONS option);
bufferobj_t* win_buffercurr(win_t * win);

#endif