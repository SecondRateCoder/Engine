#include <Public.h>
#include <stdbool.h>
#include <Window.h>


//Header Guard.
#ifndef _DP_H
#define _DP_H



#define shaderblock_t ComputeShaderBlock
#define arrk_t arrkey
#define image_t texture_image

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
        uniform_init(SB);
    }    \
}

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
    arrk_t *uniforms;
    GLuint shaderProgram, 
        vertexshader, 
        fragmentshader, 
        geometryshader,
        // *tessellation_controlshader, 
        // *tessellation_evaluationshader, 
        // *computeShader
        ;
}ComputeShaderBlock;

typedef struct arrkey{
    GLint ID;
    char *name, *type;
}arrkey;

typedef struct texture_image{
    int width, height;
    GLuint ID;
    unsigned char *img;
}texture_image;


char *cwd;
#define MAX_PATHLENGTH 1024* 1024
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

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
    "bool", "int", "float", "vec2", "vec3", "vec4",
    "ivec2", "ivec3", "ivec4", "uvec2", "uvec3", "uvec4",
    "bvec2", "bvec3", "bvec4", "mat2", "mat3", "mat4",
    "mat2x3", "mat3x2", "mat2x4", "mat4x2", "mat3x4", "mat4x3",
    "sampler1D", "sampler2D", "sampler3D", "samplerCube",
    "sampler1DShadow", "sampler2DShadow", "sampler2DArray",
    "sampler2DArrayShadow", "isampler1D", "isampler2D",
    "usampler1D", "usampler2D"
};
static const size_t builtin_shader_typehash[] = {
    str_hash("bool"), str_hash("int"), str_hash("unsignedint"), str_hash("float"), str_hash("vec2"), str_hash("vec3"), 
    str_hash("vec4"), str_hash("ivec2"), str_hash("ivec3"), str_hash("ivec4"), str_hash("uvec2"), 
    str_hash("uvec3"), str_hash("uvec4"), str_hash("bvec2"), str_hash("bvec3"), str_hash("bvec4"), 
    str_hash("mat2"), str_hash("mat3"), str_hash("mat4"), str_hash("mat2x3"), str_hash("mat3x2"), 
    str_hash("mat2x4"), str_hash("mat4x2"), str_hash("mat3x4"), str_hash("mat4x3"), str_hash("sampler1D"), 
    str_hash("sampler2D"), str_hash("sampler3D"), str_hash("samplerCube"), str_hash("sampler1DShadow"), str_hash("sampler2DShadow"), 
    str_hash("sampler2DArray"), str_hash("sampler2DArrayShadow"), str_hash("isampler1D"), str_hash("isampler2D"), str_hash("usampler1D"), 
    str_hash("usampler2D")
};

const size_t NUM_BUILTIN_TYPES = sizeof(builtin_shader_typenames) / sizeof(builtin_shader_typenames[0]);



bool cwd_init();
void shaders_pull(char *filepath);
char *shadersettings_rw(char *filepath, char *write);
shaderblock_t *shader_compile(bool delete_shaders_on_link);
void win_draw(win_t *win, GLfloat *points, size_t len, GLuint *indexes, size_t ilen);
void win_flood(win_t *win, const argb_t c);
void winimage_append(win_t *win, int image_width, int image_height, int colorch_num, char *filepath, argb_t *border_color);
arrk_t create_arrkey(GLuint id, const char* name, const char* type);
void destroy_arrkey(arrk_t key);
void uniform_init(shaderblock_t *sb_t);
#endif