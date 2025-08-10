#pragma once

#include "Public.h"
#include "window.h"
#include <stdint.h>
#include <stdbool.h>


//Header Guard.
#ifndef _DP_H
#define _DP_H


/// @brief Handle a shaderblock_t *pointer, Handling errors and compiling it with available Shaders,
/// As well as printing the necessary error Messages with printf.
#define SHADERBLOCK_HANDLE(SB, CLEAN) if(!SB->compiled_[7]){   \
    if(vertexshader == NULL || fragmentshader == NULL){ \
        printf("Vertex or Fragment Shader not set, resolving.\n");  \
        char *default_dir = cwd;    \
        strncat(default_dir, "engine/Graphics/Shaders.txt", 30);    \
        shaders_pull(default_dir);  \
    }   \
    if(SB->compiled_[7] == false){  \
        SB =shader_compile(CLEAN); \
        uniform_init(SB);   \
    }    \
}

#define BUFFEROBJECT_HANDLE(BO, VERTICES, len, INDEX_ORDER, ilen, DRAW_FORMAT, MAX_) \
size_t counter = 3;	\
bool success = false;	\
do { \
    /*Ensure BO is not NULL*/ \
    if (BO == NULL) { \
        BO = (bufferobj_t*)malloc(sizeof(bufferobj_t)); \
        if (BO == NULL){break;} /* Exit on allocation failure */ \
        BO->VBO = (GLuint*)malloc(sizeof(GLuint)); \
        if (BO->VBO == NULL){break;} \
        BO->EBO = (GLuint*)malloc(sizeof(GLuint)); \
        if (BO->EBO == NULL){break;} \
		success = true;	\
    }else{success = true;} \
    /*VAO not set-up*/ \
    if (BO->buffer_[0] == false) { \
        if (BO->VAO == 0) { glGenVertexArrays(1, &BO->VAO); } \
        glBindVertexArray(BO->VAO); \
    } \
    /*VBO not set-up*/ \
    if (BO->buffer_[1] == false && VERTICES != NULL) { \
        if (BO->VBO_len == 0) { \
            glGenBuffers(1, BO->VBO); \
            BO->VBO_len = 1; \
        } \
        glBindBuffer(GL_ARRAY_BUFFER, *BO->VBO); \
        glBufferData(GL_ARRAY_BUFFER, len * sizeof(GLfloat), VERTICES, DRAW_FORMAT); \
    } \
    /*EBO not set-up*/ \
    if (BO->buffer_[2] == false && INDEX_ORDER != NULL) { \
        GLuint* actual_indexes = NULL;  \
		bool free_indexes_new = false;	\
        if((*ilen) < len/3){*ilen = 0;}else{*ilen = len/3;}   \
        /*Handle when INDEX_ORDER is NULL, using an array of {1, 2, 3, 4... (len-1)}*/\
        if(INDEX_ORDER == NULL || (*ilen) == 0){ \
            *ilen = len/3;   \
            actual_indexes = (GLuint*)malloc((*ilen)* sizeof(GLuint)); \
            if(actual_indexes){   \
                for(size_t cc = 0; cc < len; ++cc){actual_indexes[cc] = cc;}   \
            }   \
			free_indexes_new = true;	\
        }else{actual_indexes = (GLuint*)indexes;}   \
        /*Finish handling EBO*/\
        if(BO->EBO == 0){glGenBuffers(1, BO->EBO);}   \
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BO->EBO);    \
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (*ilen) * sizeof(GLuint), actual_indexes, DRAW_FORMAT);   \
		if(free_indexes_new){free(actual_indexes);}\
    } \
} while(counter < MAX_ || success == false)

typedef struct arrkey{
    GLint Location;
    char *name, *type;
}arrkey;
#define arrk_t arrkey

typedef struct ComputeShaderBlock{
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
    struct arrk_t *uniforms;
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

typedef struct buffer_object{
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
    GLuint *VBO;
    /// @brief An array of EBO GLuint IDs, for accessing each one.
    /// @remark The EBO is used to store the index data for the OpenGL pipeline, 
    /// if there is only one the same EBO will be applied to each VBO item.
    /// If EBO_curr isn't equal to VBO_curr, this buffer_oject is considered invalid.
    /// If it is invalid and there are no other valid buffer_objects then the first EBO item will be used.
    /// If there are no EBOs then a default will be attributed.
    GLuint *EBO;
}buffer_object;
#define bufferobj_t buffer_object

typedef struct texture_image{
    int width, height, color_channels;
    GLuint ID;
    unsigned char *img;
}texture_image;
#define image_t texture_image

typedef enum BUFFER_OPTIONS{
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

char *cwd;
#define MAX_PATHLENGTH 1024* 1024

//Length: 53
const char *vertexshader_default = 
"#version 330 core\n"
"#define vs\n"
"layout(location = 0) in vec3 position;\n"
"void main(){\n"
"    gl_Position = vec4(position, 1.0);\n"
"}\n"
"#shaderend\n\0";

//Length: 54
const char *fragmentshader_default = 
"#version 330 core\n"
"#define fs\n"
"out vec4 color;\n"
"void main(){\n"
"    color = vec4(1.0, 0.0, 0.0, 1.0);\n"
"}\n"
"#shaderend\n\0";

/// @brief The Constant Shader keywords for Understanding Shader text.
const char vs_start[10]= "#define vs", 
	fs_start[10]= "#define fs", 
	gs_start[10]= "#define gs", 
	// tcs_start[10]= "#define tc", 
	// tes_start[10]= "#define te", 
	// cs_start[10]= "#define cs", 
	shader_end[10]= "#shaderend";

size_t len_typenames =36;
/// @brief Shader RAWs in case any part of the Program needs them.
char *vertexshader = NULL, 
	*fragmentshader = NULL, 
	*geometryshader = NULL, 
    **shader_typenames = NULL
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
    "usampler1D", "usampler2D"
};

static const uint128_t builtin_shader_typehash[] = {
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

const size_t NUM_BUILTIN_TYPES = sizeof(builtin_shader_typenames) / sizeof(builtin_shader_typenames[0]);



bool cwd_init();
void shaders_pull(const char* filepath);
char* shadersettings_rw(const char* filepath, char* write);
shaderblock_t *shader_compile(bool delete_shaders_on_link);
void win_draw(win_t *win, const GLfloat *points, size_t len, GLuint *indexes, size_t ilen);
void win_flood(win_t *win, const argb_t c);
void winimage_append(win_t *win, const char* filepath, const argb_t *border_color);
void destroy_arrkey(arrk_t *arrk);
void uniform_init(shaderblock_t *sb_t);
void uniform_write(shaderblock_t* shader, const char* type, const char* name, const char* property, bool* transpose, const void* value, size_t num_elements);
void uniform_free(shaderblock_t* shader);
void *buffer_bufferdo(bufferobj_t* buffer, const size_t len, const BUFFER_OPTIONS option);
bufferobj_t *win_buffercurr(win_t *win);

#endif