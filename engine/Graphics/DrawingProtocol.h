#include "gl/GL.h"

#define shaderblock_t ComputeShaderBlock

typedef struct ComputeShaderBlock{
    GLUnit *vertexshader, *fragmentshader, *geometryshader, *tessellation_controlshader, *tessellation_evaluationshader, *computeShader;
}ComputeShaderBlock;