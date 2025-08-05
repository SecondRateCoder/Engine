#include <Public.h>
#include <string.h>


bool cwd_init(){
	if(getcwd(cwd, sizeof(char)* MAX_PATHLENGTH) == NULL){
		printf("getcwd() Error, cwd not Initialised.");
		return false;
	}
	return true;
}

/// @brief Read or Write to a Shader's settings.
/// @param filepath The path to the Shader's file.
/// @param write A 10 char array to write to the Settings.
/// @note this Function has no validation.
/// @return The Setting read from the file.
char *shadersettings_rw(char *filepath, char *write){
	FILE *shaders = fopen(filepath, "r");
	char settings[10];
	if(shaders != NULL){
		fread(settings, sizeof(char), 10, shaders);
		if(write != NULL && strlen(write) == 10){
			fseek(shaders, 0, SEEK_SET);
			fwrite(write, sizeof(char), 10, shaders);
		}
		fclose(shaders);
	}
	return settings;
}


/// @brief Initialise supported Shaders from a File, using Defaults for Required Shaders that are not found.
/// @param filepath The path to the file containing the shaders.
/// @note The file should be in the current working directory.
/// @remarks The file can be in any Text format, The File Extension doesn't matter as long as it contains the relevant Conventions.
///     !For all the setting Values, All specified chars must be Used.
///		!Supports having multiple Shaders in different Files, only requirement is that Conventions are met.
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
	// vertexshader = NULL;
	// fragmentshader = NULL;
	// geometryshader = NULL;
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

/// @brief Parse and initialise a shaderblock's uniform variable list.
/// @param shader 
void uniform_init(shaderblock_t *shader){
	size_t *indexes_inshader, num_uniforms, len, len_oftypenames;
	unsigned int *size_inshader;
	char *shader_txt;
	if(shader_typenames == NULL){
		shader_typenames = (char *[36]){
			[0] = "bool",[1] = "int",[2] = "float",
			[3] = "vec2",[4] = "vec3",[5] = "vec4",
			[6] = "ivec2",[7] = "ivec3",[8] = "ivec4",
			[9] = "uvec2",[10] = "uvec3",[11] = "uvec4",
			[12] = "bvec2",[13] = "bvec3",[14] = "bvec4",
			[15] = "mat2",[16] = "mat3",[17] = "mat4",
			[18] = "mat2x3",[19] = "mat3x2",[20] = "mat2x4",
			[21] = "mat4x2",[22] = "mat3x4",[23] = "mat4x3",
			[24] = "sampler1D",[25] = "sampler2D",[26] = "sampler3D",
			[27] = "samplerCube",[28] = "sampler1DShadow",[29] = "sampler2DShadow",
			[30] = "sampler2DArray",[31] = "sampler2DArrayShadow",
			[32] = "isampler1D",[33] = "isampler2D",[34] = "usampler1D",[35] = "usampler2D",
		};
	}
	const char _uniform[7]= "uniform\0", _struct[8] = "struct\0";
	uint8_t _3 =0;
	/*
		PARSE SHADERS, REPEAT TWICE TO FULLY HANDLE AND TYPES.
	*/
	while(_3 < 6){
		switch(_3){
			case 0| 1:
				shader_txt =vertexshader == NULL? vertexshader_default: vertexshader;
				len =strlen(shader_txt);
				break;
			case 2| 3:
				shader_txt =fragmentshader == NULL? fragmentshader_default: fragmentshader;
				len =strlen(shader_txt);
				break;
			case 4| 5:
				if(geometryshader == NULL){return;}
				shader_txt = geometryshader;
				len =strlen(shader_txt);
				break;
			default: return;
		}
		#pragma region Parsing.
		for(size_t cc =0; cc < len; ++cc){
			//If the next 6 values make "uniform", cache index and length of decleration.
			char _temp[6]= (char[6]){shader_txt[cc],  (cc < len-1? shader_txt[cc+1]: '\0'), 
				(cc < len-2? shader_txt[cc+2]: '\0'), (cc < len-3? shader_txt[cc+3]: '\0'), 
				(cc < len-4? shader_txt[cc+4]: '\0'), (cc < len-5? shader_txt[cc+5]: '\0')};
			if(strncmp(_temp, _uniform, 6) == 0){
				//Is a uniform definition
				if(num_uniforms == 0){
					indexes_inshader = malloc(sizeof(size_t));
					size_inshader = malloc(sizeof(size_t));
				}else{
					indexes_inshader = realloc(indexes_inshader, ((num_uniforms+1)* sizeof(size_t)));
					size_inshader = realloc(indexes_inshader, ((num_uniforms+1)* sizeof(size_t)));
				}
				indexes_inshader[num_uniforms]= cc+6;
				//Get Length of uniform variable declaration.
				unsigned int len =6;
				while(shader_txt[cc+len] != ';' || shader_txt[cc+len] != '\0'){++len;}
				if(shader_txt[cc+len] != '\0'){
					break;
					indexes_inshader = realloc(indexes_inshader, ((num_uniforms-1)* sizeof(size_t)));
					size_inshader = realloc(indexes_inshader, ((num_uniforms-1)* sizeof(size_t)));
				}
				size_inshader[num_uniforms]= len;
				++num_uniforms;
			}else if(strncmp(_temp, _struct, 6) == 0){
				//The declaration is a struct type, store name in typename array.

				//The Index in the vertex_thingie that the thingie lies.
				const size_t index= cc+ 6;
				//Get Length of uniform variable.
				unsigned int len =0;
				while(shader_txt[index+len] != ';' || shader_txt[index+len] != '\0'){++len;}
				//Handle Invalids.
				if(shader_txt[index+len] != '\0'){break;}
				//Add new typename.
				shader_typenames = realloc(shader_typenames, sizeof(char*)* (len_oftypenames+1));
				memcpy(&shader_typenames[len_oftypenames-1], shader_txt[len], len);
				len_oftypenames++;
			}
		}

		/*
			HANDLE UNIFORMS.
			UNIFORM keyword is clipped out, the rest of the string is the type then the name of the uniform.
		*/
		for(size_t cc =0; cc < num_uniforms; ++cc){
			char *uniform_text = malloc(sizeof(char)* (size_inshader[cc]+ 1)), *name_pure = '\0';
			uniform_text[size_inshader[cc]]= '\0';
			memcpy(uniform_text, &shader_txt[indexes_inshader[cc]], size_inshader[cc]);
			//Get the type of the uniform.
			unsigned int type = 0, start =0;
			bool type_handled = false;
			for(unsigned int i =0; i < size_inshader; ++i){
				if(uniform_text[i] == ' '/* || uniform_text[i] == '\0'*/ && type_handled == false){
					//End of type found.
					char *type_pure = '\0';
					type_pure = malloc(sizeof(char)* (i+1));
					memcpy(type_pure, uniform_text, i);
					type_pure[i]= '\0';
					start = i;
					type_handled = true;
					continue;
				}
				if(type_handled){
					//Handle name.
					name_pure = malloc(sizeof(char)* (i+1));
					while(uniform_text[i] != ';' || uniform_text[i] != '\0'){i++;}
					memcpy(name_pure, &uniform_text[start], i);
					name_pure[i]= '\0';
					break;
				}
				/*
					TODO:
						Store all the Uniform IDs in the shaderrblock_t
				*/
			}
		}

		_3++;
	}
	return;
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