#include "../engine/Public.h"

/// @brief Convert a full string with the input flags.
/// @param str The input string.
/// @param handle_spaces Remove spaces.
/// @param handle_upper Convert to upper-case.
/// @return The processed string, of same length as the unput string.
char* str_normalise(const char *str, bool handle_spaces, bool handle_upper) {
    if (!str) return NULL;

    size_t str_len = strlen(str);
    char *out = calloc(str_len + 1, sizeof(char));
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
    size_t *hash = calloc(1, sizeof(uint128_t));
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

/// @brief Compare a uint128_t type and a size_t type.
/// @param a 1st uint128_t types.
/// @param b 1st size_t types.
/// @return True if a[1](lower) and b are the same.
bool uint128_t_comps(const uint128_t a, const size_t b){return a[1] == b;}

/// @brief Compare a uint128_t type and a uint128_t type.
/// @param a 1st uint128_t types.
/// @param b 1st uint128_t types.
/// @return True if the same.
bool uint128_t_comp(const uint128_t a, const uint128_t b){
    return a[0] == b[0] && a[1] == b[1];
}

/// @brief Check for an OpenGL error and print message if there was an error.
/// @param file The file that triggered the error.
/// @param line The line the error was triggered on.
/// @param call The call called.
void CheckGLError(const char* file, int line, const char* call){
    GLenum err_check;
    while ((err_check = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, ANSI_RED("\n[OpenGL Error] (%x): %s at %s:%d\n"), err_check, call, file, line);
    }
}

void debug_vert_attr(uint32_t index){
    GLint enabled = 0, size = 0, type = 0, normalized = 0, stride_ = 0;
    GLvoid* pointer = NULL;
#ifdef _DEBUG_
        glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
        glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size);
        glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_TYPE, &type);
        glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &normalized);
        glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride_);
        glGetVertexAttribPointerv(index, GL_VERTEX_ATTRIB_ARRAY_POINTER, &pointer);
        printf(
            ANSI_YELLOW("\nAttribute params: "
            "\n\tEnabled: %s"
            "\n\tSize: %d"
            "\n\tType: %d"
            "\n\tNormalised: %s"
            "\n\tStride: %d"
            "\n\tPointer: %zu"),
            (enabled == GL_TRUE? "TRUE": "FALSE"),
            size, type, (normalized == GL_TRUE? "TRUE": "FALSE"),
            stride_, (size_t)pointer
        );
#endif
}

/// @brief Configure and Link a Model's VBO and EBO to the inputted bufferobj_t object.
/// @param buffer The Buffer object to be written to.
/// @param shader The shaderblock_t containing the compiled code the model will be linked to.
/// @param pos_layout The layout index to apply a reference to the model's position data.
/// @param color_layout The layout index to apply a reference to the model's color data.
/// @param texture_layout The layout index to apply a reference to the model's texture co-ordinate data.
/// @param _mesh The _mesh to be appended.
void mesh_attrlink(bufferobj_t *buffer, mesh_t *_mesh){
	// glVertexAttribPointer:
	//		Params:
	//			[0]: The layout in shaders,
	//			[1]: The number of values,
	//			[2]: The type of values,
	//			[3]: Normalised?
	//			[4]: The stride between each of that vertex(section) in your array of values.
	//			[5]: The offset from where the data begins in the buffer.
	// Mesh vertex data should be in the format: {Pos, Color, Tex, ...}
	// glVertexAttribPointer(_mesh.local_texcoordinates_layoutindex, _mesh->uv_stride, GL_FLOAT, GL_FALSE, _mesh->vertex_stride - (_mesh->color_stride + _mesh->uv_stride) + _mesh->color_stride, (void *)(_mesh.uv_offset * sizeof(GLfloat));
    //Generate new buffers for _mesh.
    if(glIsVertexArray(buffer->VAO) == GL_FALSE){GLCall(glGenBuffer(buffer->VAO))}
    GLCall(glBindVertexArray(buffer->VAO));
    if(glIsBuffer(buffer->VBO) == GL_FALSE){GLCall(glGenBuffer(buffer->VBO));}
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer->VBO));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->EBO));
    
    draw_debug_trace(__FILE__, __LINE__);

    GLint enabled = 0, size = 0, type = 0, normalized = 0, stride_ = 0;
    GLvoid* pointer = NULL;
    if(pos_layoutindex(_mesh) != INT32_MIN){
        GLCall(glVertexAttribPointer(pos_layoutindex(_mesh), (vertex_stride(_mesh) - (color_stride(_mesh) + uv_stride(_mesh))) * sizeof(GLfloat), GL_FLOAT, GL_FALSE, color_stride(_mesh) + uv_stride(_mesh), (void *)(pos_offset(_mesh) * sizeof(GLfloat))));
        GLCall(glEnableVertexAttribArray(pos_layoutindex(_mesh)));
        debug_vert_attr(pos_layoutindex(_mesh));
    }

    if(color_layoutindex(_mesh) != INT32_MIN){
        GLCall(glVertexAttribPointer(color_layoutindex(_mesh), color_stride(_mesh), GL_FLOAT, GL_FALSE, (vertex_stride(_mesh) - (color_stride(_mesh) + uv_stride(_mesh)) + uv_stride(_mesh)) * sizeof(GLfloat), (void *)(color_offset(_mesh) * sizeof(GLfloat))));
        GLCall(glEnableVertexAttribArray(color_layoutindex(_mesh)));
        debug_vert_attr(color_layoutindex(_mesh));
    }

    if(local_texcoordinates_layoutindex(_mesh) != INT32_MIN){
        GLCall(glVertexAttribPointer(local_texcoordinates_layoutindex(_mesh), uv_stride(_mesh), GL_FLOAT, GL_FALSE, vertex_stride(_mesh) - (color_stride(_mesh) + uv_stride(_mesh)) + color_stride(_mesh), (void *)(uv_offset(_mesh) * sizeof(GLfloat))));
        GLCall(glEnableVertexAttribArray(local_texcoordinates_layoutindex(_mesh)));
        debug_vert_attr(local_texcoordinates_layoutindex(_mesh));
    }

    draw_debug_trace(__FILE__, __LINE__);
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindVertexArray(0));
}

size_t texture_num = 0;
/// @brief Bind a texture to a mesh.
/// @param m The mesh to recieve the texture.
/// @param texture The texture to be used.
void mesh_bindtexture(mesh_t *m, image_t *texture){
    //Handle texture.
    if(texture && memsize(texture->img) != 0){
        if(glIsTexture(texture->ID) == GL_FALSE){
            image_t *out = image_gen(texture->color_channels, texture->path, texture->border, texture->format);
            free(texture->path);
            free(texture->img);
            free(texture);
            texture = out;
        }
        GLCall(glBindTexture(texture->format.target, texture->ID));
        m->texture = texture;
        return;
    }
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
#define TARGET_RELATIVE_PATH "\\Resources\\Saves\\_._"

char *cwd = NULL;
size_t cwd_len = 0;

/// @brief Initialise the Current Working Directory.
/// @return True if success.
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
        char *full_path = calloc(full_len, sizeof(char));
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

/// @brief Safe wrapper for @ref glUseProgam.
/// @param prog The program to be used.
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

/// @brief Print state data of OpenGL Buffers and attributes.
/// @param file 
/// @param line 
void draw_debug_trace(const char* file, int line) {
#ifdef _DEBUG_
    GLint currentProgram = 0;
    GLint boundVAO = 0;
    GLint boundArrayBuffer = 0;
    GLint boundElementBuffer = 0;

    GLCall(glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram));
    GLCall(glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &boundVAO));
   	GLCall(glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &boundArrayBuffer));
    GLCall(glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &boundElementBuffer));

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
    GLCall(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttribs));
    for(int i = 0; i < maxAttribs; ++i){
        GLint enabled = 0;
        GLCall(glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled));
        if(enabled){fprintf(stderr, ANSI_YELLOW("  Attribute %d: ENABLED\n"), i);}
    }

    GLenum err_debug = glGetError();
    if (err_debug != GL_NO_ERROR) {
        fprintf(stderr, ANSI_RED("  OpenGL Error Before Draw: 0x%x\n"), err_debug);
    }
#endif
}

/// @brief Generate a Mesh.
/// @param vertices The vertices to be used.
/// @param indices The indices to be used.
/// @param len The length of both [0]: @ref vertices length, [1]: @ref indices length.
/// @param strides The strides to be used. [0]: vertex_stride, [1]: uv_stride, [2]: color_stride.
/// @param offsets The vertex data offsets to be used (As [type] indexes, not byte indexes). [0]: pos_offset, [1]: color_offset, [2]: uv_offset.
/// @param layout_indices The layout indexes to be used.
/// @param format The format to use.
/// @param texture The texture to be used.
/// @return A generated mesh.
mesh_t *mesh_gen(scene_t *scene, vec3 pos, vec3 rot, GLfloat *vertices, GLuint *indices, size_t len[2], uint8_t strides[3], uint8_t layouts[3], uint8_t offsets[3], GLenum format, image_t *texture, collider_shape_t collider_shape){
    mesh_t *out = calloc(1, sizeof(mesh_t));
    *out = (mesh_t){
        .ID = scene == NULL? 0: scene->mesh_num++,
        .pos[0] = pos[0], .pos[1] = pos[1], .pos[2] = pos[2],
        .rot[0] = rot[0], .rot[1] = rot[1], .rot[2] = rot[2],
        .vertex_data = vertices,
        .data_len = len[0],
        .index_data = indices,
        .index_len = len[1],
        .texture = texture,
        .strides = (strides[0] + ((uint8_t)(strides[1] << 8)) + ((uint8_t)(strides[2] << 16))),
        .offsets = (offsets[0] + ((uint8_t)(offsets[1] << 8)) + ((uint8_t)(offsets[2] << 16))),
        .layouts = (layouts[0] + ((uint8_t)(layouts[1] << 8)) + ((uint8_t)(layouts[2] << 16))),
        .coll_shape = calloc(1, sizeof(collider_shape_t))
    };
    memcpy(out->coll_shape, &collider_shape, sizeof(collider_shape_t));
    out->coll_shape->parent = out;
    out->buffer = bufferobj_gen(out, GL_STATIC_DRAW);
    // bufferobject_handle(buffer, vertices, len[0], indices, len[1], format, 10);
    return out;
}

/// @brief Generate a Texture.
/// @param color_channels The expected colors channels to use.
/// @param image_path The path to the image to be used.
/// @param border The border colors to use.
/// @param format The @ref texformat_t type to use for the texture.
/// @return The generated Image.
image_t *image_gen(uint8_t color_channels, char *image_path, float border[4], texformat_t format){
    image_t *out = calloc(1, sizeof(image_t));
	int w, h;
    *out = (image_t){
        .img = stbi_load(image_path, &w, &h, &out->color_channels, color_channels),
        .format = format,
        .path = image_path,
        .border[0] = border[0],
        .border[1] = border[1],
        .border[2] = border[2],
        .border[3] = border[3],
    };
	out->width = w; out->height = h;
    GLCall(glGenTexture(out->ID));
    if(glIsTexture(out->ID) == GL_TRUE){
        out->unit = texture_num++;
        GLCall(glActiveTexture(GL_TEXTURE0 + out->unit));
    }
    GLCall(glBindTexture(format.target, out->ID));
    GLCall(glTexParameteri(format.target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
    GLCall(glTexParameteri(format.target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
    GLCall(glTexParameterfv(format.target, GL_TEXTURE_BORDER_COLOR, out->border));
    GLCall(glTexParameteri(format.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GLCall(glTexParameteri(format.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    switch (format.target) {
        case GL_TEXTURE_1D:
            GLCall(glTexImage1D(GL_TEXTURE_1D, format.level, format.internalFormat, out->width, 0, format.pixel_format, format.pixel_type, out->img));
            break;
        case GL_TEXTURE_2D:
            GLCall(glTexImage2D(GL_TEXTURE_2D, format.level, format.internalFormat, out->width, out->height, 0, format.pixel_format, format.pixel_type, out->img));
            break;
        case GL_TEXTURE_3D:
            GLCall(glTexImage3D(GL_TEXTURE_3D, format.level, format.internalFormat, out->width, out->height, format.depth, 0, format.pixel_format, format.pixel_type, out->img));
            break;
        default:
            GLCall(glDeleteTextures(1, &out->ID));
            return NULL;
    }
	GLCall(glGenerateMipmap(format.target));
	GLCall(glBindTexture(format.target, 0));
	stbi_image_free(out->img);
	free(out);

    fprintf(stderr, ANSI_YELLOW("\nImage load output: %s\n"), stbi_failure_reason());
    printf(ANSI_YELLOW("\nImage params: \n\tUnit: %u \n\tPath: %s \n\tWidth: %u\n\tHeight: %u\n\tColor channels: %u, \n\tBorder: {%f, %f, %f, %F}"), out->unit, out->path, out->width, out->height, out->color_channels, out->border[0], out->border[1], out->border[2], out->border[3]);
    printf(ANSI_YELLOW("\nFormat: \n\tdepth: %u \n\tInternal format: %u \n\tlevel: %u \n\tpixel format: %u \n\tpixel_type: %u \n\ttarget: %u"), out->format.depth, out->format.internalFormat, out->format.level, out->format.pixel_format, out->format.pixel_type, out->format.target);
    return out;

}

void *cam_input_default(void *cam_){return NULL;}

/// @brief Generate a cam_t object as a child of a scene_t.
/// @param scene The scene to be used.
/// @param args0 The default vec3 args. [0]: pos, [1]: rot, [2]: up
/// @param args1 The GLint args. [0]: width, [1]: height, [2]: speed, [3]: sensitivity,
/// @param args2 The float args. [0]: near plane; length from Camera, [1]: far plane; length from Camera, [2]: FOV; clipping range of Camera.
/// @param input 
void cam_gen(scene_t *scene, vec3 args0[3], GLint args1[4], float args2[3], bool activate){
    scene->cameras = realloc(scene->cameras, scene->cam_num * sizeof(cam_t));
    scene->cameras[scene->cam_num] = (cam_t){
        .pos[0] = args0[0][0], .pos[1] = args0[0][1], .pos[2] = args0[0][2],
        .rot[0] = args0[1][0], .rot[1] = args0[1][1], .rot[2] = args0[1][2],
        .up[0] = args0[2][0], .up[1] = args0[2][1], .up[2] = args0[2][2],
        .width = args1[0],
        .height = args1[1],
        .speed = args1[2],
        .sensitivity = args1[3],
        .near = args2[0],
        .far = args2[1],
        .FOV = args2[2]
    };
    scene->cam_num++;
    if(activate){cam_toggle(scene->cam_num - 1, scene);}
}

mat4 *cam_mat4(scene_t *scene){
	float w, h;
	glfwGetDimensions(scene->parent, &w, &h);
	cam_t *cam = scene->cameras[*scene->loaded_cams];
    mat4 *out = calloc(1, sizeof(mat4)), projection, view;
	glm_mat4_identity(*out);
	glm_mat4_rotate(*out, glm_radians(cam->rotm), cam->rot);
	glm_mat4_translate(*out, (vec3){-cam->pos[0], -cam->pos[1], -cam->pos[2]});
	glm_mat4_perspective(*out, glm_radians(cam->rotm), (float)(w / h), cam->near, cam->far);
    return out;
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

