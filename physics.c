#include "../engine/Public.h"

//! Created online.
//* TODO:
//		Add components field to scene_t.

// This is a proof of concept Physics system,
// using a Framebuffer to use OpenGL shaders to do Collision checking.
#define glGenFrameBuffer(BUFFER) glBGenFrameBuffers(1, &BUFFER)


typedef struct physics_buffer{
	SCENECOMP_t type;
    GLuint FBO;
    char *shader;
    GLuint shaderProgram;
    mesh_t **meshes;
	uint8_t num_meshes;
}physics_buffer;
#define physb_t physics_buffer

physb_t *physics_gen(scene_t *parent, char *phys_shader){
	physb_t *out = calloc(1, sizeof(physb_t));
	*out = (physics_buffer){
		.shader = strdup(phys_shader),
		.shaderProgram = 0,
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
	return out;
}
