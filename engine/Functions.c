#include "./Public.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// This file will contain all the Public types e.t.c



char* str_normalise(const char *str, const bool handle_spaces, const bool handle_upper){
    if (str == NULL){return  NULL;}
    // char *out = malloc(sizeof(char)* strlen(str));
	size_t str_len= strlen(str);
    int i = 0, j = 0;
	char *out = malloc(sizeof(char)* strlen(str));
    // Iterate through the string until the null terminator is reached.
    while (str[i] != '\0') { 
        // Check if the current character is NOT a space
        if (IS_SPACE((unsigned char)str[i]) && handle_spaces){
			//If space write the next value.
			if(i < str_len-1){
				++i;
				out[j] = str[i];
			}
        }else if(handle_upper){
            // Convert to lowercase and copy to the new position
            out[j] = tolower((unsigned char)str[i]);
			i++;
		}
		j++;
    }
    out[j] = '\0'; // Null-terminate the modified string
    return out;
}

#define HASH_64BIT_LIMIT 12

/// @brief Return a __uint128_t variable.
/// @param str The string to be hashed.
/// @return A __uint128_t value.
size_t *str_hash(const char *str){
	// A large, odd prime number is a good choice for the initial hash value.
    // 5381 is a common value used in the DJB2 algorithm.
    size_t *hash= (size_t *)malloc(sizeof(uint128_t));
    hash[0]= 5381;
	size_t cc =0;
    int c;

    // A simple loop to iterate through the string until the null terminator is found.
    while ((c = *str++)) {
        // This is the core of the DJB2 algorithm:
        // hash = hash * 33 + c;
        // The bitwise left shift `(hash << 5)` is an efficient way to do `hash * 32`.
        // Then we add the original hash to get `hash * 33`.
		++cc;
		if(cc > HASH_64BIT_LIMIT){
			hash[1] = ((hash[1] << 5) + hash[1]) + c;
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
bool uint128_t_comp(const uint128_t a,const uint128_t b){ return a[0] == b[0] && a[1] == b[1]; }

/// @brief Configure and Link a Model's VBO and EBO to the inputted bufferobj_t object.
/// @param buffer The Buffer object to be written to.
/// @param shader The shaderblock_t containing the compiled code the model will be linked to.
/// @param pos_layout The layout index to apply a reference to the model's position data.
/// @param color_layout The layout index to apply a reference to the model's color data.
/// @param texture_layout The layout index to apply a reference to the model's texture co-ordinate data.
/// @param _mesh The _mesh to be appended.
void mesh_attrlink(bufferobj_t *buffer, uint32_t pos_layout,  uint32_t color_layout,  uint32_t texture_layout, mesh_t *_mesh){
    buffer->VBO = realloc(buffer->VBO, sizeof(GLuint)* (buffer->VBO_len));
    buffer->EBO = realloc(buffer->EBO, sizeof(GLuint)* (buffer->EBO_len));
    //Generate new buffers for _mesh.
    glGenBuffers(1, &buffer->VBO[buffer->VBO_len]);
    glGenBuffers(1, &buffer->EBO[buffer->EBO_len]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->VBO[buffer->VBO_len]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->EBO[buffer->EBO_len]);
    glBufferData(GL_ARRAY_BUFFER, _mesh->data_len, _mesh->mesh_data, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _mesh->index_len, _mesh->vertex_index, GL_STATIC_DRAW);

    // Handle position layout.
	glVertexAttribPointer(pos_layout, _mesh->data_len, GL_FLOAT, GL_FALSE, _mesh->vertex_stride, _mesh->mesh_data);
	glEnableVertexAttribArray(pos_layout);
    // Handle color layout.
	glVertexAttribPointer(color_layout, _mesh->data_len, GL_FLOAT, GL_FALSE, _mesh->color_stride, &_mesh->mesh_data[_mesh->vertex_stride-1]);
	glEnableVertexAttribArray(color_layout);
    // Handle texture coord layout.
	glVertexAttribPointer(texture_layout, _mesh->data_len, GL_FLOAT, GL_FALSE, _mesh->dpi_stride, &_mesh->mesh_data[(_mesh->vertex_stride + _mesh->color_stride) - 2]);
	glEnableVertexAttribArray(texture_layout);
    buffer->EBO_len++;
    buffer->VBO_len++;
}

void mesh_arttr_relink(const mesh_t *_mesh){
    glVertexAttribPointer(_mesh->pos_layoutindex, _mesh->data_len, GL_FLOAT, GL_FALSE, _mesh->vertex_stride, _mesh->mesh_data);
	glEnableVertexAttribArray(_mesh->pos_layoutindex);
    // Handle color layout.
	glVertexAttribPointer(_mesh->color_layoutindex, _mesh->data_len, GL_FLOAT, GL_FALSE, _mesh->color_stride, &_mesh->mesh_data[_mesh->vertex_stride-1]);
	glEnableVertexAttribArray(_mesh->color_layoutindex);
    // Handle texture coord layout.
	glVertexAttribPointer(_mesh->local_texcoordinates_layoutindex, _mesh->data_len, GL_FLOAT, GL_FALSE, _mesh->dpi_stride, &_mesh->mesh_data[(_mesh->vertex_stride + _mesh->color_stride) - 2]);
	glEnableVertexAttribArray(_mesh->local_texcoordinates_layoutindex);
}

void mesh_addtexture(mesh_t *m, image_t *texture){
    //Handle texture.
    glGenTextures(1, &texture->ID);
    glBindTexture(GL_TEXTURE_2D, texture->ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, texture->border);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->img);
	glGenerateMipmap(GL_TEXTURE_2D);
    //Add to mesh
    m->textures = realloc(m->textures, memsize(m->textures)+(memsize(texture)));
    m->textures[m->num_textures] = *texture;
    m->num_textures++;

    /*
    glGenTextures(1, &new_image->ID);
	glBindTexture(GL_TEXTURE_2D, new_image->ID);

	// Set texture wrapping and filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[4] = {
		(float)border_color->r,
		(float)border_color->g,
		(float)border_color->b,
		(float)border_color->a
	};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	// Upload image data to the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, new_image->width, new_image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, new_image->img);
	glGenerateMipmap(GL_TEXTURE_2D);
    */
}

char *cwd;
size_t cwd_len;

bool cwd_init(){
#ifdef _WIN32
    #include <direct.h>
    #define getcwd(BUFFER, COUNT) _getcwd(NULL, 0)
#else
    #include <unistd.h>
#endif
    cwd_init_label:
	if ((cwd = getcwd(NULL, 0)) == NULL) {
		printf("getcwd() Error, cwd not Initialised.\n");
		return false;
	}
    "C:\\Users\\olusa\\OneDrive\\Documents\\GitHub\\Engine\\engine";
    //Safely retrieve the string length, with a increment that will accept the .exe finish.
    // while(true){
    //     if(strncmp(&cwd[cwd_len], "Engine\\\0", 9) == 0){
    //         cwd_len += 9;
    //         break;
    //     }
    //     ++cwd_len;
    // }
    cwd_len = strlen(cwd);
    // while(isascii(cwd[cc])){++cc;}
    // if(cc > cwd_len){cwd_len = cc;}
    //Move out to the Last directory to fully access neccessary data.
    FILE *temp_ =NULL;
    do{
        fclose(temp_);
        while(cwd_len > 0 && cwd[cwd_len] != '\\'){--cwd_len;}
        cwd = realloc(cwd, sizeof(cwd[0])* (cwd_len));
        if(cwd == NULL){cwd_init();}
        cwd[cwd_len]= '\0';
        char *temp = strdup(cwd);
        temp = realloc(temp, sizeof(cwd[0])* (cwd_len + strlen("\\Resources\\Shaders\\Shaders.txt")));
        strncat(&temp[cwd_len], "\\Resources\\Shaders\\Shaders.txt", 30);
        temp_ = fopen(temp, "r");
        free(temp);
    }while(temp_ == NULL);
    fclose(temp_);
    // cwd = temp;
    // uint8_t cc_ =0;
    // while(cc_ < max_cwd_resolution_attempts){
    //     for(size_t cc =0; cc < cwd_resource_paths_len; ++cc){
    //         char *temp_char = malloc((cwd_len+strlen(cwd_resource_paths[cc]))* sizeof(char));
    //         temp_char = strdup(cwd);
    //         temp_char[strlen(cwd)] = strdup(cwd_resource_paths[cc]);
    //         FILE *temp = fopen(temp_char, "r");
    //         if(temp == NULL){break;/*And mov backwards a Directory.*/}
    //     }
    //     ++cc_;
    // }
	return true;
} 

//! Use later
// const size_t cwd_resource_paths_len = 1;
// const size_t max_cwd_resolution_attempts = 3;
// const char *cwd_resource_paths[] = {
//     // "engine\\Libraries"/*Change to just Libraries*/,
//     "glfw3.dll"
// };

const uint128_t builtin_shader_typehash[37] = {
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
	{0, 5596159940102558},  {0, 5596159940102591}
};
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
const char vertexshader_default[135] =
	"#version 330 core\n"
	"#define vs\n"
	"layout(location = 0) in vec3 position;\n"
	"void main(){\n"
	"    gl_Position = vec4(position, 1.0);\n"
	"}\n"
	"#shaderend\n\0"
;
const char fragmentshader_default[111] =
	"#version 330 core\n"
	"#define fs\n"
	"out vec4 color;\n"
	"void main(){\n"
	"    color = vec4(1.0, 0.0, 0.0, 1.0);\n"
	"}\n"
	"#shaderend\n\0"
;
size_t len_typenames = 36;

const unsigned int settings_len = 15;
const unsigned int settings_version_end_index = 5;

char* vertexshader = NULL,
	* fragmentshader = NULL,
	* geometryshader = NULL,
	** shader_typenames = NULL,
	*tessellation_controlshader = NULL,
	*tessellation_evaluationshader = NULL
	/**computeshader*/
;