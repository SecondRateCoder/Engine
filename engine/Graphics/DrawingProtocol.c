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

// void uniform_init(shaderblock_t *sb_t){
// 	char *temp_txt = NULL, **temp_typenames = NULL;
// 	const char _struct[7] ="struct\0", _uniform[7] ="uniform\0";
// 	size_t temptxt_len, typenames_len;
// 	if(shader_typenames == NULL){
// 		shader_typenames = (char *[36]){
// 			[0] = "bool",[1] = "int",[2] = "float",
// 			[3] = "vec2",[4] = "vec3",[5] = "vec4",
// 			[6] = "ivec2",[7] = "ivec3",[8] = "ivec4",
// 			[9] = "uvec2",[10] = "uvec3",[11] = "uvec4",
// 			[12] = "bvec2",[13] = "bvec3",[14] = "bvec4",
// 			[15] = "mat2",[16] = "mat3",[17] = "mat4",
// 			[18] = "mat2x3",[19] = "mat3x2",[20] = "mat2x4",
// 			[21] = "mat4x2",[22] = "mat3x4",[23] = "mat4x3",
// 			[24] = "sampler1D",[25] = "sampler2D",[26] = "sampler3D",
// 			[27] = "samplerCube",[28] = "sampler1DShadow",[29] = "sampler2DShadow",
// 			[30] = "sampler2DArray",[31] = "sampler2DArrayShadow",
// 			[32] = "isampler1D",[33] = "isampler2D",[34] = "usampler1D",[35] = "usampler2D",
// 		};
// 	}
// 	//Control the swapping between vertexshader, fragmentshader and geometryshader.
// 	uint8_t _3 =0;
// 	//Control the double-looping of each shader.
// 	bool vertex_loop2, fragment_loop2, geometry_loop2;
// 	while(_3 < 6){
// 		switch(_3){
// 			case 0:
// 				temp_txt =vertexshader == NULL? vertexshader_default: vertexshader;
// 				temptxt_len =strlen(temp_txt);
// 				break;
// 			case 1:
// 				if(vertex_loop2){
// 					temp_txt =vertexshader == NULL? vertexshader_default: vertexshader;
// 					temptxt_len =strlen(temp_txt);
// 					break;
// 				}else{continue;}
// 			case 2:
// 				temp_txt =fragmentshader == NULL? fragmentshader_default: fragmentshader;
// 				temptxt_len =strlen(temp_txt);
// 				break;
// 			case 3:
// 				if(fragment_loop2){
// 					temp_txt =fragmentshader == NULL? fragmentshader_default: fragmentshader;
// 					temptxt_len =strlen(temp_txt);
// 					break;
// 				}else{continue;}
// 			case 4:
// 				if(geometryshader == NULL){return;}
// 				temp_txt = geometryshader;
// 				temptxt_len =strlen(temp_txt);
// 				break;
// 			case 5:
// 				if(geometry_loop2 && geometryshader != NULL){
// 					temp_txt =geometryshader;
// 					temptxt_len =strlen(temp_txt);
// 					break;
// 				}else{return;}
// 			default: return;
// 		}
// 		//Loop for line by line logic, ending with '{', ';'or '\0'.
// 		for(size_t cc =0; cc < temptxt_len;){
// 			char *temp_line;
// 			size_t line_len =cc;
// 			while(temp_txt[line_len+cc] != '{' && temp_txt[line_len+cc] != ';' && temp_txt[line_len+cc] != '\0'){++line_len;}
// 			temp_line = malloc(sizeof(char)* (line_len+1));
// 			memcpy(temp_line, &temp_txt[cc], line_len);
// 			temp_line[line_len] = '\0';
// 			for(size_t cc_char =0; cc_char < line_len; ++cc_char){
// 				//Store the next 6 char values t compare against _uniform and _struct.
// 				// const char next_7[7] = (char[7]){
// 				// 	temp_line[cc_char], (cc_char < line_len-1? temp_line[cc_char+1]: '\0'), 
// 				// 	(cc_char < line_len-1? temp_line[cc_char+1]: '\0'), (cc_char < line_len-1? temp_line[cc_char+1]: '\0'), 
// 				// 	(cc_char < line_len-1? temp_line[cc_char+1]: '\0'), (cc_char < line_len-1? temp_line[cc_char+1]: '\0'), 
// 				// 	(cc_char < line_len-1? temp_line[cc_char+1]: '\0'), (cc_char < line_len-1? temp_line[cc_char+1]: '\0')};
// 				if(strncmp(%temp_line[cc_char], _struct, 6) == 0){
// 					//Is type declaration, compare for same typename. store typename if not found.

// 					//Clip out typename.
// 					cc_char+=6;
// 					char *typename = NULL;
// 					size_t typename_len =0;
// 					while(temp_line[cc_char+typename_len] != ' ' || temp_line[cc_char+typename_len] != '\0'){++typename_len;}
// 					typename = malloc(sizeof(char)* (typename_len));
// 					memcpy(typename, &temp_line[cc_char], typename_len);
// 					typename[typename_len-1] = '\0';

// 					//Check for duplicate, ignore if Duplicate.
// 					bool is_duplicate =false;
// 					for(size_t _dupecc =0; _dupecc < (typenames_len > 36? typenames_len: 36); ++_dupecc){
// 						if(_dupecc < typenames_len){
// 							if(typename_len == strlen(temp_typenames[_dupecc])){
// 								if(strncmp(typename, temp_typenames[_dupecc], typename_len) == 0){
// 									is_duplicate = true;
// 									free(typename);
// 									break;
// 								}
// 							}
// 						}
// 						if(_dupecc < 36){
// 							if(typename_len == strlen(shader_typenames[_dupecc])){
// 								if(strncmp(typename, shader_typenames[_dupecc], typename_len) == 0){
// 									is_duplicate = true;
// 									free(typename);
// 									break;
// 								}
// 							}
// 						}
// 					}
// 					if(!is_duplicate){
// 						if(typenames_len == 0){temp_typenames = malloc(sizeof(char*));
// 						}else{temp_typenames = realloc(temp_typenames, sizeof(char*)* (typenames_len+1));}
// 						typenames_len++;
// 						temp_typenames = typename;
// 					}
// 					//Increment cc to skip the rest of the struct's decleration.
// 					while((temp_txt[cc] != '}' && temp_txt[cc] != '\0') && (cc < temptxt_len-1?temp_txt[cc+1] != ';': true)){++cc;}
// 				}else if(strncmp(&temp_line[cc], _uniform, 6) == 0){
// 					//Is uniform variable decleration.
// 					char *type_pure, name_pure;
// 					cc+=6;
// 					unsigned int len_cc;
// 					bool type_handled, name_handled;
// 					//Get Pure name and Pure type.
// 					while(temp_line[cc] == ' '){++cc;}
// 					while(temp_line[cc+len_cc] != ' '){++len_cc;}
// 					type_pure = malloc(sizeof(char)* (len_cc+1));
// 					memcpy(type_pure, &temp_line[cc], len_cc);
// 					type_pure[len_cc] = '\0';
// 					++cc;
// 					len_cc =0;
// 					while(temp_line[cc] == ' '){++cc;}
// 					while(temp_line[cc+len_cc] != ' ' && temp_line[cc+len_cc] != ';' && temp_line[cc+len_cc] != '='){++len_cc;}
// 					name_pure = malloc(sizeof(char)* (len_cc));
// 					memcpy(name_pure, &temp_line[cc], len_cc);
// 					name_pure[len_cc] = '\0';
// 					break;
// 					//Add new Uniform item.
// 					//Check for duplicates.
// 					bool is_duplicate, leave;
// 					for(size_t u_cc =0; u_cc < shaders->uniform_len && leave && is_duplicate; ++u_cc){
// 						if(strlen(shaders->uniforms[u_cc]->name) == len_cc){
// 							if(strncmp(shaders->uniforms[u_cc]->name, name_pure) == 0){
// 								is_duplicate = true;
// 								free(type_pure);
// 								free(name_pure);
// 							}
// 						}
// 						if(strlen(shaders->uniforms[u_cc]->type) == strlen(type_pure)){
// 							if(strncmp(shaders->uniforms[u_cc]->type, type_pure) == 0){
// 								switch(_3){
// 									case 0:vertex_loop2 = true;break;
// 									case 1:fragment_loop2 = true;break;
// 									case 2:geometry_loop2 = true;break;
// 								}
// 								free(type_pure);
// 								free(name_pure);
// 								leave =true;
// 							}
// 						}
// 					}
// 					if(!is_duplicate){
// 						shaders->uniform_len++;
// 						shaders->uniforms = realloc(shaders->uniforms, sizeof(arrk_t)* (uniform_len));
// 						GLint _ID =glGetUniformLocation(shaders->shaderProgram, name_pure);
// 						if(_ID == GL_INVALID_VALUE || _ID == GL_INVALID_OPERATION || _ID == GL_INVALID_OPERATION){
// 							SHADERBLOCK_HANDLE(shaders, true);
// 							_ID =glGetUniformLocation(shaders->shaderProgram, name_pure);
// 							if(_ID == GL_INVALID_VALUE || _ID == GL_INVALID_OPERATION || _ID == GL_INVALID_OPERATION){
// 								printf("Error in compiling Uniforms.");
// 							}
// 						}
// 						shaders->uniforms[shaders->uniform_len-1] = (arrk_t){
// 							ID =(GLuint)_ID,
// 							name =name_pure,
// 							type =type_pure,
// 						};
// 					}else{
// 						free(type_pure);
// 						free(name_pure);
// 					}
// 				}
// 			}
// 			// free(temp_line);g
// 			cc+=line_len;
// 		}
// 	}
// 	free(shader_typenames);
// }
/// @brief Parse and initialise a shaderblock's uniform variable list.
/// @param shader 
void uniform_init(shaderblock_t *sb_t){
	char *temp_txt = NULL;
	char **temp_typenames = NULL; // This will hold dynamically found struct names
	const char _struct[] = "struct";
	const char _uniform[] = "uniform";
	size_t temptxt_len = 0;
	size_t typenames_len = 0;

	// Use a static array for built-in types, no need to check against NULL
	
	// A list of shader sources to iterate through
	const char* shader_sources[] = {vertexshader,fragmentshader,(geometryshader == NULL? '\0': geometryshader)};
	const size_t num_shaders = sizeof(shader_sources) / sizeof(shader_sources[0]);

	for(size_t i = 0; i < num_shaders; ++i){
		temp_txt = (char*)shader_sources[i];
		if (temp_txt == NULL){continue;}
		temptxt_len = strlen(temp_txt);

		// This loop parses the shader source code
		for(size_t cc = 0; cc < temptxt_len; ){
			// Find the start of a potential declaration
			while(cc < temptxt_len && isspace(temp_txt[cc])){cc++;}
			if(cc >= temptxt_len){break;}

			// Check for 'uniform' or 'struct' keywords
			if(cc + 6 < temptxt_len && strncmp(&temp_txt[cc], _struct, 6) == 0){
				// A 'struct' declaration is found
				cc += 6;
				while(cc < temptxt_len && isspace(temp_txt[cc])){cc++;}

				size_t typename_start = cc;
				while(cc < temptxt_len && !isspace(temp_txt[cc]) && temp_txt[cc] != '{'){cc++;}
				size_t typename_len = cc - typename_start;
				char* typename = strndup(&temp_txt[typename_start], typename_len);

				if(typename){
					bool is_duplicate = false;
					for(size_t _dupecc = 0; _dupecc < typenames_len; ++_dupecc){
						if(strcmp(typename, temp_typenames[_dupecc]) == 0){
							is_duplicate = true;
							break;
						}
					}
					
					if(!is_duplicate){
						// Dynamically grow the list of custom typenames
						char** temp_arr = realloc(temp_typenames, sizeof(char*) * (typenames_len + 1));
						if (temp_arr) {
							temp_typenames = temp_arr;
							temp_typenames[typenames_len] = typename;
							typenames_len++;
						}else{free(typename);}
					}else{free(typename);}
					
					// Skip the rest of the struct definition
					int brace_count = 1;
					while (cc < temptxt_len) {
						if (temp_txt[cc] == '{') brace_count++;
						else if (temp_txt[cc] == '}') brace_count--;
						if (brace_count == 0) break;
						cc++;
					}
				}
				if (cc < temptxt_len){cc++;}
			} else if (cc + 7 < temptxt_len && strncmp(&temp_txt[cc], _uniform, 7) == 0) {
				// A 'uniform' declaration is found
				cc += 7;
				while(cc < temptxt_len && isspace(temp_txt[cc])){cc++;}

				// Extract type name
				size_t type_start = cc;
				while(cc < temptxt_len && !isspace(temp_txt[cc]) && temp_txt[cc] != ';'){cc++;}
				size_t type_len = cc - type_start;
				char* type_pure = strndup(&temp_txt[type_start], type_len);
				
				while(cc < temptxt_len && isspace(temp_txt[cc])){cc++;}
				
				// Extract variable name
				size_t name_start = cc;
				while(cc < temptxt_len && !isspace(temp_txt[cc]) && temp_txt[cc] != ';' && temp_txt[cc] != '='){cc++;}
				size_t name_len = cc - name_start;
				char* name_pure = strndup(&temp_txt[name_start], name_len);

				if (type_pure && name_pure) {
					bool is_duplicate = false;
					for (size_t u_cc = 0; u_cc < sb_t->uniform_len; ++u_cc) {
						if (strcmp(sb_t->uniforms[u_cc]->name, name_pure) == 0) {
							is_duplicate = true;
							break;
						}
					}

					if (!is_duplicate) {
						GLint _ID = glGetUniformLocation(sb_t->shaderProgram, name_pure);
						
						if (_ID != -1) { // glGetUniformLocation returns -1 on failure
							arrk_t new_uniform = create_arrkey(_ID, name_pure, type_pure);
							if (new_uniform) {
								arrk_t* temp_arr = realloc(sb_t->uniforms, sizeof(arrk_t) * (sb_t->uniform_len + 1));
								if (temp_arr) {
									sb_t->uniforms = temp_arr;
									sb_t->uniforms[sb_t->uniform_len++] = new_uniform;
								} else {
									destroy_arrkey(new_uniform);
									fprintf(stderr, "Error: Failed to reallocate memory for uniforms.\n");
								}
							}
						} else {
							fprintf(stderr, "Error: Failed to get uniform location for '%s'.\n", name_pure);
						}
					}
				}
				
				free(type_pure);
				free(name_pure);
			}else{
				// If no keyword is found, skip to the end of the line
				while(cc < temptxt_len && temp_txt[cc] != '\n' && temp_txt[cc] != ';'){cc++;}
				if (cc < temptxt_len){cc++;}
			}
		}
	}

	// Clean up the temporary list of struct names
	for(size_t i = 0; i < typenames_len; ++i){free(temp_typenames[i]);}
	free(temp_typenames);
}
arrk_t create_arrkey(GLint id, char *name, char *type){return (arrk_t){id,str_normalise(name, true, false),str_normalise(type, true, true)};}
void destroy_arrkey(arrk_t ak){free(ak->type);free(ak->name);}

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
				switch(str_normalise(type, true, true)){
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
	_fragment[] = "FRAGMENT\0"
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
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ARGB, win->textures[win->textures_curr].width, win->textures[win->textures_curr].height, 0, GL_ARGB, GL_UNSIGNED_BYTE, win->textures[win->textures_curr].img);
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
	if(img_pure == NULL){return;}
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