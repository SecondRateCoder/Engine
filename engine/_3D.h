#define pointf_t point_f
#define vec3_t vec3d
#define pvec3_t vec_pure
#define poly3_t polygon
#define ppoly3_t ppolygon
#define matrix_t matrix
#define mesh_t mesh;

typedef struct vec_pure{
    float x, y, z;
}vec_pure;

typedef struct point_f{float x, y;}point_f;

typedef struct vec3d{
    float x, y, z;
    pvec3_t dir;
}vec3d;


typedef vec3_t (*origin_f)(const poly3_t);
typedef pointf_t (*porigin_f)(const ppoly3_t);

typedef struct polygon{
    vec3_t a, b, c, rotation;
    origin_f origin;
}polygon;

typedef struct ppolygon{
	pointf_t a, b, c;
	porigin_f origin;
}ppolygon;

typedef struct matrix{
    float **_matrix;
    size_t xsize, ysize;
}matrix;

typedef struct mesh{
    vec3_t *vectors, position, rotation;
    poly3_t *_matrix;
}mesh;