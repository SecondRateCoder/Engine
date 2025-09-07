#include "./Public.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// This file will contain all the Public types e.t.c



char* str_normalise(const char *str, bool handle_spaces, bool handle_upper) {
    if (!str) return NULL;

    size_t str_len = strlen(str);
    char *out = malloc(str_len + 1);
    if (!out) return NULL;

    size_t i = 0, j = 0;
    while (str[i] != '\0') {
        char c = str[i];
        if (handle_spaces && IS_SPACE(c)) {
            i++;
            continue;
        }

        if(handle_upper){c = (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;}
        out[j++] = c;
        i++;
    }

    out[j] = '\0';
    return out;
}

#define HASH_64BIT_LIMIT 12

/// @brief Return a __uint128_t variable.
/// @param str The string to be hashed.
/// @return A __uint128_t value.
size_t* str_hash(const char *str) {
    if(!str){return NULL;}
    size_t *hash = malloc(sizeof(uint128_t));
    if(!hash){return NULL;}
    hash[0] = 5381;
    hash[1] = 0;
    size_t cc = 0;
    int c;
    while((c = *str++)){
        ++cc;
        if(cc > HASH_64BIT_LIMIT){hash[1] = ((hash[1] << 5) + hash[1]) + c;
        }else{hash[0] = ((hash[0] << 5) + hash[0]) + c;}
    }
    return hash;
}

/**
 * @brief Converts an entire string to lowercase.
 *
 * @param str A pointer to the null-terminated string to be converted.
 */
void str_tolower(char *str) {
    // Iterate through the string until the null terminator is reached.
    for (int i = 0; str[i] != '\0'; i++) {
        // Use the my_tolower function to convert each character.
        str[i] = tolower(str[i]);
    }
}

bool uint128_t_comps(const uint128_t a, const size_t b){ return a[1] == b; }
bool uint128_t_comp(const uint128_t a, const uint128_t b){
    return a[0] == b[0] && a[1] == b[1];
}

void CheckGLError(const char* file, int line, const char* call){
    GLenum err_check;
    while ((err_check = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, ANSI_RED("\n[OpenGL Error] (%x): %s at %s:%d\n"), err_check, call, file, line);
    }
}

/// @brief Configure and Link a Model's VBO and EBO to the inputted bufferobj_t object.
/// @param buffer The Buffer object to be written to.
/// @param shader The shaderblock_t containing the compiled code the model will be linked to.
/// @param pos_layout The layout index to apply a reference to the model's position data.
/// @param color_layout The layout index to apply a reference to the model's color data.
/// @param texture_layout The layout index to apply a reference to the model's texture co-ordinate data.
/// @param _mesh The _mesh to be appended.
void mesh_attrlink(bufferobj_t *buffer, const int pos_layout,  const int col_layout,  const int tex_layout, mesh_t *_mesh){
    //Generate new buffers for _mesh.
    if(glIsVertexArray(buffer->VAO) == GL_FALSE){GLCall(glGenBuffer(&buffer->VAO))}
    if(buffer->VBO == GL_FALSE){GLCall(glGenBuffers(1, &buffer->VBO));}
    if(buffer->EBO == GL_FALSE){GLCall(glGenBuffers(1, &buffer->EBO));}
    GLCall(glBindVertexArray(buffer->VAO));
    
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer->VBO));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->EBO));
    // draw_debug_trace(__FILE__, __LINE__);

    GLCall(glBufferData(GL_ARRAY_BUFFER, _mesh->data_len, _mesh->mesh_data, GL_STATIC_DRAW));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, _mesh->index_len, _mesh->vertex_index, GL_STATIC_DRAW));
    // draw_debug_trace(__FILE__, __LINE__);

    size_t pos_offset = 0;
    size_t color_offset = sizeof(GLfloat) * _mesh->vertex_stride;
    size_t tex_offset = color_offset + sizeof(GLfloat) * _mesh->color_stride;
    mesh->pos_layoutindex = pos_layout;
    mesh->color_layoutindex = col_layout;
    mesh->local_texcoordinates_layoutindex = tex_layout;

    const size_t stride = sizeof(GLfloat) * (_mesh->vertex_stride + _mesh->color_stride + _mesh->dpi_stride);
    if(pos_layout != INT32_MIN){
        GLCall(glVertexAttribPointer(_mesh->pos_layoutindex, _mesh->vertex_stride, GL_FLOAT, GL_FALSE, stride, (void*)pos_offset));
        GLCall(glEnableVertexAttribArray(_mesh->pos_layoutindex));
        // draw_debug_trace(__FILE__, __LINE__);
    }

    if(col_layout != INT32_MIN){
        GLCall(glVertexAttribPointer(_mesh->color_layoutindex, _mesh->color_stride, GL_FLOAT, GL_FALSE, stride, (void*)color_offset));
        GLCall(glEnableVertexAttribArray(_mesh->color_layoutindex));
        // draw_debug_trace(__FILE__, __LINE__);
    }

    if(tex_layout != INT32_MIN){
        GLCall(glVertexAttribPointer(_mesh->local_texcoordinates_layoutindex, _mesh->dpi_stride, GL_FLOAT, GL_FALSE, stride, (void*)tex_offset));
        GLCall(glEnableVertexAttribArray(_mesh->local_texcoordinates_layoutindex));
        // draw_debug_trace(__FILE__, __LINE__);
    }

    draw_debug_trace(__FILE__, __LINE__);
    GLCall(glBindVertexArray(0));
}

void mesh_attrlinkf(bufferobj_t *buffer, mesh_t *_mesh, const size_t buffer_index){
    GLCall(glBindVertexArray(buffer->VAO));
    
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer->VBO));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->EBO));
    draw_debug_trace(__FILE__, __LINE__);

    GLCall(glBufferData(GL_ARRAY_BUFFER, _mesh->data_len, _mesh->mesh_data, GL_STATIC_DRAW));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, _mesh->index_len, _mesh->vertex_index, GL_STATIC_DRAW));
    draw_debug_trace(__FILE__, __LINE__);

    size_t pos_offset = 0;
    size_t color_offset = sizeof(GLfloat) * _mesh->vertex_stride;
    size_t tex_offset = color_offset + sizeof(GLfloat) * _mesh->color_stride;

    const size_t stride = sizeof(GLfloat) * (_mesh->vertex_stride + _mesh->color_stride + _mesh->dpi_stride);
    GLCall(glVertexAttribPointer(_mesh->pos_layoutindex, _mesh->vertex_stride, GL_FLOAT, GL_FALSE, stride, (void*)pos_offset));
    GLCall(glEnableVertexAttribArray(_mesh->pos_layoutindex));
    draw_debug_trace(__FILE__, __LINE__);

    GLCall(glVertexAttribPointer(_mesh->color_layoutindex, _mesh->color_stride, GL_FLOAT, GL_FALSE, stride, (void*)color_offset));
    GLCall(glEnableVertexAttribArray(_mesh->color_layoutindex));
    draw_debug_trace(__FILE__, __LINE__);

    GLCall(glVertexAttribPointer(_mesh->local_texcoordinates_layoutindex, _mesh->dpi_stride, GL_FLOAT, GL_FALSE, stride, (void*)tex_offset));
    GLCall(glEnableVertexAttribArray(_mesh->local_texcoordinates_layoutindex));
    draw_debug_trace(__FILE__, __LINE__);

    GLCall(glBindVertexArray(0));
}

// void mesh_arttr_relink(const mesh_t *_mesh){
//     GLCall(glVertexAttribPointer(_mesh->pos_layoutindex, _mesh->data_len, GL_FLOAT, GL_FALSE, _mesh->vertex_stride, _mesh->mesh_data));
//     GLCall(glEnableVertexAttribArray(_mesh->pos_layoutindex));
//     // Handle color layout.
//     GLCall(glVertexAttribPointer(_mesh->color_layoutindex, _mesh->data_len, GL_FLOAT, GL_FALSE, _mesh->color_stride, &_mesh->mesh_data[_mesh->vertex_stride-1]));
//     GLCall(glEnableVertexAttribArray(_mesh->color_layoutindex));
//     // Handle texture coord layout.
//     GLCall(glVertexAttribPointer(_mesh->local_texcoordinates_layoutindex, _mesh->data_len, GL_FLOAT, GL_FALSE, _mesh->dpi_stride, &_mesh->mesh_data[(_mesh->vertex_stride + _mesh->color_stride) - 2]));
// 	GLCall(glEnableVertexAttribArray(_mesh->local_texcoordinates_layoutindex));
// }

size_t texture_num = 0;

void mesh_bindtexture(mesh_t *m, image_t *texture){
    //Handle texture.
    if(memsize(texture->img) == 0){return;}
    if(glIsTexture(texture->ID) == GL_FALSE){
        GLCall(glGenTextures(1, &texture->ID));
        if(glIsTexture(texture->ID) == GL_TRUE){
            texture->unit = texture_num++;
            GLCall(glActiveTexture(GL_TEXTURE0 + texture->unit));
            GLCall(glBindTexture(texture->format->target, texture->ID));
        }
    }
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
	GLCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, texture->border));
    
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	
    const texformat_t *temp = texture->format;
    switch(temp->target){
        case GL_TEXTURE_1D:
        case GL_TEXTURE_1D_ARRAY:
            GLCall(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
            GLCall(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
            GLCall(glTexParameterfv(GL_TEXTURE_1D, GL_TEXTURE_BORDER_COLOR, texture->border));
            
            GLCall(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
            GLCall(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            GLCall(glTexImage1D(GL_TEXTURE_1D, temp->level, temp->internalFormat, texture->width, 0, temp->pixel_format, temp->pixel_type, texture->img));
            GLCall(glGenerateMipmap(GL_TEXTURE_1D));
            break;
        case GL_TEXTURE_2D:
        case GL_TEXTURE_2D_ARRAY:
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
            GLCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, texture->border));
            
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
            GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            GLCall(glTexImage2D(GL_TEXTURE_2D, temp->level, temp->internalFormat, texture->width, texture->height, 0, temp->pixel_format, temp->pixel_type, texture->img));
            GLCall(glGenerateMipmap(GL_TEXTURE_2D));
            break;
        case GL_TEXTURE_3D:
            GLCall(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
            GLCall(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
            GLCall(glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, texture->border));
            
            GLCall(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
            GLCall(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            GLCall(glTexImage3D(GL_TEXTURE_3D, temp->level, temp->internalFormat, texture->width, texture->height, temp->depth, 0, temp->pixel_format, temp->pixel_type, texture->img));
            GLCall(glGenerateMipmap(GL_TEXTURE_3D));
            break;
        default:
            GLCall(glDeleteTextures(1, &texture->ID));
            texture->ID=0;
            return;
    }
    m->texture = texture;
    return;
}



#ifdef _WIN32
    #include <direct.h>
    #define getcwd _getcwd(NULL, 0)
    #define PATH_SEPARATOR '\\'
#else
    #include <unistd.h>
    #define PATH_SEPARATOR '/'
#endif

#define MAX_ATTEMPTS 10
#define TARGET_RELATIVE_PATH "\\Resources\\Shaders\\Shaders.txt"

char *cwd = NULL;
size_t cwd_len = 0;

bool cwd_init() {
    FILE *temp_ = NULL;
    int attempts = 0;
    if((cwd = getcwd) == NULL){
        printf(ANSI_RED("getcwd() Error, cwd not initialized.\n"));
        return false;
    }

    while(attempts < MAX_ATTEMPTS){
        cwd_len = strlen(cwd);

        // Build full path to target file
        size_t full_len = cwd_len + strlen(TARGET_RELATIVE_PATH) + 1;
        char *full_path = malloc(full_len);
        if (!full_path){break;}

        snprintf(full_path, full_len, "%s%s", cwd, TARGET_RELATIVE_PATH);
        temp_ = fopen(full_path, "r");
        free(full_path);

        if (temp_){
            fclose(temp_);
            printf(ANSI_GREEN("%s"), cwd);
            return true;
        }

        // Move up one directory
        while (cwd_len > 0 && cwd[cwd_len - 1] != PATH_SEPARATOR){--cwd_len;}

        if (cwd_len == 0){break;}

        cwd[cwd_len - 1] = '\0'; // Truncate path
        cwd = realloc(cwd, cwd_len);
        if(!cwd){break;}
        ++attempts;
    }

    printf(ANSI_RED("Failed to locate target file after %d attempts.\n"), attempts);
    return false;
}

void _GLUseprogram(GLuint prog){
    do{
        GLint current;
        glGetIntegerv(GL_CURRENT_PROGRAM, &current);
        if (current != prog || current == 0){
            glUseProgram(prog);
            GLenum err = glGetError();
            if(err != GL_NO_ERROR){
                fprintf(stderr, "[OpenGL Error] (%d): glUseProgram(%u)\n", err, prog);
            }
        }
    }while(0);
}

void draw_debug_trace(const char* file, int line) {
    GLint currentProgram = 0;
    GLint boundVAO = 0;
    GLint boundArrayBuffer = 0;
    GLint boundElementBuffer = 0;

    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &boundVAO);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &boundArrayBuffer);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &boundElementBuffer);

    fprintf(stderr, ANSI_YELLOW(
        "\n[Draw Debug Trace] @ %s:%d\n"
        "  Shader Program: %d\n"
        "  VAO Bound     : %d\n"
        "  VBO Bound     : %d\n"
        "  EBO Bound     : %d\n"
    ), 
    file, line, currentProgram, boundVAO, boundArrayBuffer, boundElementBuffer);

    // Optional: Check for active attributes
    GLint maxAttribs = 0;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttribs);
    for (int i = 0; i < maxAttribs; ++i) {
        GLint enabled = 0;
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
        if (enabled) {
            fprintf(stderr, ANSI_YELLOW("  Attribute %d: ENABLED\n"), i);
        }
    }

    GLenum err_debug = glGetError();
    if (err_debug != GL_NO_ERROR) {
        fprintf(stderr, ANSI_RED("  OpenGL Error Before Draw: 0x%x\n"), err_debug);
    }
}


//! Use later
// const size_t cwd_resource_paths_len = 1;
// const size_t max_cwd_resolution_attempts = 3;
// const char *cwd_resource_paths[] = {
//     // "engine\\Libraries"/*Change to just Libraries*/,
//     "glfw3.dll"
// };

const char* builtin_shader_typenames[37] = {
	"bool", "int", "unsignedint", "float", "vec2", "vec3", "vec4",
	"ivec2", "ivec3", "ivec4", "uvec2", "uvec3", "uvec4",
	"bvec2", "bvec3", "bvec4", "mat2", "mat3", "mat4",
	"mat2x3", "mat3x2", "mat2x4", "mat4x2", "mat3x4", "mat4x3",
	"sampler1D", "sampler2D", "sampler3D", "samplerCube",
	"sampler1DShadow", "sampler2DShadow", "sampler2DArray",
	"sampler2DArrayShadow", "isampler1D", "isampler2D",
	"usampler1D", "usampler2D"
};
const char vs_start[10] = "#define vs",
	fs_start[10] = "#define fs",
	gs_start[10] = "#define gs",
	tcs_start[10]= "#define tc", 
	tes_start[10]= "#define te", 
	cs_start[10]= "#define cs", 
	shader_end[18] = "#define shaderend"
;
const char vertexshader_default[142] =
	"#version 330 core\n"
	"#define vs\n"
	"layout(location = 0) in vec3 position;\n"
	"void main(){\n"
	"    gl_Position = vec4(position, 1.0);\n"
	"}\n"
	"#define shaderend\n\0"
;
const char fragmentshader_default[118] =
	"#version 330 core\n"
	"#define fs\n"
	"out vec4 color;\n"
	"void main(){\n"
	"    color = vec4(1.0, 0.0, 0.0, 1.0);\n"
	"}\n"
	"#define shaderend\n\0"
;
size_t len_typenames = 36;

const unsigned int settings_len = 26;

char* vertexshader = NULL,
	* fragmentshader = NULL,
	* geometryshader = NULL,
	*tessellation_controlshader = NULL,
	*tessellation_evaluationshader = NULL,
	** shader_typenames = NULL
	/**computeshader*/
;

void GLAPIENTRY debug_callback(GLenum source, GLenum type, GLuint id,
                               GLenum severity, GLsizei length,
                               const GLchar *message, const void *userParam){
    fprintf(stderr, ANSI_YELLOW(
        "GL DEBUG:"
            "\tMessage: %s\n\n"
            "\tsource: %s\n"
            "\ttype: %d\n"
            "\tid: %d\n"
            "\tseverity: %d\n"
            "\tlength: %zu\n"
            "\tuserParam: %s\n" )
        , message, source, type, id, severity, length, message, userParam);
}
