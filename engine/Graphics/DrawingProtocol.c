#include <Public.h>


bool cwd_init(){
	if(getcwd(cwd, sizeof(char)* MAX_PATHLENGTH) == NULL){
		printf("getcwd() Error, cwd not Initialised.");
		return false;
	}
	return true;
}




/// @brief Initialise supported Shaders from a File, using Defaults for Required Shaders that are not found.
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
	// tessellation_controlshader = NULL;
	// tessellation_evaluationshader = NULL;
	// computeshader = NULL;
	//Open the file.
	FILE *shaders = fopen(filepath, "r");
	if(shaders != NULL){
		fseek(shaders, 0, SEEK_END);
		const size_t len =ftell(shaders);
		fseek(shaders, 0, SEEK_SET);
		char settings[10];
		fread(settings, sizeof(char), 10, shaders);
		const uint8_t vsindex =atoi((char[3]){settings[6], settings[7], NULL}) < 0? 0: atoi((char[3]){settings[6], settings[7], NULL}),
		fsindex =atoi((char[3]){settings[8], settings[9], NULL})< 0? 0: atoi((char[3]){settings[8], settings[9], NULL});
		long vs_cc =-1, fs_cc =-1;
		bool vs_handled =false, fs_handled =false;
		//Enumerate through the file for either fs_start or vs_start.
		for(size_t cc =0; cc < len; ++cc){
			//Temporary string for looking for #define fs, or #define vs.
			char *temp_10 = malloc((sizeof(char)* 10));
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
							if(fgets(temp, 10, shaders) == NULL){
								free(vertexshader);
								free(fragmentshader);
								free(geometryshader);
								// free(tessellation_controlshader);
								// free(tessellation_evaluationshader);
								// free(computeshader);
								free(temp_10);
								printf("ERROR! #define vs not found in Shader File, Shaders not pulled.");
								fclose(shaders);
								//Return NULL to indicate failure.
								return NULL;
							}
						}
						//#define end found.
						size_t curr_new = ftell(shaders);
						vertexshader = (char *)malloc(sizeof(char)* (curr_new-curr));
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
/// @param delete_shaders_on_link Should the Compiled Shaders be deleted after use?
shaderblock_t *shader_compile(bool delete_shaders_on_link){
	shaderblock_t *shaderblock = (shaderblock_t *)malloc(sizeof(shaderblock_t));
	shaderblock->compiled_ = malloc(sizeof(bool)* 8);
	//Compile Vertex Shader.
	shaderblock->vertexshader = glCreateShader(GL_VERTEX_SHADER);
	if(vertexshader != 0){
		//Use vertexshader.
		glShaderSource(shaderblock->vertexshader, 1, (const char * const *)&vertexshader, NULL);
		//Use vertexshader_default otherwise
	}else{glShaderSource(shaderblock->vertexshader, 1, (const char * const *)&vertexshader_default, NULL);}
	glCompileShader(shaderblock->vertexshader);
	shaderblock->compiled_[1]= true;

	//Compile Fragment Shader.
	shaderblock->fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
	if(fragmentshader != 0){
		//Use fragmentshader.
		glShaderSource(shaderblock->fragmentshader, 1, (const char * const *)&fragmentshader, NULL);
		//Use fragmentshader_default otherwise.
	}else{glShaderSource(shaderblock->fragmentshader, 1, (const char * const *)&fragmentshader_default, NULL);}
	glCompileShader(shaderblock->fragmentshader);
	shaderblock->compiled_[2]= true;

	//Compile Geometry Shader.
	shaderblock->geometryshader = glCreateShader(GL_GEOMETRY_SHADER);
	if(geometryshader != 0){
		//Compile with geometryshader.
		glShaderSource(shaderblock->geometryshader, 1, (const char * const *)&geometryshader, NULL);
		shaderblock->compiled_[3]= true;
		//Use None otherwise.
	}else{
		shaderblock->geometryshader = 0;
		shaderblock->compiled_[3]= false;
	}
	glCompileShader(shaderblock->geometryshader);





	// if(tessellation_controlshader != 0){
	// 	shaderblock->tessellation_controlshader = glCreateShader(GL_TESS_CONTROL_SHADER);
	// 	glShaderSource(shaderblock->tessellation_controlshader, 1, (const char * const *)&tessellation_controlshader, NULL);
	// 	glCompileShader(shaderblock->tessellation_controlshader);
	// }else{shaderblock->tessellation_controlshader = 0;}
	// if(tessellation_evaluationshader != 0){
	// 	shaderblock->tessellation_evaluationshader = glCreateShader(GL_);
	// 	glShaderSource(shaderblock->tessellation_evaluationshader, 1, (const char * const *)&tessellation_evaluationshader, NULL);
	// 	glCompileShader(shaderblock->tessellation_evaluationshader);
	// }else{shaderblock->tessellation_evaluationshader = 0;}

	//Link Program.
	shaderblock->shaderProgram = glCreateProgram();
	glAttachShader(shaderblock->shaderProgram, shaderblock->vertexshader);
	glAttachShader(shaderblock->shaderProgram, shaderblock->fragmentshader);
	if(shaderblock->geometryshader != 0){glAttachShader(shaderblock->shaderProgram, shaderblock->geometryshader);}
	// if(shaderblock->tessellation_controlshader != 0){glAttachShader(shaderblock->shaderProgram, shaderblock->tessellation_controlshader);}
	// if(shaderblock->tessellation_evaluationshader != 0){glAttachShader(shaderblock->shaderProgram, shaderblock->tessellation_evaluationshader);}
	glLinkProgram(shaderblock->shaderProgram);
	shaderblock->compiled_[0]= true;

	//Delete if needed.
	if(delete_shaders_on_link){
		shaderblock->compiled_[1]= false;
		shaderblock->compiled_[2]= false;
		shaderblock->compiled_[3]= false;
		glDeleteShader(shaderblock->vertexshader);
		glDeleteShader(shaderblock->fragmentshader);
		glDeleteShader(shaderblock->geometryshader);
		shaderblock->vertexshader = 0;
		shaderblock->fragmentshader = 0;
		shaderblock->geometryshader = 0;
		// glDeleteShader(shaderblock->tessellation_controlshader);
		// glDeleteShader(shaderblock->tessellation_evaluationshader);
		// shaderblock->tessellation_controlshader = 0;
		// shaderblock->tessellation_evaluationshader = 0;
	}
	shaderblock->compiled_[7]= true;
	return shaderblock;
}

/// @brief Draw a collection of Points on the specified Window, then Push.
/// @param win The Window to be drawn to.
/// @param points The Points to be Interpolated between.
/// @param len The length of the Point buffer.
void win_draw(win_t *win, GLfloat *points, size_t len, GLuint *indexes, size_t ilen){
	//Compile if not usable
	SHADERBLOCK_HANDLE(win->shaders, true);
	if(!win->buffer_[0] && win->buffer_[1]){
		printf("ERROR! VAO set-up before VBO!, Resolving.");
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		return;
	}

	//Set-up VAO, if not already set-up.
	if(!win->buffer_[0]){
		glGenVertexArrays(win->VAO_len, win->VAO);
		glBindVertexArray(win->VAO[win->VAO_curr]);
	}

	//Set-up VBO, if not set-up already.
	if(!win->buffer_[1]){
		glGenBuffers(win->VBO_len, win->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, win->VBO[win->VBO_curr]);
	}
	glBufferData(GL_ARRAY_BUFFER, len * sizeof(GLfloat), points, GL_STATIC_DRAW);

	//Handle EBO.
	bool indexesnull =false;
	if(indexes == NULL || ilen == 0){
		indexesnull = true;
		ilen = (len%2 == 0? len/2: (len%3 == 0? len/3: (ilen == len? len: 0)));
		indexes= malloc(ilen* sizeof(GLuint));
		for(int cc =0; cc < ilen; ++cc){indexes[cc] = cc;}
	}
	glGenBuffers(1, &win->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, win->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ilen* sizeof(GLuint), indexes, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ilen* sizeof(GLuint), points, GL_STATIC_DRAW);
	win_attrblink(win, 0, 3, GL_FLOAT, 6* sizeof(float), (void *)0);
	win_attrblink(win, 0, 3, GL_FLOAT, 6* sizeof(float), (void *)(3* sizeof(float)));
	
	//Set them as Implemented.
	win->buffer_[0]= true;
	win->buffer_[1]= true;
	win->buffer_[2]= true;
	win->vert_count = ilen;
	if(indexesnull){
		free(indexes);
		indexes = NULL;
	}
}

/// @brief Fill the Window's front and back buffer with a specified Color constant.
/// @param win The Window.
/// @param c The Color to be applied.
void win_flood(win_t *win, const argb_t c){
	glViewport(0, 0, win->w, win->h);
	glClearColor(c.r, c.g, c.b, c.a);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(win->window);
}