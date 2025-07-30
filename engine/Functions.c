#include "./engine/Public.h"
#include <immintrin.h>
#include <xmmintrin.h>

vec3_t flot_vec3(float *arr, uint32_t len){return (vec3_t){(len < 1? 0: arr[0]), (len < 2? 0: arr[1]), (len < 3? 0: arr[2])};}
__m128 vec3_m128(vec3_t vec){return _mm_load_ps((float[4]){vec.x, vec.y, vec.z, 0});}
vec3_t m128_vec3(__m128 m){
	float *out = _aligned_malloc(sizeof(float)* 4, 16);
	_mm_store_ps(out, m);
	float temp[3] = {out[0], out[1], out[2]};
	free(out);
	return (vec3_t){
		.x = temp[0],
		.y = temp[1],
		.z = temp[2],
	};
}
pvec3_t flot_vec3(float *arr, uint32_t len){return (pvec3_t){(len < 1? 0: arr[0]), (len < 2? 0: arr[1]), (len < 3? 0: arr[2])};}
__m128 pvec3_m128(pvec3_t vec){return _mm_load_ps((float[4]){vec.x, vec.y, vec.z, 0});}
pvec3_t m128_pvec3(__m128 m){
	float *out = _aligned_malloc(sizeof(float)* 4, 16);
	_mm_store_ps(out, m);
	float temp[3] = {out[0], out[1], out[2]};
	free(out);
	return (pvec3_t){
		.x = temp[0],
		.y = temp[1],
		.z = temp[2],
	};
}

vec3_t vec3_inverse(vec3_t a){return (vec3_t){ 0-a.x, 0-a.y, 0-a.z};}
vec3_t vec3_rshift(const vec3_t v, uint32_t shf){return (vec3_t){(int)v.x >> shf, (int)v.y >> shf, (int)v.z >> shf};}
vec3_t vec3_lshift(const vec3_t v, uint32_t shf){return (vec3_t){(int)v.x << shf, (int)v.y << shf, (int)v.z << shf};}
pvec3_t pvec3_inverse(pvec3_t a){return (pvec3_t){ 0-a.x, 0-a.y, 0-a.z};}
pvec3_t pvec3_rshift(const pvec3_t v, uint32_t shf){return (pvec3_t){(int)v.x >> shf, (int)v.y >> shf, (int)v.z >> shf};}
pvec3_t pvec3_lshift(const pvec3_t v, uint32_t shf){return (pvec3_t){(int)v.x << shf, (int)v.y << shf, (int)v.z << shf};}

//Not yet bench-marked.
pvec3_t pvec3_add(pvec3_t a, pvec3_t b){return (pvec3_t){a.x+b.x, a.y+b.y, a.z+b.z};}
vec3_t fastvec3_add(vec3_t a, vec3_t b){return m128_vec3(_mm_add_ps(vec3_m128(a), vec3_m128(b)));}
vec3_t vec3_add(vec3_t a, vec3_t b){return (vec3_t){a.x+b.x, a.y+b.y, a.z+b.z};}
vec3_t *vec3_largeadd(vec3_t *vptr, vec3_t *vptr2, const size_t size){
	vec3_t *out = alloca(size* sizeof(vec3_t));
	__m128 _1, _2;
	float temp[4];
	int cc_ =0;
	for(size_t cc =0;cc < size; ++cc){
		switch(cc_){
			case 0:
				_1 =_mm_load_ps((float[4]){vptr[cc].x, (cc < size-1? vptr[cc+1].x: 0), (cc < size-2? vptr[cc+2].x: 0), (cc < size-3? vptr[cc+3].x: 0)});
				_2 =_mm_load_ps((float[4]){vptr2[cc].x, (cc < size-1? vptr2[cc+1].x: 0), (cc < size-2? vptr2[cc+2].x: 0), (cc < size-3? vptr2[cc+3].x: 0)});
				_mm_store_ps(temp, _mm_add_ps(_1, _2));
				out[cc].x = temp[cc];
				if(cc < size-1){out[cc+1].x =temp[cc+1];}
				if(cc < size-2){out[cc+2].x =temp[cc+2];}
				if(cc < size-3){out[cc+3].x =temp[cc+3];}
				cc_++;
				continue;
			case 1:
				_1 =_mm_load_ps((float[4]){vptr[cc].y, (cc < size-1? vptr[cc+1].y: 0), (cc < size-2? vptr[cc+2].y: 0), (cc < size-3? vptr[cc+3].y: 0)});
				_2 =_mm_load_ps((float[4]){vptr2[cc].y, (cc < size-1? vptr2[cc+1].y: 0), (cc < size-2? vptr2[cc+2].y: 0), (cc < size-3? vptr2[cc+3].y: 0)});
				_mm_store_ps(temp, _mm_add_ps(_1, _2));
				out[cc].y = temp[cc];
				if(cc < size-1){out[cc+1].y =temp[cc+1];}
				if(cc < size-2){out[cc+2].y =temp[cc+2];}
				if(cc < size-3){out[cc+3].y =temp[cc+3];}
				cc_++;
				continue;
			case 2:
				_1 =_mm_load_ps((float[4]){vptr[cc].z, (cc < size-1? vptr[cc+1].z: 0), (cc < size-2? vptr[cc+2].z: 0), (cc < size-3? vptr[cc+3].z: 0)});
				_2 =_mm_load_ps((float[4]){vptr2[cc].z, (cc < size-1? vptr2[cc+1].z: 0), (cc < size-2? vptr2[cc+2].z: 0), (cc < size-3? vptr2[cc+3].z: 0)});
				_mm_store_ps(temp, _mm_add_ps(_1, _2));
				out[cc].z = temp[cc];
				if(cc < size-1){out[cc+1].z =temp[cc+1];}
				if(cc < size-2){out[cc+2].z =temp[cc+2];}
				if(cc < size-3){out[cc+3].z =temp[cc+3];}
				cc_= 0;
				continue;
		}
	}
	return out;
}

pvec3_t pvec3_sub(pvec3_t a, pvec3_t b){return (pvec3_t){a.x+b.x, a.y+b.y, a.z+b.z};}
vec3_t vec3_sub(vec3_t a, vec3_t b){return vec3_add(a, vec3_inverse(b));}
vec3_t *vec3_largesub(vec3_t *vptr, vec3_t *vptr2, const size_t size){
	vec3_t *out = alloca(size* sizeof(vec3_t));
	__m128 _1, _2;
	float temp[4];
	int cc_ =0;
	for(size_t cc =0;cc < size; ++cc){
		vptr2[cc] = vec3_inverse(vptr2[cc]);
		switch(cc_){
			case 0:
				_1 =_mm_load_ps((float[4]){vptr[cc].x, (cc < size-1? vptr[cc+1].x: 0), (cc < size-2? vptr[cc+2].x: 0), (cc < size-3? vptr[cc+3].x: 0)});
				_2 =_mm_load_ps((float[4]){vptr2[cc].x, (cc < size-1? vptr2[cc+1].x: 0), (cc < size-2? vptr2[cc+2].x: 0), (cc < size-3? vptr2[cc+3].x: 0)});
				_mm_store_ps(temp, _mm_add_ps(_1, _2));
				out[cc].dir =pvec3_div(pvec3_add(vptr[cc].dir, vptr2[cc].dir), 2);
				out[cc].x = temp[cc];
				if(cc < size-1){out[cc+1].x =temp[cc+1];}
				if(cc < size-2){out[cc+2].x =temp[cc+2];}
				if(cc < size-3){out[cc+3].x =temp[cc+3];}
				continue;
			case 1:
				_1 =_mm_load_ps((float[4]){vptr[cc].y, (cc < size-1? vptr[cc+1].y: 0), (cc < size-2? vptr[cc+2].y: 0), (cc < size-3? vptr[cc+3].y: 0)});
				_2 =_mm_load_ps((float[4]){vptr2[cc].y, (cc < size-1? vptr2[cc+1].y: 0), (cc < size-2? vptr2[cc+2].y: 0), (cc < size-3? vptr2[cc+3].y: 0)});
				_mm_store_ps(temp, _mm_add_ps(_1, _2));
				out[cc].y = temp[cc];
				if(cc < size-1){out[cc+1].y =temp[cc+1];}
				if(cc < size-2){out[cc+2].y =temp[cc+2];}
				if(cc < size-3){out[cc+3].y =temp[cc+3];}
				continue;
			case 2:
				_1 =_mm_load_ps((float[4]){vptr[cc].z, (cc < size-1? vptr[cc+1].z: 0), (cc < size-2? vptr[cc+2].z: 0), (cc < size-3? vptr[cc+3].z: 0)});
				_2 =_mm_load_ps((float[4]){vptr2[cc].z, (cc < size-1? vptr2[cc+1].z: 0), (cc < size-2? vptr2[cc+2].z: 0), (cc < size-3? vptr2[cc+3].z: 0)});
				_mm_store_ps(temp, _mm_add_ps(_1, _2));
				out[cc].z = temp[cc];
				if(cc < size-1){out[cc+1].z =temp[cc+1];}
				if(cc < size-2){out[cc+2].z =temp[cc+2];}
				if(cc < size-3){out[cc+3].z =temp[cc+3];}
				continue;
		}
	}
	return out;
}

pvec3_t pvec3_mul(pvec3_t v, float mul){return (pvec3_t){v.x*mul, v.y*mul, v.z*mul};}
pvec3_t pvvec3_mul(pvec3_t v, pvec3_t mul){return (pvec3_t){v.x*mul.x, v.y*mul.y, v.z*mul.z};}
vec3_t vec3_mul(vec3_t v, float mul){return (vec3_t){v.x*mul, v.y*mul, v.z*mul};}
vec3_t vvec3_mul(vec3_t a, vec3_t b){return (vec3_t){a.x*b.x, a.y*b.y, a.z*b.z, pvec3_avg(a.dir, b.dir)};}

pvec3_t pvvec3_div(pvec3_t a, pvec3_t b){return (pvec3_t){a.x/b.x, a.y/b.y, a.z/b.z};}
pvec3_t pvec3_div(pvec3_t a, float b){return (pvec3_t){a.x/b, a.y/b, a.z/b};}
vec3_t vec3_div(vec3_t a, float div){return (vec3_t){a.x/div, a.y/div, a.z/div, a.dir};}
vec3_t vvec3_div(vec3_t a, vec3_t b){return (vec3_t){a.x/b.x, a.y/b.y, a.z/b.z, pvec3_avg(a.dir, b.dir)};}

pvec3_t pvec3_avg(pvec3_t a, pvec3_t b){return pvec3_div(pvvec3_add(a, b), 2);}