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
/// @param mesh The mesh to be appended.
void mesh_attrlink(bufferobj_t *buffer, uint32_t pos_layout,  uint32_t color_layout,  uint32_t texture_layout, mesh_t *mesh){
    buffer->VBO = realloc(buffer->VBO, sizeof(GLuint)* (buffer->VBO_len));
    buffer->EBO = realloc(buffer->EBO, sizeof(GLuint)* (buffer->EBO_len));
    //Generate new buffers for Mesh.
    glGenBuffers(1, &buffer->VBO[buffer->VBO_len]);
    glGenBuffers(1, &buffer->EBO[buffer->EBO_len]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->VBO[buffer->VBO_len]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->EBO[buffer->EBO_len]);
    glBufferData(GL_ARRAY_BUFFER, mesh->data_len, mesh->mesh_data, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->index_len, mesh->vertex_index, GL_STATIC_DRAW);

    // Handle position layout.
	glVertexAttribPointer(pos_layout, mesh->data_len, GL_FLOAT, GL_FALSE, mesh->vertex_stride, mesh->mesh_data);
	glEnableVertexAttribArray(pos_layout);
    // Handle color layout.
	glVertexAttribPointer(color_layout, mesh->data_len, GL_FLOAT, GL_FALSE, mesh->color_stride, &mesh->mesh_data[mesh->vertex_stride-1]);
	glEnableVertexAttribArray(color_layout);
    // Handle texture coord layout.
	glVertexAttribPointer(texture_layout, mesh->data_len, GL_FLOAT, GL_FALSE, mesh->dpi_stride, &mesh->mesh_data[(mesh->vertex_stride + mesh->color_stride) - 2]);
	glEnableVertexAttribArray(texture_layout);
    buffer->EBO_len++;
    buffer->VBO_len++;
}

void mesh_arttr_relink(mesh_t *mesh){
    glVertexAttribPointer(mesh->pos_layoutindex, mesh->data_len, GL_FLOAT, GL_FALSE, mesh->vertex_stride, mesh->mesh_data);
	glEnableVertexAttribArray(mesh->pos_layoutindex);
    // Handle color layout.
	glVertexAttribPointer(mesh->color_layoutindex, mesh->data_len, GL_FLOAT, GL_FALSE, mesh->color_stride, &mesh->mesh_data[mesh->vertex_stride-1]);
	glEnableVertexAttribArray(mesh->color_layoutindex);
    // Handle texture coord layout.
	glVertexAttribPointer(mesh->local_texcoordinates_layoutindex, mesh->data_len, GL_FLOAT, GL_FALSE, mesh->dpi_stride, &mesh->mesh_data[(mesh->vertex_stride + mesh->color_stride) - 2]);
	glEnableVertexAttribArray(mesh->local_texcoordinates_layoutindex);
}