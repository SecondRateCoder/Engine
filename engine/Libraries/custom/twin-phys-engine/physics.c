#include "../engine/Libraries/custom/twin-phys-engine/phys_handler.h"

void physics_gen(scene_t *parent, char *phys_shader_path, size_t batch_size){
	physb_t *out = (physb_t *)get_procb(parent, 0);
	// Initialise field.
}

/// @brief Process the collisions of all meshes in a scene, broadly.
/// @remarks This runs a shader for each mesh, it takes thier largest dot and thier position,
///	It passes the target mesh's position and largest dot as uniforms and the parent's array of meshes positions and largest dot's as attributes.
/// @remarks This function runs in batches, this is handled internally.
collision_result *collision_broadproc(scene_t *scene, size_t *out_len, uint32_t *batch_num){
	GLuint og_VAO, og_VBO;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, og_VAO);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, og_VBO);
	GLuint VAO = 0, VBO = 0;
	collision_result **out = NULL;
	out_len = 0;
	sizeof(scene_t);
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
	size_t __length = (sizeof(collquery_t) / (sizeof(physb_t) - (phys_buffer->buffer_type * phys_buffer->buffer_size)));
	for(size_t cc = 0; cc < __length; ++cc){
		collquery_t *temp = (collquery_t *)(phys_buffer->buffer + sizeof(physb_t) + (sizeof(collquery_t) *  cc));
		// The maximum data to be processed.
		uint8_t max_ = (temp->batch_size > ((scene->mesh_num - temp->start_pos) - (temp->batch_size * temp->start_pos)? ((scene->mesh_num - temp->start_pos) - (temp->batch_size * temp->start_pos)): temp->batch_size));
		GLfloat *data = calloc((max_ << 2), sizeof(GLfloat) + 1); // plus 1 so that foreach 4 GLfloats, space for one GLuit is created.
		size_t cc = 0;
		sizeof(GLfloat), sizeof(GLuint);
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
				if(max_ % temp->batch_size != 0){data = realloc(data, sizeof(GLfloat) * temp->batch_size);}
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
							shortest_dist = _temp;
							shortest_index = cc_;
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
					for(; cc_ < max_; ++cc_){
						if((_temp = glm_dot((scene->meshes + temp->start_pos + longest_index)->pos, (scene->meshes + temp->start_pos + cc_)->pos)) > longest_dist){
							longest_dist = _temp;
							longest_index = cc_;
						}
					}
					data[(cc << 2)] = scene->meshes[longest_index + temp->start_pos].pos[0];
					data[(cc << 2) + 1] = scene->meshes[longest_index + temp->start_pos].pos[1];
					data[(cc << 2) + 2] = scene->meshes[longest_index + temp->start_pos].pos[2];
					data[(cc << 2) + 3] = scene->meshes[longest_index + temp->start_pos].ldot;
					data[(cc << 2) + 4] = cc;
				}
				break;
		}
		glBufferData(GL_ARRAY_BUFFER, (max_ << 3) * sizeof(GLfloat), data, GL_STREAM_READ);
		// Bind attributes.
		// Pos0
		glUniform3f(glGetUniformLocation((scene->shaders + scene->shader_curr)->shaderProgram, "pos0"), (scene->meshes + temp->target)->pos[0], (scene->meshes + temp->target)->pos[1], (scene->meshes + temp->target)->pos[2]); // Write Position
		// ldot0
		glUniform1f(glGetUniformLocation((scene->shaders + scene->shader_curr)->shaderProgram, "ldot0"), (scene->meshes + temp->target)->ldot); // Write Largest dot.
		// Pos1
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, ((sizeof(GLfloat) << 4) * 2) - (sizeof(GLfloat) * 3), (void *)0);
		// ldot1
		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, ((sizeof(GLfloat) << 4) * 2) - sizeof(GLfloat), (void *)(sizeof(GLfloat) * 3));
		// Pos2
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, ((sizeof(GLfloat) << 4) * 2) - (sizeof(GLfloat) * 3), (void *)(sizeof(GLfloat) << 2));
		// ldot2
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, ((sizeof(GLfloat) << 4) * 2) - sizeof(GLfloat), (void *)((sizeof(GLfloat) * 7) + (sizeof(mesh_t) * 2)));
		// counter
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, ((sizeof(GLfloat) << 4) * 2) - (sizeof(GLfloat) * 3), (void *)((sizeof(GLfloat) * 6) + (sizeof(mesh_t) * 2)));
		// Each 2 cells is 8 GLfloats overall
	}
	glBindVertexArray(og_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, og_VBO);
}

