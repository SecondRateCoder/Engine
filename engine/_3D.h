#define vec3_t vec3d
#define pvec3_t vec_pure
#define poly3_t polygon
#define mesh_t mesh;

typedef struct vec_pure{
    float x, y, z;
}vec_pure;

typedef struct vec3d{
    float x, y, z;
    pvec3_t dir;
}vec3d;


typedef void (*origin_f)(poly3_t);

typedef struct polygon{
    vec3_t a, b, c, rotation;
    origin_f origin;
}polygon;

typedef struct matrix{
    float **_mesh;
    size_t xsize, ysize;
}matrix;

typedef struct mesh{
    vec3_t *vectors, position, rotation;
}mesh;