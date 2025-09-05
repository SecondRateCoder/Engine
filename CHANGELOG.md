# [View Read me?](https://github.com/SecondRateCoder/Engine/blob/main/README.md)

# Table of Contents:

# Path overview:
## VER 1, []():
    Basic 3D Vector type added, as well s basic arithmetic allowing 2 vector3s to be added. Usig SIMD.
    The current SIMD usage loads all vec3 Axis into an SIMD register: [x, y, z, 0].
    This is in-eficient, and also wastes a lot of memory.
## VER 1.1, []():
    More Vec3 arithmetic added, using the same SIMD usage.
    Vec3s changed to contain two vec3s, a vec3 for magnitude and a vec3 for magitude,
    This was developed to support Rotational integration as well as allowing for control of a vector on it's local aixs.
    The plain [x, y, z] axis is named a pvec3_t, a vc3 with the axis: [x, y, z] and a direction is named a vec3_t.
    For any arithmetic that wuld change a vec3_ts direction e.g Multiplication or division. 
    The direction is calculated as either a multiplication be the matrix or as an average of the two pvec3 directions.
## VER 1.2, []():
    Matrix implementation created, 2 functions made,
    One to initialise, one to mutiply a vec3 by a matrix and one to free the matrix.
## VER 1.3, []():
    New SIMD structure devised, rather than loading the whole Vec3 into tworegisters then computing,
    the axis of 4 vec3s are loaded at the same time from two vec3 arrays, e.g
    '''
    {
        typedef struct pvec3_t{
            float x, y, z;
        }pvec3_t;
        typedef struct vec3_t{
           float x, y, z;
           pvec3_t direction;
        }vec3_t;

        // PSEUDOCODE
        vec3_t *ARR1 = {...}, *ARR2 = {...}, *out;
        size_t cc_out = 0;
        for(size_T cc =0; cc < (ARR1.LEN < ARR2.LEN? ARR1.LEN: ARR2.LEN); ++cc, ++cc_out){
            SIMD1 = {ARR1[cc].X, (cc+1 < ARR1.LEN? ARR1[cc + 1].X: 0), , (cc+2 < ARR1.LEN? ARR1[cc+2].X: 0), , (cc+3 < ARR1.LEN? ARR1[cc+3].X: 0)};
            SIMD2 = {ARR2[cc].X, (cc+1 < ARR2.LEN? ARR2[cc + 1].X: 0), , (cc+2 < ARR2.LEN? ARR2[cc+2].X: 0), , (cc+3 < ARR2.LEN? ARR2[cc+3].X: 0)};
            SIMD3 = SIMD_ADD(SIMD1, SIMD2);
            out[cc_out] = SIMD_DECODE(SIMD3);
        }
    }
    '''
    This allowed large amounts of vec3s to be processed ore quickly than with conventional arithmetic.
    The average function functioned similarly to the aforementioned code but there was no out buffer, the code fed the output into SIMD1 rather than 3 then outputted SIMD1; DECODED, and divided by cc;
