#include "./engine/_3D.h"
#include <immittrin.h>
#include <xmmittrin.h>
#include <stdbool.h>
#include <stdmath.h>

#define IS_NUM(X) ((int)X < (int)'9') && ((int)X > (int)'0')
#define IS_EVEN(X) X%2 != 0