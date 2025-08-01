#include "./engine/Public.h"
#include <immintrin.h>
#include <xmmintrin.h>

#pragma region Vector3
/// @brief Convert a float array to a vec3_t
/// @param arr The array to be converted.
/// @param len The length of the array, for any vec3_t values outside it, they are defaulted to 0.
/// @return The decoded vec3_t.
vec3_t flot_vec3(float *arr, uint32_t len){return (vec3_t){(len < 1? 0: arr[0]), (len < 2? 0: arr[1]), (len < 3? 0: arr[2]), {(len < 4? 0: arr[3]), (len < 5? 0: arr[4]), (len < 6? 0: arr[5])}};}
/// @brief Decode a vec3_t. The pure x, y, z values to a __m128 SIMD intrinsic.
/// @param vec The vec3_t to be decoded.
/// @return A decoded vec3_t.
__m128 vec3_m128(vec3_t vec){return _mm_load_ps((float[4]){vec.x, vec.y, vec.z, 0});}
/// @brief Decode a vec3_t from a __m128 SIMD intrinsic.
/// @param m The __m128 SIMD intrinsic to be decoded.
/// @return A decoded vec3_T, the pure x, y, z values.
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

/// @brief Compare to vec3_t values. 
/// @return If the same: true, otherwise false.
bool vec3_cmp(vec3_t a, vec3_t b){return (a.x == b.x)&& (a.y == b.y)&& (a.z == b.z);}
/// @brief Compare each element in a vec3_t array against all other elements in the array.
/// @param v The array of vec3_ts to be compared.
/// @param len The length of the array.
/// @return If the same: true, otherwise false.
bool vec3_cmplong(vec3_t *v, const size_t len){
	for(size_t cc= 0; cc < len; ++cc){
		for(size_t cc_ =cc; cc_ < len; ++cc_){
			if(!vec3_cmp(v[cc], v[cc_])){return false;}
		}
	}
}
/// @brief Invert the sign bit of each x, y, z value of a vec3_t. 
vec3_t vec3_inverse(vec3_t a){return (vec3_t){ 0-a.x, 0-a.y, 0-a.z};}
/// @brief Right-Shift all the pure x, y, z values in a vec3_t, converting them into integers.
/// @param v The vec3_t to be r-shifted.
/// @param shf The number of bits to be shifted by.
/// @return The vec3_t with it's x, y, and z values right shifted.
vec3_t vec3_rshift(const vec3_t v, uint32_t shf){return (vec3_t){(int)v.x >> shf, (int)v.y >> shf, (int)v.z >> shf};}
/// @brief Left-Shift all the pure x, y, z values in a vec3_t, converting them into integers.
/// @param v The vec3_t to be l-shifted.
/// @param shf The number of bits to be shifted by.
/// @return The vec3_t with it's x, y, and z values left shifted.
vec3_t vec3_lshift(const vec3_t v, uint32_t shf){return (vec3_t){(int)v.x << shf, (int)v.y << shf, (int)v.z << shf};}
/// @brief Calculate the average of every vec3_t in the array.
/// @param vs The array that the average should be calculated from.
/// @param len The length of the array.
/// @return The average of every element in the array.
vec3_t vec3_avg(vec3_t *vs, size_t len){
	vec3_t v;
	for(size_t cc =0; cc < len; ++cc){v = vec3_add(v, vs[cc]);}
	return vec3_div(v, len);
}

/// @brief Return the addition of 2 vec3_t values, thier pure x, y, z values.
/// @return The added vec3_t.
vec3_t vec3_add(vec3_t a, vec3_t b){return m128_vec3(_mm_add_ps(vec3_m128(a), vec3_m128(b)));}
/// @brief Add together each element in two vec3_t arays, this only affects the pure x, y, z values.
/// @param size The length of the arrays.
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
/// @brief Multiply a vec3_t by a float.
/// @return The multipled vec3_t.
vec3_t vec3_mul(vec3_t v, float mul){return (vec3_t){v.x*mul, v.y*mul, v.z*mul};}
/// @brief Multiply two vec3_ts together, the dir is calculated as an average of the two.
vec3_t vvec3_mul(vec3_t a, vec3_t b){return (vec3_t){a.x*b.x, a.y*b.y, a.z*b.z, pvec3_avg2(a.dir, b.dir)};}
/// @brief Divide two vec3_ts together.
/// @return The divided vec3_t.
vec3_t vec3_div(vec3_t a, float div){return (vec3_t){a.x/div, a.y/div, a.z/div, a.dir};}
/// @brief Divide two vec3_ts together, the dir is calculated as an average of the two.
vec3_t vvec3_div(vec3_t a, vec3_t b){return (vec3_t){a.x/b.x, a.y/b.y, a.z/b.z, pvec3_avg2(a.dir, b.dir)};}
/// @brief Solve the cross-product of two vec3_t values.
/// @return A vec3_t perpendicular to both inputted vec3_t vaues.
vec3_t vec_cross(vec3_t a, vec3_t b){return (vec3_t){(a.y* b.x)- (a.z* b.y), (a.z* b.x)- (a.x* b.z), (a.x* b.y)- (a.y* b.x)};}
/// @brief Get the magnitude of a vec3_t value.
/// @return the magnitude of the inputted vec3_t value.
float vec_mag(vec3_t v){return sqrtf((v.x*v.x)+ (v.y*v.y)+ (v.z*v.z));}
/// @brief Normalise a vec3_t, so that it's magnitude is 1.
/// @return The normalised vec3_t.
vec3_t vec_normalise(vec3_t v){return vec3_div(v, vec_mag(v));}
/// @brief Calculate the dot product of two vec3_t values.
/// @return The dot product of two vec3_t values.
float vec3_dot(vec3_t a, vec3_t b){return (a.x*b.x)+ (a.y*b.y)+ (a.z*b.z);}
/// @brief Calculate the dot product of a vec3_t and float array. The array is treated as a vec3_t.
/// @return The dot product of the inputted value.
float vec3_fdot(vec3_t a, float *b){return (a.x*b[0])+ (a.y*b[1])+ (a.z*b[2]);}
/// @brief Calculate the dot product of multiple vec3_t values.
/// @param aptr The first vec3_t array.
/// @param bptr The second vec3_t array.
/// @param len The length of both arrays.
/// @return The dot product of each element in each array.
float *vec3_largedot(vec3_t *aptr, vec3_t *bptr, size_t len){
	float *out = _aligned_malloc(len * sizeof(float), 16);
	__m128 _1, _2, _3;
	float temp[4];
	for(size_t cc =0; cc < len; cc++){
		_1 =_mm_load_ps((float[4]){aptr[cc].x* bptr[cc].x,
			(cc < len-1? aptr[cc+1].x* bptr[cc+1].x: 0),
			(cc < len-2? aptr[cc+2].x* bptr[cc+2].x: 0),
			(cc < len-3? aptr[cc+3].x* bptr[cc+3].x: 0)});

		_2 =_mm_load_ps((float[4]){aptr[cc].y* bptr[cc].y,
			(cc < len-1? aptr[cc+1].y* bptr[cc+1].y: 0),
			(cc < len-2? aptr[cc+2].y* bptr[cc+2].y: 0),
			(cc < len-3? aptr[cc+3].y* bptr[cc+3].y: 0)});

		_3 =_mm_load_ps((float[4]){aptr[cc].z* bptr[cc].z,
			(cc < len-1? aptr[cc+1].z* bptr[cc+1].z: 0),
			(cc < len-2? aptr[cc+2].z* bptr[cc+2].z: 0),
			(cc < len-3? aptr[cc+3].z* bptr[cc+3].z: 0)});

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
/// @remark bptrlen MUST BE AT LEAST A MULTIPLE OF 3, as well as ,
/// The length of bptr must at least be a multiple of 3.
/// @return The dot product result of each Vector3 and float(Vector3), will be the same Length as bptrlen. NULL if any of the previous condition are not met.
float *vec3_largefdot(vec3_t *aptr, float *bptr, const size_t bptrlen, size_t aptrlen){
	if(bptrlen%3 != 0 && bptrlen/aptrlen != 3){return NULL;} 
	float *out = _aligned_malloc(bptrlen * sizeof(float), 16);
	__m128 _1, _2, _3;
	size_t aptr_cc;
	for(size_t bptr_cc =0;aptr_cc < aptrlen; bptr_cc=(bptr_cc >= bptrlen? 0: bptr_cc+3), aptr_cc++){
		//Now either will loop around
		_1 =_mm_load_ps((float[4]){aptr[aptr_cc].x* bptr[bptr_cc],
			(aptr_cc < aptrlen-1? aptr[aptr_cc+1].x* (bptr_cc+3 < bptrlen ?bptr[bptr_cc+3]: 0): 0) ,
			(aptr_cc < aptrlen-2? aptr[aptr_cc+2].x* (bptr_cc+6 < bptrlen ?bptr[bptr_cc+6]: 0): 0),
			(aptr_cc < aptrlen-3? aptr[aptr_cc+3].x* (bptr_cc+9 < bptrlen ?bptr[bptr_cc+9]: 0): 0)});
		
		_2 =_mm_load_ps((float[4]){aptr[aptr_cc].y* (bptr_cc+1 < bptrlen ?bptr[bptr_cc+1]: 0),
			(aptr_cc < aptrlen-1? aptr[aptr_cc+1].y* (bptr_cc+4 < bptrlen ?bptr[bptr_cc+4]: 0): 0),
			(aptr_cc < aptrlen-2? aptr[aptr_cc+2].y* (bptr_cc+7 < bptrlen ?bptr[bptr_cc+7]: 0): 0),
			(aptr_cc < aptrlen-3? aptr[aptr_cc+3].y* (bptr_cc+10 < bptrlen ?bptr[bptr_cc+10]: 0): 0)});
			
		_3 =_mm_load_ps((float[4]){aptr[aptr_cc].z* (bptr_cc+2 < bptrlen ?bptr[bptr_cc+2]: 0),
			(aptr_cc < aptrlen-1? aptr[aptr_cc+1].z* (bptr_cc+5 < bptrlen ?bptr[bptr_cc+5]: 0): 0),
			(aptr_cc < aptrlen-2? aptr[aptr_cc+2].z* (bptr_cc+8 < bptrlen ?bptr[bptr_cc+8]: 0): 0),
			(aptr_cc < aptrlen-3? aptr[aptr_cc+3].z* (bptr_cc+11 < bptrlen ?bptr[bptr_cc+11]: 0): 0)});
			_mm_store_ps(out+aptr_cc, _mm_add_ps(_1, _mm_add_ps(_2, _3)));
		}
		return out;
	}

/// @brief Subtract two vec3_t values.
/// @return The vec3_t a-b.
vec3_t vec3_sub(vec3_t a, vec3_t b){return vec3_add(a, vec3_inverse(b));}
/// @brief Subtract from each element of vptr, each element of vptr2.
/// @param size the length of both elements.
/// @return The subtracted vec3_t vptr[X]- vptr2[X]
vec3_t *vec3_largesub(vec3_t *vptr, vec3_t *vptr2, const size_t size){
	vec3_t *out = _aligned_malloc(size* sizeof(vec3_t), 16);
	__m128 _1, _2;
	float temp[4];
	uint8_t cc_ =0;
	for(size_t cc =0;cc < size; ++cc){
		switch(cc_){
			out[cc].dir =pvec3_avg2(vptr[cc].dir, vptr2[cc].dir);
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

/// @brief Rotate each vec3_t in an array by rot.
/// @param vec The array of vec3_ts to be rotated.
/// @param apex The point they should be rotated around.
/// @param len The length of the vec3_t array.
/// @param rot The rotation that should be applied.
/// @return The roated vec3_ts.
vec3_t *vec3_rot(vec3_t *vec, vec3_t *apex, const size_t len, const vec3_t rot){
	const float xsin= sinf(rot.x), ysin= sinf(rot.y), zsin= sinf(rot.z);
	const float xcos= cosf(rot.x), ycos= cosf(rot.y), zcos= cosf(rot.z);
	const matrix_t *rotm = matrix_init(
		//Define a stack array, and just pass it's stack address.
		&(float[9]){
			(ycos* xcos), (zsin* ysin* xcos)-(zcos* xsin), (zcos* ysin* xcos)+(zsin* xsin),
			(ycos* xcos), (zsin* ysin* xcos)+(zcos* xsin), (zcos* ysin* xcos)-(zsin* xsin),
			-ysin, (zsin* ycos), (zcos* ycos)},
		9, 3, 3);
	vec3_t *out= vec3_largesub(vec, apex, len);
	matrix_vec3mul(rotm, 1, out, len);
	matrix_free(rotm);
	vec3_largeadd(out, apex, len);
	return out;
}
	
	
#pragma region Pure Vector3
/// @brief Decode a pvec3_t from a float array.
/// @param arr The array the pvec3_t should be decoded from.
/// @param len The length of the array.
/// @return The decoded pvec3_t.
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
/// @brief Invert the sign bit of the pvec3_t x,y, and z values.
pvec3_t pvec3_inverse(pvec3_t a){return (pvec3_t){ 0-a.x, 0-a.y, 0-a.z};}
/// @brief Right shift all the values in v by shf.
/// @param shf How much the values should be shifted by.
/// @return The pvec3_t with all it's values shifted.
pvec3_t pvec3_rshift(const pvec3_t v, uint32_t shf){return (pvec3_t){(int)v.x >> shf, (int)v.y >> shf, (int)v.z >> shf};}
/// @brief Left shift all the values in v by shf.
/// @param shf How much the values should be shifted by.
/// @return The pvec3_t with all it's values shifted.
pvec3_t pvec3_lshift(const pvec3_t v, uint32_t shf){return (pvec3_t){(int)v.x << shf, (int)v.y << shf, (int)v.z << shf};}
/// @brief Add two pvec3_t values.
/// @return The added pvec3_t.
pvec3_t pvec3_add(pvec3_t a, pvec3_t b){return (pvec3_t){a.x+b.x, a.y+b.y, a.z+b.z};}
/// @brief Subtract two pvec3_t values.
/// @return The subtracted pvec3_t.
pvec3_t pvec3_sub(pvec3_t a, pvec3_t b){return (pvec3_t){a.x+b.x, a.y+b.y, a.z+b.z};}

/// @brief Divide two pvec3_t values.
/// @return The divided pvec3_t.
pvec3_t pvvec3_div(pvec3_t a, pvec3_t b){return (pvec3_t){a.x/b.x, a.y/b.y, a.z/b.z};}
/// @brief Subtract pvec3_t value by a float.
/// @return The divided pvec3_t.
pvec3_t pvec3_div(pvec3_t a, float b){return (pvec3_t){a.x/b, a.y/b, a.z/b};}

/// @brief Compute a dot calculation of two pvec3_t values.
float pvec3_dot(pvec3_t a, pvec3_t b){return (a.x*b.x)+ (a.y*b.y)+ (a.z*b.z);}
/// @brief Compute a dot calculation of a pvec3_t value and a float[3] that will be treated like a pvec3_t.
float pvec3_fdot(pvec3_t a, float *b){return (a.x*b[0])+ (a.y*b[1])+ (a.z*b[2]);}

/// @brief Compute a cross product calculation between two pvec3_t values.
pvec3_t pvec_cross(pvec3_t a, pvec3_t b){return (pvec3_t){(a.y* b.x)- (a.z* b.y), (a.z* b.x)- (a.x* b.z), (a.x* b.y)- (a.y* b.x)};}
/// @brief Get the magnitude of a pvec3_t value.
/// @return The magnitude of the pvec3_t.
float pvec_mag(pvec3_t pv){return sqrtf((pv.x*pv.x)+ (pv.y*pv.y)+ (pv.z*pv.z));}
/// @brief Normalise a pvec3_t value.
/// @return The normalised pvec3_t.
pvec3_t pvec_normalise(pvec3_t pv){return pvec3_div(pv, pvec_mag(pv));}

/// @brief Multiply a pvec3_t value by a float value.
/// @return The multiplied pvec3_t.
pvec3_t pvec3_mul(pvec3_t v, float mul){return (pvec3_t){v.x*mul, v.y*mul, v.z*mul};}
/// @brief Multiply two pvec3_t values.
/// @return The multiplied pvec3_t.
pvec3_t pvvec3_mul(pvec3_t v, pvec3_t mul){return (pvec3_t){v.x*mul.x, v.y*mul.y, v.z*mul.z};}

/// @brief Calculate the average between multiple pvec3_t values.
/// @param a The array of pvec3_t values that the average will be calculated from.
/// @param len The length of the aray a.
/// @return The average between each value.
pvec3_t pvec3_avg(pvec3_t *a, size_t len){
	pvec3_t pv;
	for(int cc =0; cc < len; ++cc){pv = pvec3_add(pv, a[cc]);}
	return pvec3_div(pv, len);
}
/// @brief Compute the average between two pvec3_t values.
pvec3_t pvec3_avg2(pvec3_t a, pvec3_t b){return pvec3_div(pvec3_add(a, b), 2);}
#pragma endregion

#pragma region Matrices
/// @brief Initialise a matrix with an array of Data.
/// @param data The array, or value that the matrix should be initialised with.
/// @param arrsize the size of the array.
/// @param xsize The proposed x-size of the array.
/// @param ysize The proposed y-size of the array.
/// @return The initialised matrix.
/// @remarks The array's values are recursively called to fill the array.
/// This function can handle single value pointers, requiring the arrsize to be set to 1.
matrix_t *matrix_init(float *data, size_t arrsize, size_t xsize, size_t ysize){
	matrix_t *matrix = alloca(sizeof(matrix_t));
	matrix->_matrix = alloca(sizeof(float *)* ysize);
	size_t arrcc =0;
	for(int y =0; y < ysize; y++){
		matrix->_matrix[y] = alloca(sizeof(float)* xsize);
		for(int x =0; x < xsize; x++){
			matrix->_matrix[y][x] = data[arrcc];
			arrcc++;
			if(arrcc >= arrsize){arrcc =0;}
		}
	}
	return matrix;
}

/// @brief Flatten a matrix into a 1D float array.
/// @param m The matrix to be flattened.
/// @remarks This function fills the 1D array in the order x, y.
float *matrix_flatten(const matrix_t *m){
	float *out = alloca(sizeof(float)* m->xsize* m->ysize);
	size_t cc =0;
	for(size_t y =0; y < m->ysize; ++y){
		for(size_t x =0; x < m->xsize; ++x){
			out[cc] = m->_matrix[y][x];
			++cc;
		}
	}
	return out;
}

/// @brief Flatten a matrix into a 1D float array.
/// @param m The matrix to be flattened.
/// @remarks This function fills the 1D array in the order y, x.
float *matrix_flattenr(const matrix_t *m){
	float *out = alloca(sizeof(float)* m->xsize* m->ysize);
	size_t cc =0;
	for(size_t y =0; y < m->ysize; ++y){
		for(size_t x =0; x < m->xsize; ++x){
			out[cc] = m->_matrix[x][y];
			++cc;
		}
	}
	return out;
}

/// @brief Muliply an array of matrix_t values by vec3_ts.
/// @param m The array of metrices to be multipled against.
/// @param mlen The length of the matrix_t array.
/// @param v The array of vec3_t values.
/// @param vlen The length of the vec3_t array.
/// @return The vec3_t array but with each vec3_t; specifically thire x, y, z values, multiplied by a Matrix.
vec3_t *matrix_vec3mul(matrix_t *m, const size_t mlen, vec3_t *v, const size_t vlen){
	vec3_t *out = alloca(sizeof(vec3_t)* vlen);
	size_t cc =0;
	//Handles it as for each vertex, handle the matrices.
	float *columns;
	for(size_t mcc =0; mcc < mlen; ++mcc){
		//Skip invalid Meshes.
		if(m[mcc].ysize != 3){continue;}
		//Flatten the Matrix so it can be computed properly.
		columns = matrix_flattenr(&m[mcc]);
		for(size_t mx =0; mx < m[mcc].xsize; mx++){
			//For each X component (because The Y has to be 3) all the values at Y are put into Column.
			columns[mx] = m[mcc]._matrix[mx][0]; columns[mx+1] = m[mcc]._matrix[mx][1]; columns[mx+1] = m[mcc]._matrix[mx][2];
		}
		//Compile each Matrix once then transform each Vector3 by it.
		for(size_t vcc =0; vcc < vlen; ++vcc){
			float *value = vec3_largefdot(&v[vcc], columns, m[cc].xsize* 3, 1);
			out[cc] =(vec3_t){value[0], value[1], value[2], v[vcc].dir};
			++cc;
		}
	}
	free(columns);
	for(int cc =0; cc < vlen; ++cc){printf("%d, %d, %d", out[cc].x, out[cc].y, out[cc].z);}
	return out;
}

/// @brief Multiply a matrix_t pointer by another matrix_t pointer.
/// @return The two inputted matrices multiplied against each other.
matrix_t *matrix_mul(matrix_t *m, matrix_t *m2){
	if(m->ysize == m2->xsize){
		const float _0 =0;
		matrix_t *result =matrix_init(&_0, 1, m2->xsize, m->ysize);
		for(int i = 0; i < m->ysize; i++){ // Rows of the result matrix
        	for (int j = 0; j < m2->xsize; j++) { // Columns of the result matrix
				m->_matrix[i][j] = 0.0f; // Initialize element to 0
				for (int k = 0; k < m->xsize; k++) { //  Intermediate summation
					m->_matrix[i][j] += m->_matrix[i][k] * m2->_matrix[k][j];
				}
        	}
    	}
	}
	// for(int y =0; y < m->ysize; ++y){for(int x =0; x < m->xsize; ++x){m->_matrix[y][x] *= val;}}
}
/// @brief Scale a matrix's values against the mul value.
/// @param m The matrix to be multiplied.
/// @param val The scale factor.
/// @return A multiplied matrix.
matrix_t *matrix_fmul(matrix_t *m, const float val){
	for(size_t y =0; y < m->ysize; ++y){for(size_t x =0; x < m->xsize; ++x){m->_matrix[y][x] *= val;}}
	return m;
}

/// @brief Robustly free a matrix pointer.
/// @param mat The matrix to be freed.
void matrix_free(matrix_t *mat) {
    if (mat == NULL) return;
    for (int i = 0; i < mat->ysize; i++){free(mat->_matrix[i]);}
    free(mat->_matrix);
    free(mat);
}
#pragma endregion

#pragma region Polygon
/// @brief Calculate the centre vec3_t.
/// @param p The polygon whose origin should be calculated.
/// @remark It's essentially just a wrapper for the vec3_avg function.
/// @return The centre vec3_t of a Polygon.
vec3_t polygon_origin(const poly3_t p){return vec3_avg(&(vec3_t[3]){p.a, p.b, p.c}, 3);}

/// @brief Initialise a Polygon.
/// @param a The a vec3_t, not rotated.
/// @param b The b vec3_t, not rotated.
/// @param c The c vec3_t, not rotated.
/// @param rotation The rotation that should be applied to the vec3_ts a, b, c; around rotation.
/// @return A fully initialised polygon.
poly3_t polygon_init(vec3_t a, vec3_t b, vec3_t c, vec3_t rotation){
	vec3_t origin= vec3_avg(&(vec3_t[3]){a, b, c}, 3);
	vec3_t *rot= vec3_rot(&(vec3_t[3]){a, b, c}, &(vec3_t[3]){origin, origin, origin}, 3, rotation);
	poly3_t out= (poly3_t){
		.a= a,
		.b= b,
		.c= c,
		.origin= polygon_origin,
		.rotation= rotation,
	};
	free(rot);
	return out;
}

/// @brief Rotate Polygons around thier Origins.
/// @param p An array of Polygons.
/// @param len The length of the array.
/// @param rot The rotation to be applied.
void poly3_rot(poly3_t *p, size_t len, const vec3_t rot){
	for(size_t cc= 0; cc< len; ++cc){
		vec3_t *_rot= vec3_rot(&(vec3_t[3]){p[cc].a, p[cc].b, p[cc].c}, &(vec3_t[3]){p[cc].origin(p[cc]), p[cc].origin(p[cc]), p[cc].origin(p[cc])}, 3, rot);
		p->a= _rot[0]; p->b= _rot[1]; p->c= _rot[2];
		free(_rot);
	}
}

/// @brief Compute a full Polygon transform.
/// @param p The Polygon array to be transformed.
/// @param len The length of the poly3_t p array.
/// @param transform The movement to be applied.
/// @param rotation The rotation to be applied.
/// @param apex The apex of the polygon's rotation.
void poly3_transform(poly3_t *p, const size_t len, const vec3_t transform, const vec3_t rotation, const vec3_t apex){
	for(size_t cc =0; cc < len; ++cc){p[cc]= poly3_sub(p[cc], apex);}
	poly3_rot(p, len, rotation);
	const vec3_t ret= vec3_add(transform, apex);
	for(size_t cc =0; cc < len; ++cc){p[cc]= poly3_sub(p[cc], ret);}
}

/// @brief Rotate each Polygon in an array around an external point.
/// @param p The Polygon array containing th eelements.
/// @param len The length of the p array.
/// @param apex The pioint to be rotated around.
/// @param rot The roation to be applied.
void poly3_rotaround(poly3_t *p, size_t len, const vec3_t apex, const vec3_t rot){
	for(size_t cc =0; cc < len; ++cc){p[cc]= poly3_sub(p[cc], apex);}
	poly3_rot(p, len, rot);
	for(size_t cc =0; cc < len; ++cc){p[cc]= poly3_sub(p[cc], rot);}
}

/// @brief Apply a movement to a poly3_t.
/// @param p The Polygon to be moved.
/// @param transform The transform to be applied.
/// @return The moved Polygon.
poly3_t poly3_add(poly3_t p, vec3_t transform){
	return (poly3_t){
		.a =vec3_add(p.a, transform),
		.b =vec3_add(p.b, transform),
		.c =vec3_add(p.c, transform),
		.origin =polygon_origin,
		.rotation =p.rotation,
	};
}

/// @brief Subtract a movement to a poly3_t.
/// @param p The Polygon to be moved.
/// @param transform The transform to be applied.
/// @return The moved Polygon.
poly3_t poly3_sub(poly3_t p, vec3_t transform){
	return (poly3_t){
		.a =vec3_sub(p.a, transform),
		.b =vec3_sub(p.b, transform),
		.c =vec3_sub(p.c, transform),
		.origin =polygon_origin,
		.rotation =p.rotation,
	};
}




#pragma endregion