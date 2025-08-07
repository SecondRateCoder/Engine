#include <Libraries/include/GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stddef.h>

#define vec3_t vec3
#define pvec3_t vec3p
#define poly3_t vpolygon
#define pointf_t point_f
#define ppoly3_t ppolygon
#define matrix_t matrix
#define mesh_t mesh;

typedef struct point_f{float x, y;}point_f;

typedef struct ppolygon{pointf_t vert[3], origin, rotation;}ppolygon;
typedef struct vpolygon{vec3_t vert[3], origin, rotation;}vpolygon;

typedef struct pmesh{
    poly3_t *polygons;
    vec3 position, rotation, *furthest_point;
    size_t len;
}pmesh;