#include "../engine/Public.h"
#include "../engine/graphics/graphics.h"

#define glGenFrameBuffer(BUFFER) glBGenFrameBuffers(1, &BUFFER)
#define COLL_QUERIES(BUFFER) ((sizeof(collquery_t) / (sizeof(physb_t)) - (buffer_type(BUFFER) * buffer_size(BUFFER))))
#define TOGGLE_MAX(N) (2 << N)

typedef struct physics_buffer{
    GLuint FBO;
    GLuint shaderProgram;
	uint8_t max_queries;
}physics_buffer;
#define physb_t physics_buffer

typedef enum PHYSBATCH_t{
	PHYSBATCH_LINEAR,
	PHYSBATCH_CLOSEST,
	PHYSBATCH_FURTHEST
}PHYSBATCH_t;

typedef struct collision_result_near{
	size_t target;
	struct{
		size_t *P1_Element_index;
		vec3 *dir;
	};
}collision_result_near;
#define collresn_t collision_result_near

typedef union collision_result_broad{
	const uint32_t pixel;
	struct{
		const bool b0_1, b1_2, padding[2];
	};
}collision_result_broad;
#define collresb_t collision_result_broad

typedef enum COLLSHAPE_t{
	COLLSHAPE_CUBOID = 0x0,
	COLLSHAPE_PYRAMID = 0x1,
	COLLSHAPE_SPHERE = 0x2,
	COLLSHAPE_MESH = 0x3
}COLLSHAPE_t;
typedef struct collider_shape_t{
	mesh_t *parent;
	COLLSHAPE_t shape;
	vec3 offs, scale;
}collider_shape_t;
typedef struct collision_query{
	size_t target, start_pos;
	// Check most-significant bit for whether the query has been processed.
	uint8_t batch_size, max_;
	uint8_t batching_type;
	collresb_t *out;
}collision_query;
#define collquery_t collision_query

uint8_t physics_gen(scene_t *parent, char *phys_shader_path, uint8_t max_queries);
void collision_broadproc(scene_t *scene, uint32_t *batch_num);
collquery_t *query_collisioni(uint8_t *out_code, scene_t *scene, size_t target_mesh, size_t start_pos, uint8_t batching_size, uint8_t batching_type, bool watch_duplicate);
