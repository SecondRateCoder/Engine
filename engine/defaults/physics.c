#include "../engine/defaults/physhandler.h"

void physics_gen(scene_t *parent, char *phys_shader_path){
	physb_t *out = calloc(1, sizeof(physb_t));
	*out = (physics_buffer){
		.shaderProgram = 0,
		.type = SCENECOMP_PHYSBUFF,
		.parent = parent
	};
	FILE *f = fopen(phys_shader_path, "rb");
	fseek(f, 0, SEEK_END);
	long flen = ftell(f);
	out->shader = calloc(flen, sizeof(char));
	fseek(f, 0, SEEK_SET);
	fread(out->shader, sizeof(char), flen, f);
	shader_pull(phys_shader_path, (bool[]){true, true, false});

	shaderblock_t *shader = shader_compile(true);
	glGenFrameBuffer(out->FBO);
	glBindBuffer(GL_FRAMEBUFFER, out->FBO);
	if(glChecKFrameBufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE){
		// Success;
	}else{
		free(out->shader);
		glDeleteProgram(out->shaderProgram);
		free(out);
	}
	parent->components = realloc(parent->components, get_componenti(parent, parent->num_components) + sizeof(physb_t));
	memcpy(parent->compbuff_length, out, sizeof(physb_t));
	parent->num_components++;
	glBindFrameBuffer(0);
	return out;
}

/// @brief Process the collisions of all meshes in a scene, broadly.
/// @remarks This runs a shader for each mesh, it takes thier largest dot and thier position,
///	It passes the target mesh's position and largest dot as uniforms and the parent's array of meshes positions and largest dot's as attributes.
/// @remarks This function runs in batches, this is handled internally.
collision_result *collision_broadproc(physb_t *buffer, size_t *out_len, uint32_t *batch_num){
	GLuint VAO = 0, VBO = 0;
	collision_result **out = NULL;
	out_len = 0;
	sizeof(scene_t);
	// Organise to nearest 3.
	uint8_t _3 = (buffer->parent->mesh_num + (buffer->parent->mesh_num % 3));
	if(buffer->parent->mesh_num % 3 != 0){
		buffer->parent->meshes = realloc(buffer->parent->meshes, sizeof(mesh_t) * (buffer->parent->mesh_num + (buffer->parent->mesh_num % 3)));
	}
	uint8_t max_ = (buffer->parent->batch_size > (buffer->parent->mesh_num - (buffer->parent->batch_size * *batch_num)? (buffer->parent->mesh_num - (buffer->parent->batch_size * *batch_num)): buffer->parent->batch_size));
	GLCall(glGenBuffer(VAO));
	GLCall(glBindVertexArray(VAO));
	GLCall(glGenBuffer(VBO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	glBufferData(GL_ARRAY_BUFFER, sizeof(mesh_t) * max_, buffer->parent->meshes + (buffer->parent->batch_size * *batch_num), GL_STREAM_DRAW); 
	GLUseProgram(buffer->shaderProgram);
	int w, h;
	glfwGetFramebufferSize(buffer->parent->parent, &w, &h);
	glUniform1ui(glGetUniformLocation(buffer->shaderProgram, "width"), w);
	for(size_t cc = 0; cc < max_; ++cc){
		// Bind attributes.
		// Pos0
		glUniform3f(glGetUniformLocation(buffer->shaderProgram, "pos0"), (buffer->parent->meshes + (*batch_num * buffer->parent->batch_size) + cc)->pos[0],(buffer->parent->meshes + (*batch_num * buffer->parent->batch_size) + cc)->pos[1], (buffer->parent->meshes + (*batch_num * buffer->parent->batch_size) + cc)->pos[2]); // Write Position
		// ldot0
		glUniform1f(glGetUniformLocation(buffer->shaderProgram, "ldot0"), (buffer->parent->meshes + (*batch_num * buffer->parent->batch_size) + cc)->ldot); // Write Largest dot.
		// Pos1
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_t) - (sizeof(GLfloat) * 3), (void *)(sizeof(mesh_t)));
		// ldot1
		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(mesh_t) - sizeof(GLfloat), (void *)((sizeof(GLfloat) * 6) + sizeof(mesh_t)));
		// Pos2
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_t) - (sizeof(GLfloat) * 3), (void *)(sizeof(mesh_t) * 2));
		// ldot2
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(mesh_t) - sizeof(GLfloat), (void *)((sizeof(GLfloat) * 6) + (sizeof(mesh_t) * 2)));
		// counter
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(mesh_t) - sizeof(GLfloat), (void *)((sizeof(GLfloat) * 6) + (sizeof(mesh_t) * 2)));
	}
}

