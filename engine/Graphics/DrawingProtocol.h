#include "GL/gl.h"


#define shaderblock_t ComputeShaderBlock

typedef struct ComputeShaderBlock{
    GLuint *vertexshader, *fragmentshader, *geometryshader, *tessellation_controlshader, *tessellation_evaluationshader, *computeShader;
}ComputeShaderBlock;