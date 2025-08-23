#include "../Public.h"
#include <string.h>

#define GL_GLEXT_PROTOTYPES

// Function prototypes for missing functions, assuming they exist elsewhere
// extern const uint128_t builtin_shader_typehash[];
// extern const char* vertexshader_default;
// extern const char* fragmentshader_default;
// extern const char vs_start[10];
// extern const char* fs_start;
// extern const char* shader_end;
// extern char cwd[MAX_PATHLENGTH];
// extern char* vertexshader;
// extern char* fragmentshader;
// extern char* geometryshader;

// Global variables should be declared if used globally
// These are assumed to be defined in `DrawingProtocol.h` or elsewhere
// GLuint vertexshader, fragmentshader, geometryshader;
// char* vertexshader_code;
// char* fragmentshader_code;
// char* geometryshader_code;

// Corrected shader_error function.
// The original had incorrect `printf` format specifiers and was not properly handling the return value of `strncmp`.
// It also had inconsistent naming for the `_compiled` member. I've removed the state tracking as it's not robust.
void shader_error(shaderblock_t* sb_t, const char* type) {
	if (sb_t == NULL || type == NULL) return;

	GLuint shader = 0;
	GLint success;
	char infoLog[1024];

	if (strcmp(type, "PROGRAM") == 0) {
		shader = sb_t->shaderProgram;
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			fprintf(stderr, "Shader PROGRAM linking error: %s\n", infoLog);
		}
	} else {
		if(strcmp(type, "VERTEX") == 0){shader = sb_t->vertexshader;
		}else if(strcmp(type, "FRAGMENT") == 0){shader = sb_t->fragmentshader;
		}else if(strcmp(type, "GEOMETRY") == 0){shader = sb_t->geometryshader;
		}else{
			fprintf(stderr, "Error: Unknown shader type '%s'.\n", type);
			return;
		}

		if (shader == 0) {return;}

		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			fprintf(stderr, "Shader %s compilation error: %s\n", type, infoLog);
		}
	}
}

// Fixed function to read or write shader settings.
// This function needs to handle reading and writing separately to avoid conflicts.
// It also needs to correctly handle file pointers.
// The original function tried to do both, but `fread` and `fwrite` on the same file pointer
// without proper positioning can lead to unexpected behavior.
char* shadersettings_rw(const char* filepath, char* write){
	char* settings = NULL;
	FILE* shaders = fopen(filepath, "r+"); // Open for reading and writing

	if(shaders != NULL){
		settings = (char*)malloc((settings_len+1) * sizeof(char)); // Allocate space for 15 chars + null terminator
		if(settings == NULL){
			fprintf(stderr, "Error: Memory allocation failed in shadersettings_rw.\n");
			fclose(shaders);
			return NULL;
		}

		if(fread(settings, sizeof(char), settings_len, shaders) != settings_len){
			fprintf(stderr, "Error: Could not read 10 characters from file.\n");
			free(settings);
			settings = NULL;
		}else{settings[settings_len] = '\0';}

		if(write != NULL){
			fseek(shaders, 0, SEEK_SET);
			if (fwrite(write, sizeof(char), settings_len, shaders) != settings_len) {
				fprintf(stderr, "Error: Could not write 10 characters to file.\n");
			}
		}
		fclose(shaders);
	}else{fprintf(stderr, "Error: Failed to open file at '%s'.\n", filepath);}
	return settings;
}

void shaders_pull(const char *filepath){shader_pull(filepath, (bool[3]){true, true, true});}

uint32_t parse_shader_index(const char *settings, size_t offset) {
    char temp[3] = { settings[offset], settings[offset + 1], '\0' };
    int raw = strtol(temp, NULL, 10);
    return raw < 0 ? 0 : (uint32_t)raw;
}


/// @brief Pull shaders, populating vertexshader, fragmentshader and geometryshader with the relevant and found shaders.
/// @param filepath The filepath to a shader or multiple shaders
/// @param redo_shaders Choose between the shaders to be reset to NULL.
/// [0] -> vertexshader, [1] -> fragmentshader, [2] ->geometryshader.
void shader_pull(const char *filepath, const bool redo_shaders[3]){
	if(redo_shaders[0] == true){
		free(vertexshader);
    	vertexshader = NULL;
	}
	if(redo_shaders[1] == true){
		free(fragmentshader);
    	fragmentshader = NULL;
	}
	if(redo_shaders[2] == true){
		free(geometryshader);
    	geometryshader = NULL;
	}
	char *settings = shadersettings_rw(filepath, NULL);
	settings[settings_len] = '\0';
	FILE *text = fopen(filepath, "rb");
	if (text == NULL) {
		fprintf(stderr, "Error: Failed to open shader file at '%s'.\n", filepath);
		return;
	}else{fseek(text, settings_len, SEEK_SET);}
	const uint32_t vsindex = parse_shader_index(settings, 6);
    const uint32_t fsindex = parse_shader_index(settings, 8);
	const uint32_t gsindex = parse_shader_index(settings, 10);
	const uint32_t tesindex = parse_shader_index(settings, 12);
    const uint32_t tcsindex = parse_shader_index(settings, 14);


    int vs_cc = -1, fs_cc = -1, gs_cc = -1, tes_cc = -1, tcs_cc = -1;
	"3.300000000000";
    char line_buffer[256];
    size_t start_pos, end_pos;
	while(fgets(line_buffer, sizeof(line_buffer), text)){
		for(size_t cc = 0; cc < sizeof(line_buffer); ++cc){
			if(line_buffer[cc] == '\n'){break;}
			if(line_buffer[cc] == '#'){
				if(strncmp(&line_buffer[cc], vs_start, sizeof(vs_start)) == 0 || strncmp(&line_buffer[cc], vs_start, sizeof(fs_start)) == 0 || strncmp(&line_buffer[cc], vs_start, sizeof(gs_start)) == 0){
					start_pos = ftell(text);
					cc+=sizeof(vs_start);
					size_t cc_ =cc+2;
					while(line_buffer[cc_+1] != '#' && cc_ < 255){if(line_buffer[cc_] == '\n'){break;}else{++cc_;}}
					if(cc_ < 255 && line_buffer[cc_] == '#'){if(strncmp(&line_buffer[cc_], shader_end, sizeof(shader_end)) == 0){end_pos = ftell(text)+cc_;}}
					else{fgets(line_buffer, sizeof(line_buffer), text);}
					while(fgets(line_buffer, sizeof(line_buffer), text)){
						cc_ =0;
						while(line_buffer[cc_+1] != '#' && cc_ < 255){if(line_buffer[cc_] == '\n'){break;}else{++cc_;}}
						// if(cc_ == 255){continue;}
						if(strncmp(&line_buffer[cc_], shader_end, sizeof(shader_end)) == 0){end_pos = ftell(text)+cc_;}
						memset(line_buffer, 0, 256);
					}
					if(end_pos > start_pos){
						fseek(text, start_pos, SEEK_SET);
						if(strncmp(&line_buffer[cc], vs_start, sizeof(vs_start)) == 0){
							//VERTEX.
							vs_cc++;
							if(vs_cc == vsindex){fread(vertexshader, sizeof(char), end_pos -start_pos, text);}
						}else if(strncmp(&line_buffer[cc], fs_start, sizeof(fs_start)) == 0){
							//FRAGMENT.
							fs_cc++;
							if(fs_cc == fsindex){fread(fragmentshader, sizeof(char), end_pos -start_pos, text);}
						}else if(strncmp(&line_buffer[cc], gs_start, sizeof(gs_start)) == 0){
							//GEOMETRY.
							gs_cc++;
							if(gs_cc == gsindex){fread(geometryshader, sizeof(char), end_pos -start_pos, text);}
						}else if(strncmp(&line_buffer[cc], tes_start, sizeof(tes_start)) == 0){
							//TESSELATION EVAL.
							tes_cc++;
							if(tes_cc == tesindex){fread(tessellation_evaluationshader, sizeof(char), end_pos -start_pos, text);}
						}else if(strncmp(&line_buffer[cc], tcs_start, sizeof(tcs_start)) == 0){
							//TESSELLATION CONTROL
							tcs_cc++;
							if(tcs_cc == tcsindex){fread(tessellation_controlshader, sizeof(char), end_pos -start_pos, text);}
						}
					}
				}
			}
		}
		memset(line_buffer, 0, 256);
	}
    if (vertexshader == NULL){fprintf(stderr, "Warning: Vertex shader with index %d not found, using default: \"%s\".\n", vsindex, vertexshader_default);}
    if (fragmentshader == NULL){fprintf(stderr, "Warning: Fragment shader with index %d not found, using default: \"%s\".\n", fsindex, fragmentshader_default);}
}

// Corrected uniform_init function.
// The original was missing crucial cleanup and had logical errors in memory management.
// The `strndup` calls and the `realloc` logic were a particular problem.
void uniform_init(shaderblock_t *sb){
	if(sb == NULL){return;}

	const char *shaders[] = {
		vertexshader ? vertexshader : vertexshader_default,
		fragmentshader ? fragmentshader : fragmentshader_default,
		geometryshader ? geometryshader : NULL,
		tessellation_controlshader ? tessellation_controlshader : NULL,
		tessellation_evaluationshader ? tessellation_evaluationshader : NULL
	};
	char** temp_typenames = NULL;
	size_t typenames_len = 0;
	
	if(sb->uniforms != NULL || sb->uniform_len > 0){
		for (size_t i = 0; i < sb->uniform_len; ++i) {destroy_arrkey(&sb->uniforms[i]);}
		free(sb->uniforms);
		sb->uniforms = NULL;
		sb->uniform_len = 0;
	}
	// size_t sb_uniform_size = 0;
	// for(size_t cc =0; cc < sb->uniform_len; ++cc){sb_uniform_size += strlen(sb->uniforms[sb->uniform_len].name) + strlen(sb->uniforms[sb->uniform_len].type) + sizeof(sb->uniforms[sb->uniform_len].Location);}
	for(size_t cc =0; cc < 5; ++cc){
		if(shaders[cc] == NULL){continue;}
		const size_t shader_len = strlen(shaders[cc]);
		for(size_t char_cc =0; char_cc < shader_len; ++char_cc){
			while(char_cc < shader_len && isspace(shaders[cc][char_cc])){++char_cc;}
			if(shaders[cc][char_cc] == 'u'){
				//Might be uniform.
				if(strncmp(&shaders[cc][char_cc], "uniform", 8) == 0){
					char_cc+= 8;
					while(char_cc < shader_len && isspace(shaders[cc][char_cc])){++char_cc;}
					unsigned int len_char_cc =0;
					//Get type definition.
					while(len_char_cc + char_cc < shader_len && isalnum(shaders[cc][char_cc+len_char_cc])){++len_char_cc;}
					sb->uniforms = realloc(sb->uniforms, sizeof(arrk_t)* (sb->uniform_len+1));
					sb->uniforms[sb->uniform_len].type = malloc(sizeof(char)* (len_char_cc));
					memcpy(sb->uniforms[sb->uniform_len].type, &shaders[cc][char_cc], len_char_cc);
					sb->uniforms[sb->uniform_len].type[len_char_cc] = '\0';
					char_cc += len_char_cc;
					len_char_cc = 0;
					//Get name definition.
					while(char_cc < shader_len && isspace(shaders[cc][char_cc])){char_cc++;}
					while(len_char_cc + char_cc < shader_len && isalnum(shaders[cc][char_cc+len_char_cc])){++len_char_cc;}
					sb->uniforms[sb->uniform_len].name = malloc(sizeof(char)* (len_char_cc));
					memcpy(sb->uniforms[sb->uniform_len].name, &shaders[cc][char_cc], len_char_cc);
					sb->uniforms[sb->uniform_len].name[len_char_cc] = '\0';
					if((sb->uniforms[sb->uniform_len].Location = glGetUniformLocation(sb->shaderProgram, sb->uniforms[sb->uniform_len].name)) == -1){
						printf("Uh-oh!, I couldn't access the uniform %s of type %s and at location %d", sb->uniforms[sb->uniform_len].name, sb->uniforms[sb->uniform_len].type, char_cc);
					}
					sb->uniform_len++;
				}
			}// else if(shaders[cc][char_cc] == 's'){continue;}
		}
	}
}

// void uniform_init(shaderblock_t* sb_t) {
// 	for (size_t i = 0; i < num_shaders; ++i) {
// 		const char* temp_txt = shader_sources[i];
// 		if (temp_txt == NULL) continue;
// 		size_t temptxt_len = strlen(temp_txt);

// 		// This loop parses the shader source code
// 		for (size_t cc = 0; cc < temptxt_len; ) {
// 			// Find the start of a potential declaration
// 			while (cc < temptxt_len && isspace(temp_txt[cc])) {
// 				cc++;
// 			}
// 			if (cc >= temptxt_len) break;

// 			// Check for 'uniform' or 'struct' keywords
// 			if (cc + 6 <= temptxt_len && strncmp(&temp_txt[cc], "struct", 6) == 0) {
// 				cc += 6;
// 				while (cc < temptxt_len && isspace(temp_txt[cc])) { cc++; }

// 				size_t typename_start = cc;
// 				while (cc < temptxt_len && !isspace(temp_txt[cc]) && temp_txt[cc] != '{' && temp_txt[cc] != ';') {
// 					cc++;
// 				}
// 				size_t typename_len = cc - typename_start;
// 				char* typename = strdup(&temp_txt[typename_start]);

// 				if (typename) {
// 					bool is_duplicate = false;
// 					for (size_t _dupecc = 0; _dupecc < typenames_len; ++_dupecc) {
// 						if (strcmp(typename, temp_typenames[_dupecc]) == 0) {
// 							is_duplicate = true;
// 							break;
// 						}
// 					}
					
// 					if (!is_duplicate) {
// 						char** temp_arr = (char**)realloc(temp_typenames, sizeof(char*) * (typenames_len + 1));
// 						if (temp_arr) {
// 							temp_typenames = temp_arr;
// 							temp_typenames[typenames_len] = typename;
// 							typenames_len++;
// 						} else {
// 							fprintf(stderr, "Error: Failed to reallocate memory for struct names.\n");
// 							free(typename);
// 						}
// 					} else {
// 						free(typename);
// 					}
// 				}
				
// 				// Skip the rest of the struct definition
// 				int brace_count = 0;
// 				while (cc < temptxt_len) {
// 					if (temp_txt[cc] == '{') brace_count++;
// 					else if (temp_txt[cc] == '}') brace_count--;
// 					if (brace_count == 0) break;
// 					cc++;
// 				}
// 				if (cc < temptxt_len) cc++;
// 			} else if (cc + 7 <= temptxt_len && strncmp(&temp_txt[cc], "uniform", 7) == 0) {
// 				cc += 7;
// 				while (cc < temptxt_len && isspace(temp_txt[cc])) { cc++; }

// 				size_t type_start = cc;
// 				while (cc < temptxt_len && !isspace(temp_txt[cc]) && temp_txt[cc] != ';' && temp_txt[cc] != '=') {
// 					cc++;
// 				}
// 				size_t type_len = cc - type_start;
// 				char* type_pure = strdup(&temp_txt[type_start]);
				
// 				while (cc < temptxt_len && isspace(temp_txt[cc])) { cc++; }
				
// 				size_t name_start = cc;
// 				while (cc < temptxt_len && !isspace(temp_txt[cc]) && temp_txt[cc] != ';' && temp_txt[cc] != '=') {
// 					cc++;
// 				}
// 				size_t name_len = cc - name_start;
// 				char* name_pure = strdup(&temp_txt[name_start]);

// 				if (type_pure && name_pure) {
// 					bool is_duplicate = false;
// 					for (size_t u_cc = 0; u_cc < sb_t->uniform_len; ++u_cc) {
// 						if (sb_t->uniforms[u_cc].name && strcmp(sb_t->uniforms[u_cc].name, name_pure) == 0) {
// 							is_duplicate = true;
// 							break;
// 						}
// 					}

// 					if (!is_duplicate) {
// 						GLint _ID = glGetUniformLocation(sb_t->shaderProgram, name_pure);
						
// 						if (_ID != -1) {
// 							arrk_t new_uniform = (arrk_t){_ID, name_pure, type_pure};
// 							arrk_t* temp_arr = (arrk_t*)realloc(sb_t->uniforms, sizeof(arrk_t) * (sb_t->uniform_len + 1));
// 							if (temp_arr) {
// 								sb_t->uniforms = temp_arr;
// 								sb_t->uniforms[sb_t->uniform_len++] = new_uniform;
// 							} else {
// 								destroy_arrkey(&new_uniform);
// 								fprintf(stderr, "Error: Failed to reallocate memory for uniforms.\n");
// 							}
// 						} else {
// 							fprintf(stderr, "Error: Failed to get uniform location for '%s'.\n", name_pure);
// 						}
// 					}
// 				}
// 				free(type_pure);
// 				free(name_pure);
// 			} else {
// 				while (cc < temptxt_len && temp_txt[cc] != '\n' && temp_txt[cc] != ';') { cc++; }
// 				if (cc < temptxt_len) cc++;
// 			}
// 		}
// 	}

// 	for (size_t i = 0; i < typenames_len; ++i) {
// 		free(temp_typenames[i]);
// 	}
// 	free(temp_typenames);
// }

void destroy_arrkey(arrk_t *arrk){
	if(arrk->name != NULL){free(arrk->name);}
	if(arrk->type != NULL){free(arrk->type);}
}

// Corrected uniform_clean function.
// The original was accessing the `uniforms` array incorrectly.
void uniform_free(shaderblock_t* shader){
	if (shader == NULL || shader->uniforms == NULL) return;
	for (size_t cc = 0; cc < shader->uniform_len; ++cc) {
		free(shader->uniforms[cc].type);
		free(shader->uniforms[cc].name);
	}
	free(shader->uniforms);
	shader->uniforms = NULL;
	shader->uniform_len = 0;
}

// Corrected uniform_write function.
// The original had a `return NULL` in a `void` function and incorrect pointer dereferencing.
bool uniform_write(shaderblock_t* shader, const char* type, const char* name, const char* property, bool transpose, const void* value, size_t num_elements){
	if (shader == NULL || name == NULL) return false;
	size_t *typehash_ = str_hash(str_normalise(type, true, true));
	const uint128_t typehash = {typehash[1], typehash[0]};
	GLint location = -1;
	// Find the uniform location
	for (size_t cc = 0; cc < shader->uniform_len; ++cc) {
		if (strcmp(name, shader->uniforms[cc].name) == 0) {
			location = shader->uniforms[cc].Location;
			break;
		}
	}

	if (location == -1) {
		fprintf(stderr, "Warning: Uniform '%s' not found.\n", name);
		return false;
	}
	for(size_t cc =0; cc < 37; ++cc){
		if(uint128_t_comp(builtin_shader_typehash[cc], typehash) == true){
			// Use a switch statement for dispatching GL uniform functions
			// Note: The original switch had incorrect pointer casts and logic for single vs. multiple elements.
			switch (cc) {
				case 0: // GL_INT
				case 1: // GL_BOOL
					glUniform1iv(location, num_elements, (const GLint*)value);
					break;
				case 2: // GL_UNSIGNED_INT
					glUniform1uiv(location, num_elements, (const GLuint*)value);
					break;
				case 3: // GL_FLOAT
					glUniform1fv(location, num_elements, (const GLfloat*)value);
					break;
				case 4: // vec2
					glUniform2fv(location, num_elements, (const GLfloat*)value);
					break;
				case 5: // vec3
					glUniform3fv(location, num_elements, (const GLfloat*)value);
					break;
				case 6: // vec4
					glUniform4fv(location, num_elements, (const GLfloat*)value);
					break;
				case 7: // ivec2
					glUniform2iv(location, num_elements, (const GLint*)value);
					break;
				case 8: // ivec3
					glUniform3iv(location, num_elements, (const GLint*)value);
					break;
				case 9: // ivec4
					glUniform4iv(location, num_elements, (const GLint*)value);
					break;
				case 10: // uvec2
					glUniform2uiv(location, num_elements, (const GLuint*)value);
					break;
				case 11: // uvec3
					glUniform3uiv(location, num_elements, (const GLuint*)value);
					break;
				case 12: // uvec4
					glUniform4uiv(location, num_elements, (const GLuint*)value);
					break;
				case 13: // mat2
					glUniformMatrix2fv(location, num_elements, transpose, (const GLfloat*)value);
					break;
				case 14: // mat3
					glUniformMatrix3fv(location, num_elements, transpose, (const GLfloat*)value);
					break;
				case 15: // mat4
					glUniformMatrix4fv(location, num_elements, transpose, (const GLfloat*)value);
					break;
				case 16: // mat2x3
					glUniformMatrix2x3fv(location, num_elements, transpose, (const GLfloat*)value);
					break;
				case 17: // mat3x2
					glUniformMatrix3x2fv(location, num_elements, transpose, (const GLfloat*)value);
					break;
				case 18: // mat2x4
					glUniformMatrix2x4fv(location, num_elements, transpose, (const GLfloat*)value);
					break;
				case 19: // mat4x2
					glUniformMatrix4x2fv(location, num_elements, transpose, (const GLfloat*)value);
					break;
				case 20: // mat3x4
					glUniformMatrix3x4fv(location, num_elements, transpose, (const GLfloat*)value);
					break;
				case 21: // mat4x3
					glUniformMatrix4x3fv(location, num_elements, transpose, (const GLfloat*)value);
					break;
				default:
					//Just attempt writing something ngl.
					// fprintf(stderr, "Error: Unsupported shader type or uniform '%s' not found.\n", type);
					const size_t name_len  =strlen(name), type_len = strlen(type), pointerchar_len = 4, dotchar_len = 3;
					char *full_access = malloc(strlen(name)+strlen(property)+strlen("->\0")+1);
					full_access[strlen(name)+strlen(property)+strlen("->\0")] = '\0';
					strncat(full_access, name, strlen(name));
					strncat(full_access+name_len, "->", 3);
					strncat(full_access+name_len+pointerchar_len, property, strlen(property));
					GLint out = glGetUniformLocation(shader->shaderProgram, full_access);
					if(out == -1){
						//Try with just a "."
						free(full_access);
						full_access = realloc(full_access, strlen(name)+strlen(property)+strlen(".\0")+1);
						full_access[strlen(name)+strlen(property)+strlen(".")] = '\0';
						// strncat(full_access, name, strlen(name));
						strncat(full_access+name_len, ".", 2);
						strncat(full_access+name_len+dotchar_len, property, strlen(property));
						out = glGetUniformLocation(shader->shaderProgram, full_access);
						if(out == -1){return false;}
					}
					glUniform1f(out, ((const GLfloat*)value)[0]);
					free(full_access);
					return false;
			}
		}
	}

	return true;
}


// Corrected shader_compile function.
// The original had a few logical errors, incorrect checks, and was missing error handling for `malloc`.
shaderblock_t* shader_compile(bool delete_shaders_on_link) {
	shaderblock_t* shaderblock = (shaderblock_t*)malloc(sizeof(shaderblock_t));
	if (shaderblock == NULL) {
		fprintf(stderr, "Error: Failed to allocate memory for shaderblock.\n");
		return NULL;
	}
	memset(shaderblock, 0, sizeof(shaderblock_t));

	// Compile Vertex Shader.
	shaderblock->vertexshader = glCreateShader(GL_VERTEX_SHADER);
	const char* vs_source = (vertexshader != NULL) ? vertexshader : vertexshader_default;
	glShaderSource(shaderblock->vertexshader, 1, &vs_source, NULL);
	glCompileShader(shaderblock->vertexshader);
	shader_error(shaderblock, "VERTEX");

	// Compile Fragment Shader.
	shaderblock->fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fs_source = (fragmentshader != NULL) ? fragmentshader : fragmentshader_default;
	glShaderSource(shaderblock->fragmentshader, 1, &fs_source, NULL);
	glCompileShader(shaderblock->fragmentshader);
	shader_error(shaderblock, "FRAGMENT");

	// Compile Geometry Shader (optional).
	if (geometryshader != NULL) {
		shaderblock->geometryshader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(shaderblock->geometryshader, 1, (const char* const*)&geometryshader, NULL);
		glCompileShader(shaderblock->geometryshader);
		shader_error(shaderblock, "GEOMETRY");
	} else {
		shaderblock->geometryshader = 0;
	}
	
	// Link Program.
	shaderblock->shaderProgram = glCreateProgram();
	glAttachShader(shaderblock->shaderProgram, shaderblock->vertexshader);
	glAttachShader(shaderblock->shaderProgram, shaderblock->fragmentshader);
	if (shaderblock->geometryshader != 0) {
		glAttachShader(shaderblock->shaderProgram, shaderblock->geometryshader);
	}
	glLinkProgram(shaderblock->shaderProgram);
	shader_error(shaderblock, "PROGRAM");

	// Delete if needed.
	if (delete_shaders_on_link) {
		glDeleteShader(shaderblock->vertexshader);
		glDeleteShader(shaderblock->fragmentshader);
		if (shaderblock->geometryshader != 0) {
			glDeleteShader(shaderblock->geometryshader);
		}
	}
	
	// The original code was using `shaderblock->compiled_` and `shaderblock->_compiled`.
	// It's likely a typo. I've assumed one of them is the correct member.
	// I am not fixing this, but instead removing the `_compiled` checks, as they are not 
	// standard practice and the `shader_error` function already reports issues.
	
	return shaderblock;
}



/// @brief Add a _mesh to be drawn in the Window.
/// @param win The Window the _mesh will be drawn in.
/// @param _mesh The _mesh to be drawn.
void win_draw(win_t *win, mesh_t *mesh){
	if (win == NULL || mesh == NULL){return;}
	bufferobj_t *temp_ptr = &win->buffers[win->buffer_curr];
	SHADERBLOCK_HANDLE(win->shaders, true, true);
	BUFFEROBJECT_HANDLE(temp_ptr, mesh->mesh_data, mesh->data_len, mesh->vertex_index, mesh->index_len, GL_STATIC_DRAW, 10);
	mesh_attrlink(win->buffers, win->layout_offset, win->layout_offset+1, win->layout_offset+2, mesh);
	glDrawElements(GL_TRIANGLES, mesh->data_len, GL_UNSIGNED_INT, mesh->mesh_data);
}

// Corrected winimage_append function.
// The original code used `glGenTexturesEXT`, which is not standard, and had other
// incorrect function calls and logic for texture management.
// void winimage_append(win_t* win, const char* filepath, const argb_t* border_color){
// 	if (win == NULL || filepath == NULL) return;

// 	// Use a simpler approach to add a new texture
// 	win->textures = (image_t*)realloc(win->textures, sizeof(image_t) * (win->textures_len + 1));
// 	if (win->textures == NULL) {
// 		fprintf(stderr, "Error: Failed to reallocate memory for textures.\n");
// 		return;
// 	}
	
// 	image_t* new_image = &win->textures[win->textures_len];

// 	// Load the image with STB
// 	stbi_set_flip_vertically_on_load(true);
// 	new_image->img = stbi_load(filepath, &new_image->width, &new_image->height, &new_image->color_channels, 0);
// 	if (new_image->img == NULL) {
// 		fprintf(stderr, "Error: Failed to load image at '%s'.\n", filepath);
// 		return;
// 	}

// 	// Generate and bind the new texture
	
	
// 	win->textures_len++;
// }

// Fixed win_flood function.
// The original had no issues, but some minor formatting improvements were made for clarity.
void win_flood(win_t* win, const argb_t c) {
	if (win == NULL || win->g_window == NULL) return;
	
	glViewport(0, 0, win->w, win->h);
	glClearColor(c.r, c.g, c.b, c.a);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(win->g_window);
}

/// @brief Do a function from BUFFER_OPTIONS on EBO or VBO of a Buffer Object.
/// @param buffer 
/// @param option 
/// @return 
void *buffer_bufferdo(bufferobj_t* buffer, const size_t len, const BUFFER_OPTIONS option){
	switch(option){
		//For single buffer objects.
		case BUFFER_OPTIONS_CLEAR:
			glDeleteVertexArrays(1, &buffer->VAO);
			buffer_bufferdo(buffer, 1, BUFFER_OPTIONS_CLEAR_VBO);
			buffer_bufferdo(buffer, 1, BUFFER_OPTIONS_CLEAR_EBO);
			return NULL;
		case BUFFER_OPTIONS_CLEAR_VBO:
			glDeleteBuffers(1, buffer->VBO);
			buffer->VBO_len = 0;
			free(buffer->VBO);
			return NULL;
		case BUFFER_OPTIONS_CLEAR_EBO:
			glDeleteBuffers(1, buffer->EBO);
			buffer->EBO_len = 0;
			free(buffer->EBO);
			return NULL;
		case BUFFER_OPTIONS_FREE:
			// Free the buffer object itself.
			buffer_bufferdo(buffer, 1, BUFFER_OPTIONS_CLEAR);
			free(buffer->VBO);
			free(buffer->EBO);
			return NULL;
		
		//For multiple buffer objects.
		case BUFFER_OPTIONS_FREE_ALL:
			for(size_t cc =0; cc < len; ++cc){buffer_bufferdo(&buffer[cc], 1, BUFFER_OPTIONS_FREE);}
			return NULL;

		case BUFFER_OPTIONS_CLEAR_VAO:
			for(size_t cc =0; cc < len; ++cc){glDeleteVertexArrays(1, &buffer[cc].VAO);}
			return NULL;
		
		case BUFFER_OPTIONS_COLLECT_VAO:
			GLuint *out = (GLuint*)malloc(sizeof(GLuint) * len);
			if (out == NULL) {
				fprintf(stderr, "Error: Memory allocation failed in BUFFER_OPTIONS_COLLECT_VAO.\n");
				return NULL;
			}else{
				for(size_t cc =0; cc < len; ++cc){out[cc] = buffer[cc].VAO;}
				return out;
			}
		case BUFFER_OPTIONS_COLLECT_VBO_MULTIPLE:
			GLuint *out_vbo = (GLuint*)malloc(sizeof(GLuint) * len);
			if (out_vbo == NULL) {
				fprintf(stderr, "Error: Memory allocation failed in BUFFER_OPTIONS_COLLECT_VBO_MULTIPLE.\n");
				return NULL;
			}else{
				for(size_t cc =0; cc < len; ++cc){out_vbo[cc] = buffer[cc].VBO[0];}
				return out_vbo;
			}
		case BUFFER_OPTIONS_COLLECT_EBO_MULTIPLE:
			GLuint *out_ebo = (GLuint*)malloc(sizeof(GLuint) * len);
			if (out_ebo == NULL) {
				fprintf(stderr, "Error: Memory allocation failed in BUFFER_OPTIONS_COLLECT_EBO_MULTIPLE.\n");
				return NULL;
			}else{
				for(size_t cc =0; cc < len; ++cc){out_ebo[cc] = buffer[cc].EBO[0];}
				return out_ebo;
			}
		default:
			fprintf(stderr, "Error: Unknown BUFFER_OPTIONS value %zu.\n", option);
			return NULL;
		
	}
}

bufferobj_t *win_buffercurr(win_t *win){
	if(win->buffer_curr >= win->buffer_len){win->buffer_curr = win->buffer_len - 1;}
	return &win->buffers[win->buffer_curr];
}

void shaderblock_handle(shaderblock_t *shader, bool clean, bool do_uniforms){
	if(vertexshader == NULL || fragmentshader == NULL){
		printf("Vertex or Fragment Shader not set, resolving.\n");
		char *default_dir = strdup(cwd);
		// default_dir = realloc(default_dir, (cwd_len)* sizeof(char));
		strncat(default_dir, "\\Resources\\Shaders\\Shaders.txt", 30);
		shaders_pull(default_dir);
	}
	if(do_uniforms){uniform_init(shader);}
	if(shader->compiled_[7] == false){shader =shader_compile(clean);}
}
void bufferobject_handle(bufferobj_t *buffer, GLfloat *vertices, size_t v_len, GLuint *index_order, size_t index_len, GLenum draw_format, uint8_t max_tries){
	bool success = 0;
	uint8_t counter = 0;
	do { \
		if(buffer == NULL){break;}
		success = true;
		/*VAO not set-up*/ 
		if (buffer->buffer_[0] == false) { 
		if (buffer->VAO == 0){glGenVertexArrays(1, &buffer->VAO);}	
		glBindVertexArray(buffer->VAO); 
		} 
		/*VBO not set-up*/ 
		if(buffer->buffer_[1] == false && vertices != NULL){	
				if(buffer->VBO_len == 0){	
					glGenBuffers(1, buffer->VBO); 
					buffer->VBO_len = 1; 
				} 
				glBindBuffer(GL_ARRAY_BUFFER, *buffer->VBO); 
				glBufferData(GL_ARRAY_BUFFER, v_len * sizeof(GLfloat), vertices, draw_format); 
		} 
		/*EBO not set-up*/ 
		if(buffer->buffer_[2] == false && index_order != NULL){	
			GLuint* actual_indexes = NULL;  
			bool free_indexes_new = false;	
			if((index_len) < v_len / 3){index_len = 0;}else{index_len = v_len / 3;}   
				/*Handle when INDEX_ORDER is NULL, using an array of {1, 2, 3, 4... (v_len-1)}*/
				if (index_order == NULL || (index_len) == 0) {	
					index_len = v_len / 3;   
					actual_indexes = (GLuint*)malloc((index_len) * sizeof(GLuint)); 
					if (actual_indexes){for (size_t cc = 0; cc < v_len; ++cc) { actual_indexes[cc] = cc; }}   
					free_indexes_new = true;	
				}else{actual_indexes = index_order;}   
				/*Finish handling EBO*/	
				if (buffer->EBO == 0) {glGenBuffers(1, buffer->EBO);}   
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *buffer->EBO);    
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, (index_len) * sizeof(GLuint), actual_indexes, draw_format);   
				if(free_indexes_new){free(actual_indexes);}	
		} 
	}while (counter < max_tries || success == false);
}