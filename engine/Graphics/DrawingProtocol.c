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

const char vs_start[10]= "#define vs", fs_start[10]= "#define fs", shader_end[10]= "#shaderend";

char *vertexshader;
char *fragmentshader;

/*
Shader format:
    Written in conventional SAhadr script language.
    At the top of File are some settings:
    - Version: The version of the shader language used.
    - Vertex Shader Index (0 - 99): The index of the vertex shader in the file that should be used.
    - Fragment Shader Index (0 - 99): The index of the fragment shader in the file that should be used.

    Shader Conventions:
    - Vertex shaders are defined with the keyword "#define vs" on a line followed by the shader code.
    - Fragment shaders are defined with the keyword "#define fs" on a line followed by the shader code.
    - The end of a Shader Block must b marked with a line containing only the keyword "#shaderend".
    These allow the Shader puller to identify the type shader in the file.
*/

void shader_pull(char *filepath){
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
						vertexshader = (char *)malloc(sizeof(char)* (curr_new-curr));
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
						fread(fragmentshader, sizeof(char), curr_new-curr, shaders);
                    }
                }
            }
            free(temp_10);
        }
    }
    fclose(shaders);
}