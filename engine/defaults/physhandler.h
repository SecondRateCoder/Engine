#include "../engine/Public.h"

#define glGenFrameBuffer(BUFFER) glBGenFrameBuffers(1, &BUFFER)

typedef struct physics_buffer{
	SCENECOMP_t type;
    GLuint FBO;
    char *shader;
	scene_t *parent;
    GLuint shaderProgram;
    mesh_t **meshes;
	uint8_t num_meshes;
}physics_buffer;
#define physb_t physics_buffer

// Framebuffer should be formatted such that it can be read directly into this struct.
typedef struct collision_result{
	size_t mesh_index;
	uint32_t num_collisions;
	uint8_t num_overflows;
	size_t collisions[];
}collision_result;

physb_t *physics_gen(scene_t *parent, char *phys_shader);
