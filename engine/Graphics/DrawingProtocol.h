#include "GL/gl.h"
#include <Window/Window.h>


#define shaderblock_t ComputeShaderBlock

typedef struct ComputeShaderBlock{
    GLuint shaderProgram, *vertexshader, *fragmentshader, *geometryshader, *tessellation_controlshader, *tessellation_evaluationshader, *computeShader;
}ComputeShaderBlock;