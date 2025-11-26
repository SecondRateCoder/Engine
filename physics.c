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
	// Organise to nearest 3.
	uint8_t _3 = (buffer->parent->mesh_num + (buffer->parent->mesh_num % 3));
	if(buffer->parent->mesh_num % 3 != 0){
		buffer->parent->meshes = realloc(buffer->parent->meshes, sizeof(mesh_t) * (buffer->parent->mesh_num + (buffer->parent->mesh_num % 3)));
	}
	uint8_t max_ = (buffer->parent->batch_size > (buffer->parent->mesh_num - (buffer->parent->batch_size * *batch_num)? (buffer->parent->mesh_num - (buffer->parent->batch_size * *batch_num): buffer->parent->batch_size);
	GLCall(glGenBuffer(VAO));
	GLCall(glBindVertexArray(VAO));
	GLCall(glGenBuffer(VBO));
	GLCall(glBindBuffer(GL_VERTEX_BUFFER, VBO));
	glBufferData(GL_ARRAY_BUFFER, sizeof(mesh_t) * max_, buffer->parent->meshes + (buffer->parent->batch_size * *bacth_num), GL_STREAM_DRAW); 
	GLUseProgram(buffer->shaderProgram);
	glUniform1ui(glGetUniformLocation(buffer->shaderProgram, "width"), buffer->parent->parent->width);
	// Bind attributes.
	// Pos0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_t) - (sizeof(GLfloat) * 3), (void *)0);
	// ldot0
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(mesh_t) - sizeof(GLfloat), (void *)(sizeof(GLfloat) * 6);
	// Pos1
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_t) - (sizeof(GLfloat) * 3), (void *)(sizeof(mesh_t));
	// ldot1
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(mesh_t) - sizeof(GLfloat), (void *)((sizeof(GLfloat) * 6) + sizeof(mesh_t));
	// Pos2
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_t) - (sizeof(GLfloat) * 3), (void *)(sizeof(mesh_t * 2)));
	// ldot2
	glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(mesh_t) - sizeof(GLfloat), (void *)((sizeof(GLfloat) * 6) + (sizeof(mesh_t) * 2));
	// counter
	glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(mesh_t) - sizeof(GLfloat), (void *)((sizeof(GLfloat) * 6) + (sizeof(mesh_t) * 2));
}
