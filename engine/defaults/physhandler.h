#include "../engine/Public.h"

#define glGenFrameBuffer(BUFFER) glBGenFrameBuffers(1, &BUFFER)

// Framebuffer should be formatted such that it can be read directly into this struct.
typedef struct collision_result{
	size_t mesh_index;
	uint32_t num_collisions;
	uint8_t num_overflows;
	size_t collisions[];
}collision_result;

sceneprocbf_t *physics_gen(scene_t *parent, char *phys_shader);
collision_result *collision_broadproc(sceneprocbf_t *buffer, size_t *out_len, uint32_t *batch_num);