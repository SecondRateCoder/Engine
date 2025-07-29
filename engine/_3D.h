
#define vec3_t Vec3D
#define poly3_t Polygon
#define mesh_t mesh;

typedef struct Vec3D{
    float x, y, z;
}Vec3D;

typedef void (*origin_f)(poly3_t);

typedef struct Polygon{
    vec3_t a, b, c, rotation;
    origin_f origin;
}Polygon;

typedef struct Mesh{
    vec3_t *vectors, position, rotation;
}Mesh;