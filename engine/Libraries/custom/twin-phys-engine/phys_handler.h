#include "../engine/Public.h"

#define glGenFrameBuffer(BUFFER) glBGenFrameBuffers(1, &BUFFER)
#define COLL_QUERIES(BUFFER) ((sizeof(collquery_t) / (sizeof(physb_t)) - (BUFFER->buffer_type * BUFFER->buffer_size)))
#define TOGGLE_MAX(N) (2 << N)

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
#define collres_t collision_result

typedef enum PHYSBATCH_t{
	PHYSBATCH_LINEAR,
	PHYSBATCH_CLOSEST,
	PHYSBATCH_FURTHEST
}PHYSBATCH_t;

typedef struct collision_query{
	const size_t target, start_pos;
	// Check most-significant bit for whether the query has been processed.
	const uint16_t batch_size;
	const uint8_t batching_type;
	const void *out_buffer;
}collision_query;
#define collquery_t collision_query

void physics_gen(scene_t *parent, char *phys_shader);
collision_result *collision_broadproc(scene_t *scene, size_t *out_len, uint32_t *batch_num);
