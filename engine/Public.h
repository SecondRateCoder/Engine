#include "./engine/_3D.h"
#include <immintrin.h>
#include <xmmintrin.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define IS_NUM(X) ((int)X < (int)'9') && ((int)X > (int)'0')
#define IS_EVEN(X) (X%2 != 0)
#define INT_SIMP(X) (X< 0? -1: 1)