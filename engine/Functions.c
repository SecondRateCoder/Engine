
#include "./engine/Public.h"

vec3_t flot_vec3(float *arr, uint len){
	return (vec3_t){(len < 1? 0: arr[0]), (len < 2? 0: arr[1]), (len < 3? 0: arr[2])};
}
__m128i vec3_m128(vec3_t vec){return _mm_load_ps((float[4]{vec.x, vec.y, vec.z, 0}));}
vec3_t m128_vec3(__m128 m){
	__attribute__(aligned(16))float out[4];
	_mm_store_ps(out, m);
	return (vec3_t){
		.x = out[0],
		.y = out[1],
		.z = out[2],
	}

}

vec3_t vec3_negate(vec3_t a){return (vec3_t){ 0-a.x, 0-a.y, 0-a.z};}
vec3_t vec3_rshift(vec3_t v, uint shf){return (vec3_t){v.x >> shf, v.y >> shf, v.z >> shf};}
vec3_t vec3_lshift(vec3_t v, uint shf){return (vec3_t){v.x << shf, v.y << shf, v.z << shf};}

//Not yet bench-marked.
vec3_t fastvec3_add(vec3_t a, vec3_t b){return m128_vec3(mm_add_ps(m128_vec3(a), m128_vec3(b)));}
vec3_t vec3_add(vec3_t a, vec3_t b){return (vec3_t){a.x+b.x, a.y+b.y, a.z+b.z};}

vec3_t vec3_mul(vec3_t v, float mul){
	if(mul%)
}

vec3_t vec3_sub(vec3_t a, vec3_t b){return vec3_add(a, vec3_negate(b));}