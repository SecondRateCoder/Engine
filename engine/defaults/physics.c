#include "../engine/defaults/physhandler.h"

void physics_gen(scene_t *parent, char *phys_shader_path, size_t batch_size){
	physb_t *out = (physb_t *)get_procb(parent, 0);
	// Initialise field.
}

/// @brief Process the collisions of all meshes in a scene, broadly.
/// @remarks This runs a shader for each mesh, it takes thier largest dot and thier position,
///	It passes the target mesh's position and largest dot as uniforms and the parent's array of meshes positions and largest dot's as attributes.
/// @remarks This function runs in batches, this is handled internally.
collision_result *collision_broadproc(scene_t *scene, size_t *out_len, uint32_t *batch_num){
	GLuint VAO = 0, VBO = 0;
	collision_result **out = NULL;
	out_len = 0;
	sizeof(scene_t);
	// Organise to nearest 3.
	uint8_t _3 = (buffer->parent->mesh_num + (buffer->parent->mesh_num % 3));
	if(buffer->parent->mesh_num % 3 != 0){
		buffer->parent->meshes = realloc(buffer->parent->meshes, sizeof(mesh_t) * (buffer->parent->mesh_num + (buffer->parent->mesh_num % 3)));
	}
	GLCall(glGenBuffer(VAO));
	GLCall(glBindVertexArray(VAO));
	GLCall(glGenBuffer(VBO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GLUseProgram(buffer->shaderProgram);
	int w, h;
	glfwGetFramebufferSize(buffer->parent->parent, &w, &h);
	glUniform1ui(glGetUniformLocation(buffer->shaderProgram, "width"), w);
	sceneprocbf_t *phys_buffer = get_procb(scene, SCENEPROC_PHYSPOLL);
	// Get number of queries.
	for(size_t cc = 0; cc < (sizeof(collquery_t) / (sizeof(physb_t) - (phys_buffer->buffer_type * phys_buffer->buffer_size))); ++cc){
		collquery_t *temp = (collquery_t *)(phys_buffer->buffer + sizeof(physb_t) + (sizeof(collquery_t) *  cc);
			// The maximum data to be processed.
			uint8_t max_ = (temp->batch_size > ((buffer->parent->mesh_num - temp->start_pos) - (temp->batch_size * temp->start_pos)? ((buffer->parent->mesh_num - temp->start_pos) - (buffer->parent->batch_size * temp->starting_pos)): temp->batch_size));
		switch(temp->batching_type)){
			case PHYSBATCH_LINEAR:
				// Compile flat GLfloat list of every gameObj's vec3 position and GLfloat ldot.
				size_t cc = temp->start_pos;
				GLfloat *data = calloc(max_, sizeof(GLfloat));
				for(; cc < max_; cc+=4){
					data[cc - temp->start_pos] = scene->meshes[cc].pos[0];
					data[cc - temp->start_pos + 1] = scene->meshes[cc].pos[1];
					data[cc - temp->start_pos + 2] = scene->meshes[cc].pos[2];
					data[cc - temp->start_pos + 3] = scene->meshes[cc].ldot;
				}
				if(max_ % temp->batch_size != 0){data = realloc(data, sizeof(GLfloat) * temp->batch_size);}
				break;
		}
		// Bind attributes.
		// Pos0
		//glUniform3f(glGetUniformLocation(buffer->shaderProgram, "pos0"), (buffer->parent->meshes + (*batch_num * buffer->parent->batch_size) + cc)->pos[0],(buffer->parent->meshes + (*batch_num * buffer->parent->batch_size) + cc)->pos[1], (buffer->parent->meshes + (*batch_num * buffer->parent->batch_size) + cc)->pos[2]); // Write Position
		// ldot0
		//glUniform1f(glGetUniformLocation(buffer->shaderProgram, "ldot0"), (buffer->parent->meshes + (*batch_num * buffer->parent->batch_size) + cc)->ldot); // Write Largest dot.
		// Pos1
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_t) - (sizeof(GLfloat) * 3), (void *)(sizeof(mesh_t)));
		// ldot1
		//glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(mesh_t) - sizeof(GLfloat), (void *)((sizeof(GLfloat) * 6) + sizeof(mesh_t)));
		// Pos2
		//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_t) - (sizeof(GLfloat) * 3), (void *)(sizeof(mesh_t) * 2));
		// ldot2
		//glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(mesh_t) - sizeof(GLfloat), (void *)((sizeof(GLfloat) * 6) + (sizeof(mesh_t) * 2)));
		// counter
		//glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(mesh_t) - sizeof(GLfloat), (void *)((sizeof(GLfloat) * 6) + (sizeof(mesh_t) * 2)));
	}
}

