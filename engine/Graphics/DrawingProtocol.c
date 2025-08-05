#include <DrawingProtocol.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char *cwd;
#define MAX_PATHLENGTH 1024* 1024
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif
bool cwd_init(){
	if(getcwd(cwd, sizeof(char)* MAX_PATHLENGTH) == NULL){
		printf("getcwd() Error, cwd not Initialised.");
		return false;
	}
	return true;
}

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
const char vs_start[10]= "#define vs", fs_start[10]= "#define fs", shader_end[10]= "#shaderend";


shaderblock_t *shaders;
/// @brief Shader RAWs in case any part of the Program needs them.
char *vertexshader, *fragmentshader, *geometryshader, *tessellation_controlshader, *tessellation_evaluationshader, *computeshader;


/// @brief Initialise a Vertex and Fragment Shader from a File.
/// @param filepath The path to the file containing the shaders.
/// @note The file should be in the current working directory.
/// @remarks The file can be in any Text format, The File Extension doesn't matter as long as it contains the relevant Conventions.
///     !For all the setting Values, All specified chars must be Used.
///		!Does not support Comments, Accepts everything with no Validation or Sanitation
///  Shader Format:
///     Written in conventional OpenGL 3.3 script language.
///     At the top of File are some settings:
///     - Version, INDEX_INCLUSIVE[0 - 5]: The version of the shader language used.
///     - Vertex Shader Index, INDEX_INCLUSIVE[6 - 7] RANGE(0 - 99): The index of the vertex shader in the file that should be used.
///     - Fragment Shader Index, INDEX_INCLUSIVE[8 - 9] RANGE(0 - 99): The index of the fragment shader in the file that should be used.
///     Shader Conventions:
///     - Vertex shaders are defined with the keyword "#define vs" on a line followed by the shader code.
///     - Fragment shaders are defined with the keyword "#define fs" on a line followed by the shader code.
///     - The end of a Shader Block must b marked with a line containing only the keyword "#shaderend".
///     These allow the Shader puller to identify the type shader in the file.
void shaders_pull(char *filepath){
	vertexshader = NULL;
	fragmentshader = NULL;
	geometryshader = NULL;
	tessellation_controlshader = NULL;
	tessellation_evaluationshader = NULL;
	computeshader = NULL;
	//Open the file.
	FILE *shaders = fopen(filepath, "r");
	if(shaders != NULL){
		fseek(shaders, 0, SEEK_END);
		const size_t len =ftell(shaders);
		fseek(shaders, 0, SEEK_SET);
		char settings[10];
		fread(settings, sizeof(char), 10, shaders);
		const uint8_t vsindex =atof((char[3]){settings[6], settings[7], NULL}) < 0? 0: atof((char[3]){settings[6], settings[7], NULL}),
		fsindex =atof((char[3]){settings[8], settings[9], NULL})< 0? 0: atof((char[3]){settings[8], settings[9], NULL});
		long vs_cc =-1, fs_cc =-1;
		bool vs_handled =false, fs_handled =false;
		//Enumerate through the file for either fs_start or vs_start.
		for(size_t cc =0; cc < len; ++cc){
			//Temporary string for looking for #define fs, or #define vs.
			char *temp_10 = malloc(sizeof(char)* 10);
			//If not null, means there is something there.
			if(fgets(temp_10, sizeof(char)* 10, shaders)!=NULL){
				//Is vertex shader block.
				if(strncmp(temp_10, vs_start, 10) == 0){
					vs_cc++;
					// handle vertex shader block
					if(vs_cc == vsindex){
						//Take a snapshot of indexers.
						const size_t curr= ftell(shaders);
						//Set Pointer in case it's not accurately set.
						fseek(shaders, cc* sizeof(char), SEEK_SET);
						//Check for #define end
						char temp[10];
						while(strncmp(temp, shader_end, 10) != 0){
							//Fail on Error.
							if(fgets(temp, 10, shaders) == NULL){return NULL;}
						}
						//#define end found.
						size_t curr_new = ftell(shaders);
						vertexshader = (char *)malloc(sizeof(char)* (curr_new-curr)+1);
						vertexshader[sizeof(char)* (curr_new-curr)]= NULL;
						fread(vertexshader, sizeof(char), curr_new-curr, shaders);
					}
				//Is fragment shader block.
				}else if(strncmp(temp_10, fs_start, 10) == 0){
					fs_cc++;
					// handle fragment shader block
					if(fs_cc == fsindex){
						//Take a snapshot of indexers.
						const size_t curr= ftell(shaders);
						//Set Pointer in case it's not accurately set.
						fseek(shaders, cc* sizeof(char), SEEK_SET);
						//Check for #define end
						char temp[10];
						while(strncmp(temp, shader_end, 10) != 0){
							//Fail on Error.
							if(fgets(temp, 10, shaders) == NULL){return NULL;}
						}
						//#define end found.
						size_t curr_new = ftell(shaders);
						fragmentshader = (char *)malloc(sizeof(char)* (curr_new-curr));
						fragmentshader[sizeof(char)* (curr_new-curr)]= NULL;
						fread(fragmentshader, sizeof(char), curr_new-curr, shaders);
					}
				}
			}
			free(temp_10);
		}
	}
	fclose(shaders);
}

/// @brief Compile all intialised shaders into a singular ComputeShaderBlock struct pointer.
shaderblock_t *shader_compile(){
	shaderblock_t *shaderblock = (shaderblock_t *)malloc(sizeof(shaderblock_t));
	shaderblock->vertexshader = glCreateShader(GL_VERTEX_SHADER);
	if(vertexshader != NULL){
		glShaderSource(shaderblock->vertexshader, 1, (const char * const *)&vertexshader, NULL);
	}else{glShaderSource(shaderblock->vertexshader, 1, (const char * const *)&vertexshader_default, NULL);}
	glCompileShader(shaderblock->vertexshader);
	shaderblock->fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
	if(fragmentshader != NULL){
		glShaderSource(shaderblock->fragmentshader, 1, (const char * const *)&fragmentshader, NULL);
	}else{glShaderSource(shaderblock->fragmentshader, 1, (const char * const *)&fragmentshader_default, NULL);}
	glCompileShader(shaderblock->fragmentshader);
	if(geometryshader != NULL){
		shaderblock->geometryshader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(shaderblock->geometryshader, 1, (const char * const *)&geometryshader, NULL);
		glCompileShader(shaderblock->geometryshader);
	}else{shaderblock->geometryshader = NULL;}
	if(tessellation_controlshader != NULL){
		shaderblock->tessellation_controlshader = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(shaderblock->tessellation_controlshader, 1, (const char * const *)&tessellation_controlshader, NULL);
		glCompileShader(shaderblock->tessellation_controlshader);
	}else{shaderblock->tessellation_controlshader = NULL;}
	if(tessellation_evaluationshader != NULL){
		shaderblock->tessellation_evaluationshader = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(shaderblock->tessellation_evaluationshader, 1, (const char * const *)&tessellation_evaluationshader, NULL);
		glCompileShader(shaderblock->tessellation_evaluationshader);
	}else{shaderblock->tessellation_evaluationshader = NULL;}

	
	return shaderblock;
}

/// @brief Draw a collection of Points on the specified Window, then Push.
/// @param win The Window to be drawn to.
/// @param points The Points to be Interpolated between.
/// @param len The length of the Point buffer.
void win_draw(win_t *win, pointf_t *points, size_t len){
	shaders =shader_compile();
	if(vertexshader == NULL || fragmentshader == NULL){
		printf("Vertex or Fragment Shader not set, resolving.\n");
		char *default_dir = cwd;
		strncat(default_dir, "engine/Graphics/Shaders.txt", 30);
		shaders_pull(default_dir);
	}
	shaders->shaderProgram = glCreateProgam();
	glAttachShader(shaders->shaderProgram, shaders->vertexshader);
	glAttachShader(shaders->shaderProgram, shaders->fragmentshader);
	if(shaders->geometryshader != NULL){glAttachShader(shaders->shaderProgram, shaders->geometryshader);}
	if(shaders->tessellation_controlshader != NULL){glAttachShader(shaders->shaderProgram, shaders->tessellation_controlshader);}
	if(shaders->tessellation_evaluationshader != NULL){glAttachShader(shaders->shaderProgram, shaders->tessellation_evaluationshader);}
	glLinkProgram(shaders->shaderProgram);
}