#include "./engine/Public.h"
#include <immintrin.h>
#include <xmmintrin.h>

#pragma region Vector3
vec3_t flot_vec3(float *arr, uint32_t len){return (vec3_t){(len < 1? 0: arr[0]), (len < 2? 0: arr[1]), (len < 3? 0: arr[2])};}
__m128 vec3_m128(vec3_t vec){return _mm_load_ps((float[4]){vec.x, vec.y, vec.z, 0});}
vec3_t m128_vec3(__m128 m){
	float *out = _aligned_malloc(4* sizeof(float), 16);
	_mm_store_ps(out, m);
	float temp[3] = {out[0], out[1], out[2]};
	free(out);
	return (vec3_t){
		.x = temp[0],
		.y = temp[1],
		.z = temp[2],
	};
}

vec3_t vec3_inverse(vec3_t a){return (vec3_t){ 0-a.x, 0-a.y, 0-a.z};}
vec3_t vec3_rshift(const vec3_t v, uint32_t shf){return (vec3_t){(int)v.x >> shf, (int)v.y >> shf, (int)v.z >> shf};}
vec3_t vec3_lshift(const vec3_t v, uint32_t shf){return (vec3_t){(int)v.x << shf, (int)v.y << shf, (int)v.z << shf};}

//Not yet bench-marked.
vec3_t vec3_add(vec3_t a, vec3_t b){return m128_vec3(_mm_add_ps(vec3_m128(a), vec3_m128(b)));}
// vec3_t vec3_add(vec3_t a, vec3_t b){return (vec3_t){a.x+b.x, a.y+b.y, a.z+b.z};}
vec3_t *vec3_largeadd(vec3_t *vptr, vec3_t *vptr2, const size_t size){
	vec3_t *out = _aligned_malloc(size* sizeof(vec3_t), 16);
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
				break;
			case 1:
				_1 =_mm_load_ps((float[4]){vptr[cc].y, (cc < size-1? vptr[cc+1].y: 0), (cc < size-2? vptr[cc+2].y: 0), (cc < size-3? vptr[cc+3].y: 0)});
				_2 =_mm_load_ps((float[4]){vptr2[cc].y, (cc < size-1? vptr2[cc+1].y: 0), (cc < size-2? vptr2[cc+2].y: 0), (cc < size-3? vptr2[cc+3].y: 0)});
				_mm_store_ps(temp, _mm_add_ps(_1, _2));
				out[cc].y = temp[cc];
				if(cc < size-1){out[cc+1].y =temp[cc+1];}
				if(cc < size-2){out[cc+2].y =temp[cc+2];}
				if(cc < size-3){out[cc+3].y =temp[cc+3];}
				cc_++;
				break;
			case 2:
				_1 =_mm_load_ps((float[4]){vptr[cc].z, (cc < size-1? vptr[cc+1].z: 0), (cc < size-2? vptr[cc+2].z: 0), (cc < size-3? vptr[cc+3].z: 0)});
				_2 =_mm_load_ps((float[4]){vptr2[cc].z, (cc < size-1? vptr2[cc+1].z: 0), (cc < size-2? vptr2[cc+2].z: 0), (cc < size-3? vptr2[cc+3].z: 0)});
				_mm_store_ps(temp, _mm_add_ps(_1, _2));
				out[cc].z = temp[cc];
				if(cc < size-1){out[cc+1].z =temp[cc+1];}
				if(cc < size-2){out[cc+2].z =temp[cc+2];}
				if(cc < size-3){out[cc+3].z =temp[cc+3];}
				cc_= 0;
				break;
		}
	}
	return out;
}
vec3_t vec3_mul(vec3_t v, float mul){return (vec3_t){v.x*mul, v.y*mul, v.z*mul};}
vec3_t vvec3_mul(vec3_t a, vec3_t b){return (vec3_t){a.x*b.x, a.y*b.y, a.z*b.z, pvec3_avg(a.dir, b.dir)};}
vec3_t vec3_div(vec3_t a, float div){return (vec3_t){a.x/div, a.y/div, a.z/div, a.dir};}
vec3_t vvec3_div(vec3_t a, vec3_t b){return (vec3_t){a.x/b.x, a.y/b.y, a.z/b.z, pvec3_avg(a.dir, b.dir)};}
vec3_t vec_cross(vec3_t a, vec3_t b){return (vec3_t){(a.y* b.x)- (a.z* b.y), (a.z* b.x)- (a.x* b.z), (a.x* b.y)- (a.y* b.x)};}
float vec_mag(vec3_t v){return sqrtf((v.x*v.x)+ (v.y*v.y)+ (v.z*v.z));}
vec3_t vec_normalise(vec3_t v){return vec3_div(v, vec_mag(v));}
float vec3_dot(vec3_t a, vec3_t b){return (a.x*b.x)+ (a.y*b.y)+ (a.z*b.z);}
float vec3_fdot(vec3_t a, float *b){return (a.x*b[0])+ (a.y*b[1])+ (a.z*b[2]);}
float *vec3_largedot(vec3_t *aptr, vec3_t *bptr, size_t len){
	float *out = _aligned_malloc(len * sizeof(float), 16);
	__m128 _1, _2, _3;
	float temp[4];
	uint8_t cc_;
	for(size_t cc =0; cc < len; cc++){
		_1 =_mm_load_ps((float[4]){aptr[cc].x* bptr[cc].x, (cc < len-1? aptr[cc+1].x* bptr[cc+1].x: 0), (cc < len-2? aptr[cc+2].x* bptr[cc+2].x: 0), (cc < len-3? aptr[cc+3].x* bptr[cc+3].x: 0)});
		_2 =_mm_load_ps((float[4]){aptr[cc].y* bptr[cc].y, (cc < len-1? aptr[cc+1].y* bptr[cc+1].y: 0), (cc < len-2? aptr[cc+2].y* bptr[cc+2].y: 0), (cc < len-3? aptr[cc+3].y* bptr[cc+3].y: 0)});
		_3 =_mm_load_ps((float[4]){aptr[cc].z* bptr[cc].z, (cc < len-1? aptr[cc+1].z* bptr[cc+1].z: 0), (cc < len-2? aptr[cc+2].z* bptr[cc+2].z: 0), (cc < len-3? aptr[cc+3].z* bptr[cc+3].z: 0)});
		_mm_store_ps(out+cc, _mm_add_ps(_1, _mm_add_ps(_2, _3)));
	}
	return out;
}
/// @brief Compute multiple dot product operations with a Vector3 and floats(that are treated as Vector3). 
/// It functions by computing each multiplication locally and using SIMD to add the results, The SIMD __m128s are cfilled on a per-axis basis, 
/// where one axis fills the whole of the __m128 and 4 floats can be handled.
/// @param aptr The Vectors to be dotted with.
/// @param bptr The floats that will be treated as Vector3s.
/// @param bptrlen The length of the bptr array 
/// @param aptrlen The length of the aptr array.
/// @remark bptrlen MUST BE AT LEAST A MULTIPLE OF 3,
/// The length of bptr must at least be a multiple of 3, if aptr or bptr are longer then the Program will adapt itself to ensure all Vector3s have a dot function applied.
/// @return The dot product result of each Vector3 and float(Vector3), will be the same Length as aptrlen.
float *vec3_largefdot(vec3_t *aptr, float *bptr, const size_t bptrlen, size_t aptrlen){
	if(bptrlen%3 != 0){return NULL;}
	float *out = _aligned_malloc(aptrlen * sizeof(float), 16);
	__m128 _1, _2, _3;
	float temp[4];
	size_t cc_12 =0;
	for(size_t cc_4 =0; cc_4 < aptrlen && cc_12 < bptrlen; cc_4=(cc_4 == aptrlen?0: cc_4+4), cc_12=(cc_12 == bptrlen?0: cc_12+12)){
		//X
		_1 =_mm_load_ps((float[4]){aptr[cc_4].x* bptr[cc_12], (cc_4 < aptrlen-1? aptr[cc_4+1].x* bptr[cc_12+1]: 0), (cc_4 < aptrlen-2? aptr[cc_4+2].x* bptr[cc_12+2]: 0), (cc_4 < aptrlen-3? aptr[cc_4+3].x* bptr[cc_12+3]: 0)});
		//Y
		_2 =_mm_load_ps((float[4]){aptr[cc_4].y* bptr[cc_12+1], (cc_4 < aptrlen-1? aptr[cc_4+1].y* bptr[cc_12+2]: 0), (cc_4 < aptrlen-2? aptr[cc_4+2].y* bptr[cc_12+3]: 0), (cc_4 < aptrlen-3? aptr[cc_4+3].y* bptr[cc_12+4]: 0)});
		//Z
		_3 =_mm_load_ps((float[4]){aptr[cc_4].z* bptr[cc_12+2], (cc_4 < aptrlen-1? aptr[cc_4+1].z* bptr[cc_12+3]: 0), (cc_4 < aptrlen-2? aptr[cc_4+2].z* bptr[cc_12+4]: 0), (cc_4 < aptrlen-3? aptr[cc_4+3].z* bptr[cc_12+5]: 0)});
		_mm_store_ps(out+cc_4, _mm_add_ps(_1, _mm_add_ps(_2, _3)));
	}
	return out;
}


#pragma region Pure Vector3
	pvec3_t flot_pvec3(float *arr, uint32_t len){return (pvec3_t){(len < 1? 0: arr[0]), (len < 2? 0: arr[1]), (len < 3? 0: arr[2])};}
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
pvec3_t pvec3_inverse(pvec3_t a){return (pvec3_t){ 0-a.x, 0-a.y, 0-a.z};}
pvec3_t pvec3_rshift(const pvec3_t v, uint32_t shf){return (pvec3_t){(int)v.x >> shf, (int)v.y >> shf, (int)v.z >> shf};}
pvec3_t pvec3_lshift(const pvec3_t v, uint32_t shf){return (pvec3_t){(int)v.x << shf, (int)v.y << shf, (int)v.z << shf};}
pvec3_t pvec3_add(pvec3_t a, pvec3_t b){return (pvec3_t){a.x+b.x, a.y+b.y, a.z+b.z};}
pvec3_t pvec3_sub(pvec3_t a, pvec3_t b){return (pvec3_t){a.x+b.x, a.y+b.y, a.z+b.z};}
vec3_t vec3_sub(vec3_t a, vec3_t b){return vec3_add(a, vec3_inverse(b));}
vec3_t *vec3_largesub(vec3_t *vptr, vec3_t *vptr2, const size_t size){
	vec3_t *out = _aligned_malloc(size* sizeof(vec3_t), 16);
	__m128 _1, _2;
	float temp[4];
	uint8_t cc_ =0;
	for(size_t cc =0;cc < size; ++cc){
		switch(cc_){
			out[cc].dir =pvec3_avg(vptr[cc].dir, vptr2[cc].dir);
			case 0:
				_1 =_mm_load_ps((float[4]){vptr[cc].x, (cc < size-1? vptr[cc+1].x: 0), (cc < size-2? vptr[cc+2].x: 0), (cc < size-3? vptr[cc+3].x: 0)});
				_2 =_mm_load_ps((float[4]){vptr2[cc].x, (cc < size-1? vptr2[cc+1].x: 0), (cc < size-2? vptr2[cc+2].x: 0), (cc < size-3? vptr2[cc+3].x: 0)});
				_mm_store_ps(temp, _mm_sub_ps(_1, _2));
				out[cc].x = temp[cc];
				if(cc < size-1){out[cc+1].x =temp[cc+1];}
				if(cc < size-2){out[cc+2].x =temp[cc+2];}
				if(cc < size-3){out[cc+3].x =temp[cc+3];}
				break;
			case 1:
				_1 =_mm_load_ps((float[4]){vptr[cc].y, (cc < size-1? vptr[cc+1].y: 0), (cc < size-2? vptr[cc+2].y: 0), (cc < size-3? vptr[cc+3].y: 0)});
				_2 =_mm_load_ps((float[4]){vptr2[cc].y, (cc < size-1? vptr2[cc+1].y: 0), (cc < size-2? vptr2[cc+2].y: 0), (cc < size-3? vptr2[cc+3].y: 0)});
				_mm_store_ps(temp, _mm_sub_ps(_1, _2));
				out[cc].y = temp[cc];
				if(cc < size-1){out[cc+1].y =temp[cc+1];}
				if(cc < size-2){out[cc+2].y =temp[cc+2];}
				if(cc < size-3){out[cc+3].y =temp[cc+3];}
				break;
			case 2:
				_1 =_mm_load_ps((float[4]){vptr[cc].z, (cc < size-1? vptr[cc+1].z: 0), (cc < size-2? vptr[cc+2].z: 0), (cc < size-3? vptr[cc+3].z: 0)});
				_2 =_mm_load_ps((float[4]){vptr2[cc].z, (cc < size-1? vptr2[cc+1].z: 0), (cc < size-2? vptr2[cc+2].z: 0), (cc < size-3? vptr2[cc+3].z: 0)});
				_mm_store_ps(temp, _mm_sub_ps(_1, _2));
				out[cc].z = temp[cc];
				if(cc < size-1){out[cc+1].z =temp[cc+1];}
				if(cc < size-2){out[cc+2].z =temp[cc+2];}
				if(cc < size-3){out[cc+3].z =temp[cc+3];}
				break;
		}
	}
	return out;
}

pvec3_t pvvec3_div(pvec3_t a, pvec3_t b){return (pvec3_t){a.x/b.x, a.y/b.y, a.z/b.z};}
pvec3_t pvec3_div(pvec3_t a, float b){return (pvec3_t){a.x/b, a.y/b, a.z/b};}

float pvec3_dot(pvec3_t a, pvec3_t b){return (a.x*b.x)+ (a.y*b.y)+ (a.z*b.z);}
float pvec3_fdot(pvec3_t a, float *b){return (a.x*b[0])+ (a.y*b[1])+ (a.z*b[2]);}

pvec3_t pvec_cross(pvec3_t a, pvec3_t b){return (pvec3_t){(a.y* b.x)- (a.z* b.y), (a.z* b.x)- (a.x* b.z), (a.x* b.y)- (a.y* b.x)};}
float pvec_mag(pvec3_t pv){return sqrtf((pv.x*pv.x)+ (pv.y*pv.y)+ (pv.z*pv.z));}
pvec3_t pvec_normalise(pvec3_t pv){return pvec3_div(pv, pvec_mag(pv));}

pvec3_t pvec3_mul(pvec3_t v, float mul){return (pvec3_t){v.x*mul, v.y*mul, v.z*mul};}
pvec3_t pvvec3_mul(pvec3_t v, pvec3_t mul){return (pvec3_t){v.x*mul.x, v.y*mul.y, v.z*mul.z};}


pvec3_t pvec3_avg(pvec3_t a, pvec3_t b){return pvec3_div(pvec3_add(a, b), 2);}
#pragma endregion

#pragma region Matrices
matrix_t *matrix_init(float *data, size_t arrsize, size_t xsize, size_t ysize){
	matrix_t *matrix = alloca(sizeof(matrix_t));
	matrix->_mesh = alloca(sizeof(float *)* ysize);
	size_t arrcc =0;
	for(int y =0; y < ysize; y++){
		matrix->_mesh[y] = alloca(sizeof(float)* xsize);
		for(int x =0; x < xsize; x++){
			matrix->_mesh[y][x] = data[arrcc];
			arrcc++;
			if(arrcc >= arrsize){return matrix;}
		}
	}
}

vec3_t *matrix_vec3mul(matrix_t *m, const size_t mlen, vec3_t *v, const size_t vlen){
	vec3_t *out = alloca(sizeof(vec3_t)* vlen);
	size_t cc =0;
	//Handles it as for each vertex, handle the matrices.
	float *columns = alloca(sizeof(float));
	for(size_t mcc =0; mcc < mlen; ++mcc){
		if(m[mcc].ysize != 3){break;}
		columns = realloca(sizeof(float)* m[cc].xsize);
		for(size_t mx =0; mx < m[mcc].xsize; mx+=3){
			//For each X component (because The Y has to be 3) all the values at Y are put into Column.
			columns[mx] = m[mcc]._mesh[mx][0]; columns[mx+1] = m[mcc]._mesh[mx][1]; columns[mx+1] = m[mcc]._mesh[mx][2];
		}
		//Compile each Matrix once then transform each Vector3 by it.
		for(size_t vcc =0; vcc < vlen; ++vcc){
			column = vec3_largefdot(v, columns, );
			//Avoid any wrongly sized matrices.
		}
	}
	free(columns);
}


#pragma endregion