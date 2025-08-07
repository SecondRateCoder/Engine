#include <Public.h>
#include <DrawingProtocol.h>
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

arrk_t create_arrkey(GLint id, char *name, char *type){return (arrk_t){id,str_normalise(name, true, false),str_normalise(type, true, true)};}
void destroy_arrkey(arrk_t *ak){free(ak->type);free(ak->name);}

void uniform_clean(shaderblock_t *shader){
	for(size_t cc =0; cc < shader->uniform_len; ++cc){
		free(shader->uniforms[cc].type);
		free(shader->uniforms[cc].name);
	}
	free(shader->uniforms);
}

void uniform_write(shaderblock_t *shader, char *type, char *name, char *property, bool *transpose, void *value, size_t num_elements){
	if(name == NULL){return NULL;}
	size_t cc =0;
	for(; cc < shader->uniform_len; ++cc){
		if(strlen(name) == strlen(shader->uniforms[cc]->name)){
			if(strncmp(name, shader->uniforms[cc]->name, strlen(name)) == 0){
				//The Item found.
				switch(str_hash(str_normalise(type, true, true))){
					case "bool":
						if(num_elements > 1){
							glUniform1i(shader->uniforms[cc]->ID, ((GLint*)value)[0]);
						}else{glUniform1i(shader->uniforms[cc]->ID, (GLint*)value);}
						return;
					case "int":
						if(num_elements > 1){
							glUniform1i(shader->uniforms[cc]->ID, ((GLint*)value)[0]);
						}else{glUniform1i(shader->uniforms[cc]->ID, (GLint*)value);}
						return;
					case "unsignedint":
						if(num_elements > 1){
							glUniform1ui(shader->uniforms[cc]->ID, ((GLuint*)value)[0]);
						}else{glUniform1ui(shader->uniforms[cc]->ID, (GLuint*)value);}
						return;
					case "float":
						if(num_elements > 1){
							glUniform1f(shader->uniforms[cc]->ID, ((GLfloat*)value)[0]);
						}else{glUniform1f(shader->uniforms[cc]->ID, (GLfloat*)value);}
						return;
					case "vec2":
						glUniform2fv(shader->uniforms[cc]->ID, num_elements, (GLfloat *)value);
						return;
					case "vec3":
						glUniform3fv(shader->uniforms[cc]->ID, num_elements, (GLfloat *)value);
						return;
					case "vec4":
						glUniform4fv(shader->uniforms[cc]->ID, num_elements, (GLfloat *)value);
						return;
					case "ivec2":
						glUniform2iv(shader->uniforms[cc]->ID, num_elements, (GLint *)value);
						return;
					case "ivec3":
						glUniform3iv(shader->uniforms[cc]->ID, num_elements, (GLint *)value);
						return;
					case "ivec4":
						glUniform4iv(shader->uniforms[cc]->ID, num_elements, (GLint *)value);
						return;
					case "uivec2":
						glUniform2uiv(shader->uniforms[cc]->ID, num_elements, (GLuint *)value);
						return;
					case "uivec3":
						glUniform3uiv(shader->uniforms[cc]->ID, num_elements, (GLuint *)value);
						return;
					case "uivec4":
						glUniform4uiv(shader->uniforms[cc]->ID, num_elements, (GLuint *)value);
						return;
					case "mat2":
						glUniformMatrix2fv(shader->uniforms[cc]->ID, 1, *transpose, (GLfloat *)value);;
						return;
					case "mat3":
						glUniformMatrix3fv(shader->uniforms[cc]->ID, 1, *transpose, (GLfloat *)value);
						return;
					case "mat4":
						glUniformMatrix4fv(shader->uniforms[cc]->ID, 1, *transpose, (GLfloat *)value);
						return;
					case "mat2x3":
						glUniformMatrix2x3fv(shader->uniforms[cc]->ID, 1, *transpose, (GLfloat *)value);
						return;
					case "mat3x2":
						glUniformMatrix3x2fv(shader->uniforms[cc]->ID, 1, *transpose, (GLfloat *)value);
						return;
					case "mat2x4":
						glUniformMatrix2x4fv(shader->uniforms[cc]->ID, 1, *transpose, (GLfloat *)value);
						return;
					case "mat4x2":
						glUniformMatrix4x2fv(shader->uniforms[cc]->ID, 1, *transpose, (GLfloat *)value);
						return;
					case "mat3x4":
						glUniformMatrix3x4fv(shader->uniforms[cc]->ID, 1, *transpose, (GLfloat *)value);
						return;
					case "mat4x3":
						glUniformMatrix4x3fv(shader->uniforms[cc]->ID, 1, *transpose, (GLfloat *)value);
						return;
					default:	return;
				}
			}
		}
	}
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
	shader_error(shaderblock, "VERTEX");

	//Compile Fragment Shader.
	shaderblock->fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
	if(fragmentshader != 0){
		//Use fragmentshader.
		glShaderSource(shaderblock->fragmentshader, 1, (const char * const *)&fragmentshader, NULL);
		//Use fragmentshader_default otherwise.
	}else{glShaderSource(shaderblock->fragmentshader, 1, (const char * const *)&fragmentshader_default, NULL);}
	glCompileShader(shaderblock->fragmentshader);
	shader_error(shaderblock, "FRAGMENT");
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
	shader_error(shaderblock, "GEOMETRY");

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
	shader_error(shaderblock, "PROGRAM");
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

void shader_error(shaderblock_t *sb_t, const char *type){
	const char _program[] = "PROGRAM\0",
	_vertex[] = "VERTEX\0",
	_fragment[] = "FRAGMENT\0",
	_geometry[] = "GEOMETRY\0";
	GLint compiled;
	const GLuint shader = (strncmp(type, _vertex, strlen(_vertex)) == 0? sb_t->vertexshader:
		strncmp(type, _fragment, strlen(_fragment)) == 0? sb_t->fragmentshader:
			strncmp(type, _geometry, strlen(_geometry)) == 0? sb_t->geometryshader:
				strncmp(type, _program, strlen(_program)) == 0? sb_t->shaderProgram:);
	char infoLog[1024];
	if(shader != sb_t->shaderProgram){
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if(compiled == GL_FALSE){
			glGetShaderInfo(shader, 1024, NULL, infoLog);
			infoLog[1023] = '\0';
			fprintf("Shader Compilation error: /s", infoLog);
			switch(shader){
				case sb_t->vertexshader:
					sb_t->_compiled[1] = false;
					return;
				case sb_t->vertexshader:
					sb_t->_compiled[2] = false;
					return;
				case sb_t->vertexshader:
					sb_t->_compiled[3] = false;
					return;
			}
		}else{
			switch(shader){
				case sb_t->vertexshader:
					sb_t->_compiled[1] = true;
					return;
				case sb_t->vertexshader:
					sb_t->_compiled[2] = true;
					return;
				case sb_t->vertexshader:
					sb_t->_compiled[3] = true;
					return;
			}
		}
	}else{
		glGetProgramiv(shader, GL_COMPILE_STATUS, &compiled);
		if(compiled == GL_FALSE){
			glGetProgramInfo(shader, 1024, NULL, infoLog);
			infoLog[1023] = '\0';
			fprintf("Shader Compilation error: /s", infoLog);
			sb_t->_compiled[7] = false;
			sb_t->_compiled[0] = false;
		}else{
			sb_t->_compiled[7] = false;
			sb_t->_compiled[0] = false;
		}
	}
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
	win_attrblink(win, 0, 3, GL_FLOAT, 8* sizeof(float), (void *)0);
	win_attrblink(win, 1, 3, GL_FLOAT, 8* sizeof(float), (void *)(3* sizeof(float)));
	win_attrblink(win, 2, 2, GL_FLOAT, 8* sizeof(float), (void *)(6* sizeof(float)));
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, win->textures[win->textures_curr].width, win->textures[win->textures_curr].height, 0, GL_RGBA, GL_UNSIGNED_BYTE, win->textures[win->textures_curr].img);
	glGenerateMipmap(GL_TEXTURE_2D);
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

void winimage_append(win_t *win, int image_width, int image_height, int colorch_num, char *filepath, argb_t *border_color){
	SHADERBLOCK_HANDLE(win->shaders, true);
	if(win->textures_len == 0){
		win->textures = malloc(sizeof(image_t)* 1);
		win->textures_len++;
	}
	stbi_set_flip_vertically_on_load(true);
	win->textures[win->textures_len-1].img = stbi_load(filepath, &image_width, &image_height, &colorch_num, 0);;
	glGenTexturesEXT(win->textures_len, win->textures);
	switch(win->textures_len-1){
		case 0:
			glActiveTexture(GL_TEXTURE0);
			break;
		case 1:
			glActiveTexture(GL_TEXTURE1);
			break;
		case 2:
			glActiveTexture(GL_TEXTURE2);
			break;
		case 3:
			glActiveTexture(GL_TEXTURE3);
			break;
		case 4:
			glActiveTexture(GL_TEXTURE4);
			break;
	}
	glBindTextures(GL_TEXTURE_2D, win->textures[win->textures_len-1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (float[4]){border_color->a, border_color->r, border_color->g, border_color->b});
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
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