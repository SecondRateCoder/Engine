#include "../engine/Public.h"

#define glGenFrameBuffer(BUFFER) glBGenFrameBuffers(1, &BUFFER)

typedef struct physics_buffer{
    GLuint FBO;
    GLuint shaderProgram;
	uint8_t batch_size;
}physics_buffer;
#define physb_t physics_buffer
// Framebuffer should be formatted such that it can be read directly into this struct.
typedef struct collision_result{
	size_t mesh_index;
	uint32_t num_collisions;
	uint8_t num_overflows;
	size_t collisions[];
}collision_result;

void physics_gen(scene_t *parent, char *phys_shader);
collision_result *collision_broadproc(scene_t *scene, size_t *out_len, uint32_t *batch_num);