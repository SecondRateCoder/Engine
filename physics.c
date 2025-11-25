#include "../engine/defaults/physhandler.h"

physb_t *physics_gen(scene_t *parent, char *phys_shader){
	physb_t *out = calloc(1, sizeof(physb_t));
	*out = (physics_buffer){
		.shader = strdup(phys_shader),
		.shaderProgram = 0,
		.type = SCENECOMP_PHYSBUFF,
		.parent = parent
	};
	GLuint shader = glCompileShader(phys_shader);
	glCreateProgram(out->shaderProgram);
	glAttachShader(shader, out->shaderProgram);
	glLinkProgram(out->shaderProgram);
	glDeleteShader(shader);
	glGenFrameBuffer(out->FBO);
	glBindBuffer(GL_FRAMEBUFFER, out->FBO);
	if(glChecKFrameBufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE){
		// Success;
	}else{
		free(out->shader);
		glDeleteProgram(out->shaderProgram);
		free(out);
	}
	parent->components = realloc(parent->components, scenecomp_arrsize(parent) + sizeof(physb_t));
	memcpy(scene_getcomp(parent, parent->comp_num - 1), out, sizeof(physb_t));
	parent->comp_num++;
	glBindFrameBuffer(0);
	return out;
}

/// @brief Process the collisions of all meshes in a scene, broadly.
/// @remarks This runs a shader for each mesh, it takes thier largest dot and thier position,
///	It passes the target mesh's position and largest dot as uniforms and the parent's array of meshes positions and largest dot's as attributes.
/// @remarks This function runs in batches, this is handled internally.
collision_result *collision_broadproc(physbuf_t *buffer, size_t *out_len, uint32_t *batch_num){
	GLuint VAO = 0, VBO = 0;
	collision_result **out = NULL;
	out_len = 0;
	GLCall(glGenBuffer(VAO));
	GLCall(glBindVertexArray(VAO));
	GLCall(glGenBuffer(VBO));
	GLCall(glBindBuffer(GL_VERTEX_BUFFER, VBO));
	for(size_t cc =0; cc < buffer->parent->batch_size; ++cc){
		// Enable attribute for position and largest dot.
		size_t cc_ = cc + (buffer->parent->batch_size * batch_num); // Count num of batches in.
		GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(mesh_t) * (cc_ > buffer->parent->mesh_num? buffer->parent->mesh_num - cc_: cc_ > buffer->parent->batch_size), buffer->parent->meshes + cc, GL_STATIC_READ));
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_t) - (sizeof(GLfloat) * 3), (void *)(sizeof(mesh_t) * cc));
		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(mesh_t) - sizeof(GLfloat), (void *)((sizeof(GLfloat) * 6) + (sizeof(mesh_t) * cc)));
		GLUseProgram(buffer->shaderProgram);
		glBindFrameBuffer(buffer->FBO);
		glDrawArrays(GL_TRIANGLES, 0, buffer->parent->mesh_num);
		/*
			For each call, read off FrameBuffer pixels.
			Read off enough to read off rest off data.
		*/
		out = realloc(out, sizeof(collision_result *) * out_len);
		out[out_len] = calloc(sizeof(collision_result));
		glReadPixels(cc_%buffer->parent->parent->w, cc_/buffer->parent->parent->w, sizeof(collision_result), (size_t)(sizeof(collision_result)/buffer->parent->parent->w), GL_RGBA, GL_UNSIGNED_BYTE, sizeof(collision_result), out[out_len]);
		out[out_len] = realloc(out[out_len], sizeof(collision_result) + (sizeof(size_t) * out[out_len]->num_collisions));
		out_len++;
	}
}
