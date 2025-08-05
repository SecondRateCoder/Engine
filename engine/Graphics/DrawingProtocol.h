#include "glad.h"
#include <Window/Window.h>

//Header Guard.
#ifndef _DP_H
#define _DP_H



#define shaderblock_t ComputeShaderBlock
#define SHADERBLOCK_HANDLE(SB, CLEAN) if(!SB->compiled_[7]){   \
    if(vertexshader == NULL || fragmentshader == NULL){ \
			printf("Vertex or Fragment Shader not set, resolving.\n");  \
			char *default_dir = cwd;    \
			strncat(default_dir, "engine/Graphics/Shaders.txt", 30);    \
			shaders_pull(default_dir);  \
		}   \
		SB =shader_compile(CLEAN); \
}

typedef struct ComputeShaderBlock{
    //Length 8.
    /*
    [0]: Is Shader Program compiled?
    [1]: Is Vertex Shader compiled?
    [2]: Is Fragment Shader compiled?
    [3]: Is Geometry Shader compiled?
    ! DEPRECATED, [4]: Is Tessellation Control Shader compiled? 
    ! DEPRECATED, [5]: Is Tesselation Evaluation Shader compiled?
    ! DEPRECATED, [6]: Is Compute Shader compiled?
    [7]: Is ShaderBlock usable?
    */
    bool *compiled_;
    GLuint shaderProgram, 
        vertexshader, 
        fragmentshader, 
        geometryshader, 
        // *tessellation_controlshader, 
        // *tessellation_evaluationshader, 
        // *computeShader;
}ComputeShaderBlock;

char *cwd;
#define MAX_PATHLENGTH 1024* 1024
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

//Length: 53
const char *vertexshader_default = 
"#version 330 core\n"
"#define vs\n"
"layout(location = 0) in vec3 position;\n"
"void main(){\n"
"    gl_Position = vec4(position, 1.0);\n"
"}\n"
"#shaderend\n\0";

//Length: 54
const char *fragmentshader_default = 
"#version 330 core\n"
"#define fs\n"
"out vec4 color;\n"
"void main(){\n"
"    color = vec4(1.0, 0.0, 0.0, 1.0);\n"
"}\n\0"
"#shaderend\n";

/// @brief The Constant Shader keywords for Understanding Shader text.
const char vs_start[10]= "#define vs", 
	fs_start[10]= "#define fs", 
	gs_start[10]= "#define gs", 
	// tcs_start[10]= "#define tc", 
	// tes_start[10]= "#define te", 
	// cs_start[10]= "#define cs", 
	shader_end[10]= "#shaderend";

/// @brief Shader RAWs in case any part of the Program needs them.
char *vertexshader, 
	*fragmentshader, 
	*geometryshader
	// *tessellation_controlshader, 
	// *tessellation_evaluationshader, 
	// *computeshader
	;


#endif