#ifndef _PUBLIC_H

#define _PUBLIC_H

#include <engine/Libraries/include/KHR/khrplatform.h>
#include <engine/Libraries/include/GLFW/glfw3.h>
#include <engine/Libraries/include/GLFW/glfw3native.h>
#include <engine/Graphics/DrawingProtocol.h>
#include <engine/Graphics/Window/Window.h>
#include <engine/Libraries/include/glad/glad.h>
#include "./engine/_3D.h"
#include <immintrin.h>
#include <xmmintrin.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define IS_NUM(X) ((int)X < (int)'9') && ((int)X > (int)'0')
#define IS_EVEN(X) (X%2 != 0)
#define INT_SIMP(X) (X< 0? -1: 1)

#endif