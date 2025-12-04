#include "../engine/Public.h"

const uint8_t MAX_SCENE_PROC = 1;

/// @brief Generater a new scene.
/// @param name The name of the scene.
/// @param meshes The meshes to use in the scene.
/// @param shaders The shaders to use in the scene.
/// @param len Array of lengths, [0]: Number of meshes, [1]: Number of shaders
/// @return The generated scenes.
scene_t *scene_gen(GLFWwindow *parent, char *name, mesh_t *meshes, shaderblock_t *shaders, cam_t *cameras, size_t len[3]){
	scene_t *out = calloc(1, sizeof(scene_t));
	*out = (scene_t){
		.header.name = name,
		.meshes = meshes,
		.mesh_num = len[0],
		.shaders = shaders,
		.shader_num = len[1],
		.shader_curr  = 0,
        .loaded_cams = calloc(1, sizeof(size_t *)),
        .num_loadedcams = 1,
        .input_handles = NULL,
        .num_inhandles = 0,
        .proc_buffers[0] = (sceneproc_buffer){
			.buffer = calloc(25, sizeof(GLenum) + sizeof(uint32_t)),
            .meta_data = 0 + (25 << 8) + ((sizeof(GLenum) + sizeof(uint32_t)) << 16),
            .last_poll  =0, .last_pollcycle = 0,
            .process = SCENEPROC_INPUTPOLL,
        },
        .parent = parent
	};
	if(cameras == NULL){cam_gen(out, (vec3[]){0, 0, 0, 0, 0, 0, 1, 0}, (GLint[]){90, 90, 1, 10}, (GLfloat[]){0.1, 10, 9}, true);
	}else{
		out->cameras = cameras;
		out->cam_num = len[2];
	}
	return out;
}

/// @brief Toggle a cam from whether it should be processed or not.
/// @param index The index (in loaded_cams or in cams list.).
/// @param scene The scene containing the Camera.
void cam_toggle(size_t index, scene_t *scene){
	for(size_t cc =0; cc < scene->num_loadedcams; ++cc){
		if(index == scene->loaded_cams[cc]){
			// Move all above down, then realloc, maybe or just inc/dec size marker.
			memcpy(scene->loaded_cams + cc, scene->loaded_cams + cc + 1, scene->num_loadedcams - cc - 1);
			scene->num_loadedcams--;
			return;
		}
	}
	//Is not a loaded camera.
	if(index > scene->cam_num){return;}
	scene->loaded_cams = realloc(scene->loaded_cams, (scene->num_loadedcams + 1) * sizeof(size_t *));
	scene->loaded_cams[scene->num_loadedcams] = index;
	scene->num_loadedcams++;
}

/// @brief Un-register a handle from with a specific key, target combo.
/// @param scene The scene containing the input.
/// @param key The key.
/// @param target The target.
/// @param handle The handle.
/// @return Success code.
uint8_t scene_inputh_unreg(scene_t *scene, GLenum key, GLenum target, INPUTH_handlef handle){
	for(size_t cc =0; cc < scene->num_inhandles - 1; ++cc){
		if(scene->input_handles[cc].key == key && scene->input_handles[cc].target == target){
            for(uint8_t cc_ = 0; cc_ < scene->input_handles[cc].num_handles; ++cc_){
                if(scene->input_handles[cc].handles[cc_] == handle){
                    memcpy(scene->input_handles[cc].handles + cc_, scene->input_handles[cc].handles + cc_ + 1, scene->input_handles[cc].num_handles - cc_ - 1);
                    scene->input_handles[cc].num_handles--;
                    return 0;
                }
            }
        }
	}
	return SCENEPROC_ERRORNOREF;
}

/// @brief Registr a signle handle to multiple key/target combos.
/// @param scene The scene to have the inputs registered.
/// @param keys The key, must be @ref len length.
/// @param targets The target, must be @ref len length.
/// @param len The length of both @ref keys and @ref targets.
/// @param handle The handle to be attached.
/// @param append If there is already an element with the input's key/target combo this function append the handle function.
/// @return All the outputs for all elements.
uint8_t *scene_inputh_regm(scene_t *scene, GLenum *keys, GLenum *targets, uint8_t len, INPUTH_handlef handle, bool append){
    uint8_t *out = calloc(len, sizeof(uint8_t));
    for(uint8_t cc = 0; cc < len; ++cc){out[cc] = scene_inputh_regh(scene, keys[cc], targets[cc], 1, &(handle), append);}
    return out;
}

/// @brief Register a number of handles to a single key and it's press type.
/// @param scene The scene to have the input's registered to.
/// @param key The key for the scenes.
/// @param target The target pressing type.
/// @param num_handles The number of in handles.
/// @param handles The handles to be registered.
/// @param append If there is already an element with the input's key/target combo this function append the handle function.
/// @return An error code, if not 0 check @related SCENEPROC_t
uint8_t scene_inputh_regh(scene_t *scene, GLenum key, GLenum target, size_t num_handles, INPUTH_handlef *handles, bool append){
    for(size_t cc = 0; cc < scene->num_inhandles; ++cc){
        if(scene->input_handles[cc].key == key && scene->input_handles[cc].target == target){
			if(append){
				scene->input_handles[cc].handles = realloc(scene->input_handles[cc].handles, (num_handles + scene->input_handles[cc].num_handles) * sizeof(INPUTH_handlef));
				memcpy(scene->input_handles[cc].handles + scene->input_handles[cc].num_handles, handles, num_handles * sizeof(INPUTH_handlef));
				scene->input_handles[cc].num_handles++;
				return 0;
			}else{return (uint8_t)SCENEPROC_ERRORDUPE;}
		}
    }
    scene->input_handles = realloc(scene->input_handles + 1, scene->num_inhandles * sizeof(INPUT_Handle));
    scene->input_handles[scene->num_inhandles] = (inputh_t){
        .handles = handles,
        .key = key,
        .target = target
    };
    scene->num_inhandles++;
    return 0;
}

/// @brief Process one of a single scene processes each frame.
/// @param scene The scene to have it's processes processed.
/// @param polls The polls.
/// @param poll_cycles Number of "polls" overflows.
void scene_poll(scene_t *scene, size_t polls, size_t pollcycles){
	scene_draw(scene);
    if(polls%SCENEPROC_INPUTPOLL == 0){sceneproc_inputhandle(scene, polls);}
    return;
}

/// @brief Process ALL input handles in a scene as well as add an entry to the INPUT buffer..
/// @param scene The scene to have it's inputs processed.
/// @param polls The polls.
void sceneproc_inputhandle(scene_t *scene, size_t polls){
    for(size_t cc =0; cc < scene->num_inhandles; ++cc){
        if(scene->input_handles[cc].key == GLFW_KEY_MOUSE_MOVE){
            for(uint8_t cc_ = 0; cc_ < scene->input_handles[cc].num_handles; ++cc_){scene->input_handles[cc].handles[cc_](scene, scene->input_handles[cc].key, scene->input_handles[cc].target);}
        }else{
            if(glfwGetKey(scene->parent, scene->input_handles[cc].key) == scene->input_handles[cc].target){
                ((size_t */*Size of an input entry*/)scene->proc_buffers->buffer)[counter(scene->proc_buffers)] = (/*Move to upper 16 bits*/scene->input_handles[cc].key << 32) + (uint32_t)polls;
                scene->proc_buffers->meta_data++;
                if(counter(scene->proc_buffers) >= buffer_size(scene->proc_buffers)){
                    scene->proc_buffers->meta_data |= 0xFF;   // Set all of lowset 8 bits to 1
                    scene->proc_buffers->meta_data != 0xFF;   // Force all of lowest 8 bits to 0
                }
                for(uint8_t cc_ = 0; cc_ < scene->input_handles[cc].num_handles; ++cc_){scene->input_handles[cc].handles[cc_](scene, scene->input_handles[cc].key, scene->input_handles[cc].target);}
            }
        }
    }
}

/// @brief Draw a scene for one frame.
/// @param scene The scene to be drawn.
void scene_draw(scene_t *scene){
    for(size_t cc = 0; cc < scene->mesh_num; ++cc){
        GLCall(GLUseProgram(scene->shaders[scene->shader_curr].shaderProgram));
        GLCall(glBindVertexArray(scene->meshes[cc].buffer->VAO));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, scene->meshes[cc].buffer->VBO));

        if(scene->meshes[cc].texture){
            GLCall(glActiveTexture(GL_TEXTURE0 + scene->meshes[cc].texture->unit));
            GLCall(glBindTexture(scene->meshes[cc].texture->format.target, GL_TEXTURE0 + scene->meshes[cc].texture->ID));
            GLCall(glUniform1i(glGetUniformLocation(scene->shaders[scene->shader_curr].shaderProgram, "tex0"), scene->meshes[cc].texture->unit));
        }

        //! For now handle 1 Camera.
        mat4 *out = cam_mat4(scene->cameras + *scene->loaded_cams);
        GLCall(glUniformMatrix4fv(glGetUniformLocation(scene->shaders[scene->shader_curr].shaderProgram, "matrix"), 1, true, (GLfloat *)(*out)));
        GLCall(glUniform3f(glGetUniformLocation(scene->shaders[scene->shader_curr].shaderProgram, "offs"), (scene->cameras + *scene->loaded_cams)->pos[0], (scene->cameras + *scene->loaded_cams)->pos[1], (scene->cameras + *scene->loaded_cams)->pos[2]));

#ifdef _DEBUG_ // Full debug
        printf(ANSI_RED("FULL DEBUG:"));
        DEBUG_BUFFER_STATE(GL_ARRAY_BUFFER, "VBO");
        DEBUG_BUFFER_STATE(GL_ELEMENT_ARRAY_BUFFER, "EBO");
        draw_debug_trace(__FILE__, __LINE__);
        debug_vert_attr(pos_layoutindex(scene->meshes + cc));
        debug_vert_attr(color_layoutindex(scene->meshes + cc));
        debug_vert_attr(local_texcoordinates_layoutindex(scene->meshes + cc));
#endif

        // GLCall(glDrawElements(scene->meshes[cc].buffer->format, scene->meshes[cc].index_len, GL_UNSIGNED_INT, 0));
        GLCall(glDrawArrays(GL_TRIANGLES, 0, scene->meshes[cc].data_len));
    }
    return;
}

/// @brief Load a scene from a file.
/// @param path The path to said file.
/// @return The loaded scene.
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
            out->mesh_num = counter;
            out->meshes = calloc(counter, sizeof(mesh_t));
            for(size_t cc =0; cc < counter; ++cc){
                // Strides
                fread(&((out->meshes + cc)->strides), sizeof(uint32_t), 1, file);
                fread(&((out->meshes + cc)->offsets), sizeof(uint32_t), 1, file);
    
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


/// @brief Save a scene into a single file.
/// @param scene The scene to be saved.
/// @param target_file The target file or path to be saved to.
/// @return Whether the saving succeeded.
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
        fwrite(&scene->mesh_num, sizeof(size_t), 1, file);
        for(size_t cc = 0; cc < scene->mesh_num; ++cc){
            fwrite(&((scene->meshes + cc)->strides), sizeof(uint32_t), 1, file);
            fwrite(&((scene->meshes + cc)->offsets), sizeof(uint32_t), 1, file);

            // fwrite(&scene->meshes[cc].mesh_num, sizeof(size_t), 1, file);
            // if(scene->meshes[cc].mesh_num != 0){fwrite(scene->meshes[cc].vertex_lens, sizeof(size_t), scene->meshes[cc].mesh_num, file);}

            fwrite(&scene->meshes[cc].data_len, sizeof(size_t), 1, file);
            fwrite(scene->meshes[cc].vertex_data, vertex_stride(scene->meshes + cc)* sizeof(GLfloat), scene->meshes[cc].data_len, file);

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

/// @brief Destroy a scene.
/// @param scene The scene o be destroyed.
/// @param save Should the scene be saved?
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
        if(scene->shaders[cc].vertexshader == 0){GLCall(glDeleteShader(scene->shaders[cc].vertexshader));}
        free(scene->shaders[cc].fragment);
        if(scene->shaders[cc].fragmentshader == 0){GLCall(glDeleteShader(scene->shaders[cc].fragmentshader));}
        free(scene->shaders[cc].geometry);
        if(scene->shaders[cc].geometryshader == 0){GLCall(glDeleteShader(scene->shaders[cc].geometryshader));}
        for(size_t cc_ = 0; cc_ < scene->shaders[cc].uniform_len; ++cc_){
            free(scene->shaders[cc].uniforms[cc_].name);
            free(scene->shaders[cc].uniforms[cc_].type);
        }
        free(scene->shaders + cc);
    }
	for(size_t cc =0; cc < scene->mesh_num; ++cc){
		free(scene->meshes[cc].vertex_data);
		free(scene->meshes[cc].index_data);
		free(scene->meshes[cc].texture->img);
		free(scene->meshes[cc].texture->path);
		free(scene->meshes[cc].texture);
		GLCall(glDeleteBuffer(scene->meshes[cc].buffer->VAO));
		GLCall(glDeleteBuffer(scene->meshes[cc].buffer->VBO));
		GLCall(glDeleteBuffer(scene->meshes[cc].buffer->EBO));
	}
}

/// @brief Complete a specific function on a scene's buffers.
/// @param scene The scene containing the @ref bufferobj_t properties to be processed.
/// @param option The function to be ran.
/// @return The returns from the values.
void **scene_bufferdo(scene_t *scene, const BUFFER_OPTIONS option){
	void **out = NULL;
	size_t len = 0;
	for(size_t cc =0; cc < scene->mesh_num; ++cc){
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

sceneprocbf_t *get_procb(scene_t *scene, SCENEPROC_t process){
    for(uint8_t cc =0; cc < MAX_SCENE_PROC; ++cc){
        if(scene->proc_buffers[cc].process == process){return scene->proc_buffers + cc;}
    }
    return NULL;
}
