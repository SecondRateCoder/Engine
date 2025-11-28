#include "../engine/Libraries/custom/twin-phys-engine/phys_handler.h"

uint8_t physics_gen(scene_t *parent, char *phys_shader_path, uint8_t max_queries){
	sceneprocbf_t *out = get_procb(parent, SCENEPROC_PHYSPOLL);
	if(out == NULL){
		// Generate
		out = get_procb(parent, 0); // Get unused buffer
		if(out == NULL){return SCENEPROC_NOMEM;}else{
			*out = (sceneprocbf_t){
				.last_poll = 0,
				.last_pollcycle = 0,
				.buffer_size = max_queries,
				.buffer_type = sizeof(collquery_t),
				.counter = 0,
				.buffer = malloc(sizeof(physb_t) + (sizeof(collquery_t) * max_queries)),
				.process = SCENEPROC_PHYSPOLL
			};
			physb_t *temp = out->buffer;
			temp->max_queries = max_queries;
			glGenFrameBuffer(temp->FBO);
			shaderblock_t *sb = shader_compile(true);
			if(sb->compiled_[0]){return SCENEPROC_ERRORLOGIC;}
			temp->shaderProgram = sb->shaderProgram;
			for(size_t cc =0; cc < sb->uniform_len; ++cc){
				free(sb->uniforms[cc].name);
				free(sb->uniforms[cc].type);
			}
			free(sb->uniforms);		free(sb);
			return 0;
		}
	}
	return SCENEPROC_DUPE;
	// Initialise field.
}

collquery_t *query_collisioni(uint8_t *out_code, scene_t *scene, size_t target_mesh, size_t start_pos, uint8_t batching_size, uint8_t batching_type, bool watch_duplicate){
	sceneprocbf_t *buffer = get_procb(scene, SCENEPROC_PHYSPOLL);
	if(buffer == NULL){out_code = SCENEPROC_NOREF;	return NULL;}
	if(((physb_t *)buffer->buffer)->max_queries == buffer->counter){*out_code = SCENEPROC_ERRORREJECTION;	return NULL;}
	if(watch_duplicate){
		collquery_t *queries = (collquery_t *)(buffer->buffer + sizeof(physb_t));
		for(size_t cc =0; cc < buffer->counter; ++cc){
			if(
				queries[cc].target == target_mesh &&
				queries[cc].start_pos == start_pos &&
				queries[cc].batch_size == batch_size &&
				queries[cc].batching_type == batching_type
			){*out_code = SCENEPROC_DUPE;		return NULL;}
		}
	}
	*(collquery_t *)(buffer->buffer + sizeof(physb_t) + (buffer->counter * buffer->buffer_type)) = (collquery_t){
		.target = target_mesh,
		.start_pos = start_pos,
		.batch_size = batch_size,
		.batching_type = batching_type,
		.out = NULL
	};
	buffer->counter++;
	if(buffer->counter == buffer->buffer_size){buffer->counter = 0;}
	return (collquery_t *)(buffer->buffer + sizeof(physb_t) + (buffer->counter * buffer->buffer_type));
}

/// @brief Process the collisions of all meshes in a scene, broadly.
/// @remarks This runs a shader for each mesh, it takes thier largest dot and thier position,
///	It passes the target mesh's position and largest dot as uniforms and the parent's array of meshes positions and largest dot's as attributes.
/// @remarks This function runs in batches, this is handled internally.
collision_result *collision_broadproc(scene_t *scene, uint32_t *batch_num){
	GLuint og_VAO, og_VBO;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, og_VAO);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, og_VBO);
	GLuint VAO = 0, VBO = 0;
	collision_result **out = NULL;
	out_len = 0;
	// Organise to nearest 3.
	const uint8_t _3 = (scene->mesh_num + (scene->mesh_num % 3));
	if(scene->mesh_num % 3 != 0){scene->meshes = realloc(scene->meshes, sizeof(mesh_t) * (scene->mesh_num + (scene->mesh_num % 3)));}
	GLCall(glGenBuffer(VAO));
	GLCall(glBindVertexArray(VAO));
	GLCall(glGenBuffer(VBO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GLUseProgram((scene->shaders + scene->shader_curr)->shaderProgram);
	int w, h;
	glfwGetFramebufferSize(scene->parent, &w, &h);
	glUniform1ui(glGetUniformLocation((scene->shaders + scene->shader_curr)->shaderProgram, "width"), w);
	sceneprocbf_t *phys_buffer = get_procb(scene, SCENEPROC_PHYSPOLL);
	// Get number of queries.
	size_t __length = COLL_QUERIES(phys_buffer);
	for(size_t cc = 0; cc < __length; ++cc){
		collquery_t *temp = (collquery_t *)(phys_buffer->buffer + sizeof(physb_t) + (sizeof(collquery_t) *  cc));
		// The maximum data to be processed.
		uint8_t max_ = (temp->batch_size > ((scene->mesh_num - temp->start_pos) - (temp->batch_size * temp->start_pos)? ((scene->mesh_num - temp->start_pos) - (temp->batch_size * temp->start_pos)): temp->batch_size));
		temp->max_ = max_;
		GLfloat *data = calloc((max_ << 2), sizeof(GLfloat) + 1); // plus 1 so that foreach 4 GLfloats, space for one GLuint is created.
		size_t cc = 0;
		bool selected_[max_] = {0};
		switch(temp->batching_type){
			case PHYSBATCH_LINEAR:
				// Compile flat GLfloat list of every gameObj's vec3 position and GLfloat ldot.
				for(cc = 0; cc < max_; cc++){
					data[(cc << 2)] = scene->meshes[cc + temp->start_pos].pos[0];
					data[(cc << 2) + 1] = scene->meshes[cc + temp->start_pos].pos[1];
					data[(cc << 2) + 2] = scene->meshes[cc + temp->start_pos].pos[2];
					data[(cc << 2) + 3] = scene->meshes[cc + temp->start_pos].ldot;
					data[(cc << 2) + 4] = cc;
				}
				// if(max_ % temp->batch_size != 0){data = realloc(data, sizeof(GLfloat) * temp->batch_size);}
				break;
			case PHYSBATCH_CLOSEST:
				// Compare each gameobj and find the closest one.
				size_t counter = 0;
				// Count linearly through mesh list.
				for(cc = 0; cc < max_; ++cc){
					// Count till end, leaving _temp such that it's the index of the lowest length item.
					size_t cc_ = cc;
					size_t shortest_index = 0;
					float shortest_dist, _temp;
					for(; cc_ < max_; ++cc_){
						if((_temp = glm_dot((scene->meshes + temp->start_pos + shortest_index)->pos, (scene->meshes + temp->start_pos + cc_)->pos)) < shortest_dist){
							if(selected_[cc_] != true){
								selected_[cc_] = true;
								shortest_dist = _temp;
								shortest_index = cc_;
							}
						}
					}
					data[(cc << 2)] = scene->meshes[shortest_index + temp->start_pos].pos[0];
					data[(cc << 2) + 1] = scene->meshes[shortest_index + temp->start_pos].pos[1];
					data[(cc << 2) + 2] = scene->meshes[shortest_index + temp->start_pos].pos[2];
					data[(cc << 2) + 3] = scene->meshes[shortest_index + temp->start_pos].ldot;
					data[(cc << 2) + 4] = cc;
				}
				break;
			case PHYSBATCH_FURTHEST:
				// Compare each gameobj and find the furthest one.
				size_t counter = 0;
				// Count linearly through mesh list.
				for(cc = 0; cc < max_; ++cc){
					// Count till end, leaving _temp such that it's the index of the lowest length item.
					size_t cc_ = cc;
					size_t longest_index = 0;
					float longest_dist, _temp;
					bool selected[max_] = {0};
					for(; cc_ < max_; ++cc_){
						if((_temp = glm_dot((scene->meshes + temp->start_pos + longest_index)->pos, (scene->meshes + temp->start_pos + cc_)->pos)) > longest_dist){
							if(selected_[cc_]!= true){
								selected_[cc_] = true;
								longest_dist = _temp;
								longest_index = cc_;
							}
						}
					}
					data[(cc << 2)] = scene->meshes[longest_index + temp->start_pos].pos[0];
					data[(cc << 2) + 1] = scene->meshes[longest_index + temp->start_pos].pos[1];
					data[(cc << 2) + 2] = scene->meshes[longest_index + temp->start_pos].pos[2];
					data[(cc << 2) + 3] = scene->meshes[longest_index + temp->start_pos].ldot;
					data[(cc << 2) + 4] = cc;
				}
				break;
            default:    return;
		}
		glBufferData(GL_ARRAY_BUFFER, (max_ << 3) * sizeof(GLfloat), data, GL_STREAM_READ);
		// Bind attributes.
		// Pos0
		glUniform3f(glGetUniformLocation((scene->shaders + scene->shader_curr)->shaderProgram, "pos0"), (scene->meshes + temp->target)->pos[0], (scene->meshes + temp->target)->pos[1], (scene->meshes + temp->target)->pos[2]); // Write Position
		// ldot0
		glUniform1f(glGetUniformLocation((scene->shaders + scene->shader_curr)->shaderProgram, "ldot0"), (scene->meshes + temp->target)->ldot); // Write Largest dot.
		// Pos1
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, ((sizeof(GLfloat) << 3) + sizeof(GLuint) << 1) - (sizeof(GLfloat) * 3), (void *)0);
		// ldot1
		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, ((sizeof(GLfloat) << 3) + sizeof(GLuint) << 1) - sizeof(GLfloat), (void *)(sizeof(GLfloat) * 3));
		// Pos2
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, ((sizeof(GLfloat) << 3) + sizeof(GLuint) << 1) - (sizeof(GLfloat) * 3), (void *)((sizeof(GLfloat) << 2) + sizeof(GLuint)));
		// ldot2
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, ((sizeof(GLfloat) << 3) + sizeof(GLuint) << 1) - sizeof(GLfloat), (void *)((sizeof(GLfloat) << 2) + sizeof(GLuint) + (sizeof(GLfloat) * 3)));
		// counter
		glVertexAttribPointer(4, 1, GL_UNSIGNED_INT, GL_FALSE, ((sizeof(GLfloat) << 3) + sizeof(GLuint) << 1) - sizeof(GLuint), (void *)((sizeof(GLfloat) << 4) + sizeof(GLuint) << 1));
		// Each cell is 8 GLfloats and 2 GLuint, stride should be ((sizeof(GLfloat) << 3) + sizeof(GLuint) << 1)
		glDrawArrays(GL_POINTS, 0, max_);

		// Retrieve Shader output.
		temp->out = calloc(max_, (sizeof(uint8_t) * 2));
		glReadPixels(0, 0, (max_ > w ? w: max_), max_ % w, GL_RGBA, GL_UNSIGNED_BYTE, max_ << 1, temp->out);
		temp->batch_size |= TOGGLE_MASK(15); // Set top-most bit to 1, thus telling the caller that the thingy has be run.
	}
	glBindBuffer(GL_ARRAY_BUFFER, og_VBO);
	glBindVertexArray(og_VAO);
}

