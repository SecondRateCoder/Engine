#include "../graphics/graphics.h"

/*
scene_t decoded:
    shader header:
        NULL-terminated string
    shaders:
        Pure shader text, given with the metadata:
            [Total number of shader script characters(size_t[2])]
        Expects shader text in proper format
*/

scene_t *scene_gen(char *name, mesh_t *meshes, shaderblock_t *shaders, size_t len[2]){
	scene_t *out = malloc(sizeof(scene_t));
	*out = (scene_t){
		.header.name = name,
		.meshes = meshes,
		.meshes_num = len[0],
		.shaders = shaders,
		.shader_num = len[1],
		.shader_curr = len[1] > 1? 0: -1
	};
	return out;
}

scene_t *scene_load(const char *path){
    FILE *file = fopen(path, "rb");
    if(file == NULL){printf("Invalid scene load attempt:\n\tReason:\n\t\tInvalid path: %s", path);}else{
        size_t counter = 0;
        scene_t *out = calloc(sizeof(scene_t), 1);
        fread(&counter, sizeof(size_t), 1, file);
        out->header.name = calloc(counter, sizeof(char));
        fread(out->header.name, sizeof(char), counter, file);
        // Load shader number
        fread(&counter, sizeof(size_t), 1, file);
        out->shader_num = counter;
        out->shaders = calloc(counter, sizeof(shaderblock_t));
        for(size_t cc = 0; cc < counter; ++cc){
            size_t len = 0;
            // Load vertex shader
            fread(&len, sizeof(size_t), 1, file);
            out->shaders[cc].vertex = calloc(len, sizeof(char));
            fread(out->shaders[cc].vertex, sizeof(char), len, file);
            // Load fragment shader
            fread(&len, sizeof(size_t), 1, file);
            out->shaders[cc].fragment = calloc(len, sizeof(char));
            fread(out->shaders[cc].fragment, sizeof(char), len, file);
            // Load geometry shader
            fread(&len, sizeof(size_t), 1, file);
            out->shaders[cc].geometry = calloc(len, sizeof(char));
            fread(out->shaders[cc].geometry, sizeof(char), len, file);
        }
        fread(&counter, sizeof(size_t), 1, file);
        if(counter != 0){printf("Invalid scene load attempt:\n\tReason:\n\t\tInvalid file/file format: %s", path);}else{
            // Load mesh number
            fread(&counter, sizeof(size_t), 1, file);
            out->meshes_num = counter;
            out->meshes = calloc(counter, sizeof(mesh_t));
            for(size_t cc =0; cc < counter; ++cc){
                // Strides
                fread(&out->meshes[cc].align_stride, sizeof(uint8_t), 1, file);
                fread(&out->meshes[cc].vertex_stride, sizeof(uint8_t), 1, file);
                fread(&out->meshes[cc].color_stride, sizeof(uint8_t), 1, file);
                fread(&out->meshes[cc].dpi_stride, sizeof(uint8_t), 1, file);
    
                // Mesh data length
                fread(&counter, sizeof(size_t), 1, file);
                out->meshes[cc].data_len = counter;
                // Mesh data
                out->meshes[cc].vertex_data = calloc(counter, sizeof(GLfloat));
                fread(out->meshes[cc].vertex_data, sizeof(GLfloat), counter, file);
    
                //Index data len
                fread(&counter, sizeof(size_t), 1, file);
                out->meshes[cc].index_len = counter;
                // Texture struct
                out->meshes[cc].texture = calloc(1, sizeof(image_t));
                fread(&counter, sizeof(size_t), 1, file);
                out->meshes[cc].texture->path = calloc(counter, sizeof(char));
                fread(out->meshes[cc].texture->path, sizeof(char), counter, file);
    
                fread(&out->meshes[cc].texture->format, sizeof(texformat_t), 1, file);
    
                fread(&out->meshes[cc].texture->width, sizeof(GLuint), 1, file);
                fread(&out->meshes[cc].texture->height, sizeof(GLuint), 1, file);
                fread(&out->meshes[cc].texture->color_channels, sizeof(GLuint), 1, file);
    
                fread(&out->meshes[cc].texture->border, sizeof(float), 4, file);
            }
            return out;
        }
    }
    return NULL;
}

void scene_draw(scene_t *scene){
    for(size_t cc =0; cc < scene->meshes_num; ++cc){
        glBindVertexArray(scene->meshes[cc].buffer->VAO);
        glUseProgram(scene->shaders[scene->shader_curr].shaderProgram);
        glDrawElements(scene->meshes[cc].buffer->format, scene->meshes[cc].index_len, GL_UNSIGNED_INT, scene->meshes[cc].index_data);
    }
    return;
}

bool scene_save(scene_t *scene, char *target_file){
    FILE *file = fopen(target_file, "wb");
    //Write NULL-terminated header name.
    const size_t _null = 0;
    size_t _temp = strlen(scene->header.name);  _temp++;
    fwrite(&_temp, sizeof(size_t), 1, file);
    fwrite(scene->header.name, sizeof(char), _temp + 1, file);
    //Write number of shaders
    fwrite(&(scene->shader_num), sizeof(size_t), 1, file);
    if(!file){printf("Invalid scene load attempt:\n\tReason:\n\t\tInvalid path: %s", target_file);}else{
        // Shaders
        //Convert all shader texts to
        for(size_t cc = 0; cc < scene->shader_num; ++cc){
            // Malloc for buffer, each shader will be NULL-terminated.
            _temp = strlen(scene->shaders[cc].vertex) + 1;
            fwrite(&_temp, sizeof(size_t), 1, file);
            fwrite(scene->shaders[cc].vertex, sizeof(char), _temp++, file);

            _temp = strlen(scene->shaders[cc].fragment) + 1;
            fwrite(&_temp, sizeof(size_t), 1, file);
            fwrite(scene->shaders[cc].fragment, sizeof(char), _temp++, file);

            _temp = strlen(scene->shaders[cc].geometry) + 1;
            fwrite(&_temp, sizeof(size_t), 1, file);
            fwrite(scene->shaders[cc].geometry, sizeof(char), _temp++, file);
        }
        // 8-byte 0 digits to split between shader and next segment
        fwrite(&_null, sizeof(size_t), 1, file);
        fwrite(&scene->meshes_num, sizeof(size_t), 1, file);
        for(size_t cc = 0; cc < scene->meshes_num; ++cc){
            fwrite(&scene->meshes[cc].align_stride, sizeof(uint8_t), 1, file);
            fwrite(&scene->meshes[cc].vertex_stride, sizeof(uint8_t), 1, file);
            fwrite(&scene->meshes[cc].color_stride, sizeof(uint8_t), 1, file);
            fwrite(&scene->meshes[cc].dpi_stride, sizeof(uint8_t), 1, file);

            // fwrite(&scene->meshes[cc].mesh_num, sizeof(size_t), 1, file);
            // if(scene->meshes[cc].mesh_num != 0){fwrite(scene->meshes[cc].vertex_lens, sizeof(size_t), scene->meshes[cc].mesh_num, file);}

            fwrite(&scene->meshes[cc].data_len, sizeof(size_t), 1, file);
            fwrite(scene->meshes[cc].vertex_data, scene->meshes[cc].vertex_stride* sizeof(GLfloat), scene->meshes[cc].data_len, file);

            fwrite(&scene->meshes[cc].index_len, sizeof(size_t), 1, file);
            fwrite(&scene->meshes[cc].index_data, sizeof(GLuint), scene->meshes[cc].index_len, file);

            _temp = strlen(scene->meshes[cc].texture->path) + 1;
            fwrite(&_temp, sizeof(size_t), 1, file);
            fwrite(scene->meshes[cc].texture->path, sizeof(char), _temp, file);
            fwrite(&scene->meshes[cc].texture->format, sizeof(texformat_t), 1, file);

            fwrite(&scene->meshes[cc].texture->width, sizeof(GLuint), 1, file);
            fwrite(&scene->meshes[cc].texture->height, sizeof(GLuint), 1, file);
            fwrite(&scene->meshes[cc].texture->color_channels, sizeof(GLuint), 1, file);

            fwrite(scene->meshes[cc].texture->border, sizeof(float), 4, file);
        }

    }
    return false;
}

void scene_kill(scene_t *scene, bool save){
    if(save){
        char *temp = calloc(strlen(cwd) + strlen(scene->header.name) + 28, sizeof(char));
        memcpy(temp, cwd, strlen(cwd) * sizeof(cwd[0]));
        memcpy(temp + (strlen(cwd) * sizeof(cwd[0])), "\\Resources\\Saves\\", 17);
        memcpy(temp + ((strlen(cwd) + 17) * sizeof(cwd[0])), scene->header.name, strlen(scene->header.name));
        memcpy(temp + ((strlen(cwd) + strlen(scene->header.name) + 28) * sizeof(cwd[0])), ".scenesave", 17);
        scene_save(scene, temp);
        free(temp);
    }
    for(size_t cc = 0; cc < scene->shader_num; ++cc){
        free(scene->shaders[cc].vertex);
        if(scene->shaders[cc].vertexshader == 0){glDeleteShader(scene->shaders[cc].vertexshader);}
        free(scene->shaders[cc].fragment);
        if(scene->shaders[cc].fragmentshader == 0){glDeleteShader(scene->shaders[cc].fragmentshader);}
        free(scene->shaders[cc].geometry);
        if(scene->shaders[cc].geometryshader == 0){glDeleteShader(scene->shaders[cc].geometryshader);}
        for(size_t cc_ = 0; cc_ < scene->shaders[cc].uniform_len; ++cc_){
            free(scene->shaders[cc].uniforms[cc_].name);
            free(scene->shaders[cc].uniforms[cc_].type);
        }
        free(scene->shaders + cc);
    }
	for(size_t cc =0; cc < scene->meshes_num; ++cc){
		free(scene->meshes[cc].vertex_data);
		free(scene->meshes[cc].index_data);
		free(scene->meshes[cc].texture->img);
		free(scene->meshes[cc].texture->path);
		free(scene->meshes[cc].texture);
		glDeleteBuffer(scene->meshes[cc].buffer->VAO);
		glDeleteBuffer(scene->meshes[cc].buffer->VBO);
		glDeleteBuffer(scene->meshes[cc].buffer->EBO);
	}
}

void **scene_bufferdo(scene_t *scene, const BUFFER_OPTIONS option){
	void **out = NULL;
	size_t len = 0;
	for(size_t cc =0; cc < scene->meshes_num; ++cc){
		switch(option){
			case BUFFER_OPTIONS_COLLECT_VAO:
			case BUFFER_OPTIONS_COLLECT_EBO_MULTIPLE:
			case BUFFER_OPTIONS_COLLECT_VBO_MULTIPLE:
				if(len == 0){out = malloc(sizeof(void **));}else{
					out = realloc(out, len * sizeof(void **));}
				out[len] = buffer_bufferdo(scene->meshes[cc].buffer, 1, option);
				len++;
				continue;
			default:
				buffer_bufferdo(scene->meshes[cc].buffer, 1, option);
				continue;
		}
	}
	return out;
}