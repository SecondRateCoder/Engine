#include "../Public.h"

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

/*
Shader setting conventions:
	[0 - 4]: OpenGL Version,
	[5]: Core or Compatibility?
	[6 - 10]: Vertex Shader index
	[11 - 15]: Fragment Shader index
	[16 - 20]: Geometry Shader index
*/


/// @brief Check for compilation errors in a shader's state.
/// @param sb_t The shaderblock_t to be checked for errors.
/// @param type The type to be checked, Can be: "PROGRAM", "VERTEX", "FRAGMENT", "GEOMETRY".
/// @return true on no fail, false on whether type is invalid or shader compilation error.
bool shader_error(shaderblock_t* sb_t, const char* type) {
	if(sb_t == NULL || type == NULL){return false;}

	GLuint shader = 0;
	GLint success;
	char infoLog[1024];

	if (strncmp(type, "PROGRAM", 7) == 0) {
		shader = sb_t->shaderProgram;
		GLCall(GLUseProgram(shader));
		GLCall(glGetProgramiv(shader, GL_LINK_STATUS, &success));
		if (!success) {
			GLCall(glGetProgramInfoLog(shader, 1024, NULL, infoLog));
			fprintf(stderr, ANSI_RED("Shader PROGRAM linking error: %s\n"), infoLog);
			return false;
		}
	} else {
		if(strncmp(type, "VERTEX", 6) == 0){shader = sb_t->vertexshader;
		}else if(strcmp(type, "FRAGMENT") == 0){shader = sb_t->fragmentshader;
		}else if(strcmp(type, "GEOMETRY") == 0){shader = sb_t->geometryshader;
		}else{
			fprintf(stderr, ANSI_YELLOW("Error: Unknown shader type '%s'.\n"), type);
			return false;
		}

		if(shader == 0){
			if(glIsShader(shader) == GL_FALSE){
				printf(ANSI_RED("SHADER FAIL!!!"));
			}
			return false;
		}
		GLCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
		if(success){
			printf(ANSI_GREEN("Linking success"));
			return true;
		}else{
			GLCall(glGetShaderInfoLog(shader, 1024, NULL, infoLog));
			fprintf(stderr, ANSI_RED("Shader %s compilation error: %s\n"), type, infoLog);
			return false;
		}
	}
	printf(ANSI_GREEN("Compilation success"));
	return true;
}

/// @brief Read/Write to a setings snippet of a formatted Shader file.
/// @param filepath The file pah to the shader file.
/// @param write The snippet to write.
/// @return The snippet read from the file.
char* shadersettings_rw(const char* filepath, char* write){
	char* settings = NULL;
	FILE* shaders = fopen(filepath, "r+"); // Open for reading and writing

	if(shaders != NULL){
		settings = (char*)calloc((settings_len), sizeof(char)); // Allocate space for 15 chars + null terminator
		if(settings == NULL){
			fprintf(stderr, ANSI_RED("Error: Memory allocation failed in shadersettings_rw.\n"));
			fclose(shaders);
			return NULL;
		}

		if(fread(settings, sizeof(char), settings_len, shaders) != settings_len){
			fprintf(stderr, ANSI_RED("Error: Could not read %d characters from file.\n"), settings_len);
			free(settings);
			settings = NULL;
		}else{settings[settings_len] = '\0';}

		if(write != NULL){
			fseek(shaders, 0, SEEK_SET);
			if (fwrite(write, sizeof(char), settings_len, shaders) != settings_len) {
				fprintf(stderr, ANSI_RED("Error: Could not write 10 characters to file.\n"));
			}
		}
		fclose(shaders);
	}else{fprintf(stderr, ANSI_RED("Error: Failed to open file at '%s'.\n"), filepath);}
	return settings;
}

/// @brief Pull some shaders into the global shader (char *) types.
/// @param filepath The file path to the file containing the shaders.
void shaders_pull(const char *filepath){shader_pull(filepath, (bool[3]){true, true, false});}

/// @brief Parse a 4 char into an integer.
/// @param settings The settings snippet.
/// @param offset The offset in the settings snippet.
/// @param len The length of the snippet.
/// @return The output.
uint32_t parse_shader_index(const char *settings, uint8_t offset, const uint8_t len){
	char *temp = calloc((len + 1), sizeof(settings[0]));
	uint8_t cc_ = 0;
	for(uint8_t cc =offset; cc < (len < strlen(settings)? len: strlen(settings)); ++cc, ++cc_){
		temp[cc_] = settings[cc];
	}
	temp[len] = '\0';
    int raw = strtol(temp, NULL, len);
	free(temp);
    return raw < 0 ? 0 : (uint32_t)raw;
}

/// @brief Decode a settings snippet into a @ref dsettings_t type
/// @param settings The settings snippet.
/// @return The parsed @ref dsetting_t type.
dsetting_t settings_decode(char settings[settings_len]){
	dsetting_t out = {
		.vsindex = parse_shader_index(settings, 6, SHADER_INDEX_LEN),
		.fsindex = parse_shader_index(settings, 11, SHADER_INDEX_LEN),
		.gsindex = parse_shader_index(settings, 16, SHADER_INDEX_LEN),
		.version = NULL
	};
	out.version = calloc(12, sizeof(settings[0]));
	memcpy(out.version, "#version ", 9* sizeof(settings[0]));
	memcpy(out.version + 9, settings, 3* sizeof(settings[0]));
	if(settings[5] == '0'){
		out.version = realloc(out.version, strlen(out.version) + 6);
		memcpy(out.version + 12, " core\n", 6* sizeof(settings[0]));
		out.version[18] = '\0';
	}else{
		out.version = realloc(out.version, strlen(out.version) + 15);
		memcpy(out.version + 12, " compatibility\n", 15* sizeof(settings[0]));
		out.version[27] = '\0';
	}
	out.version_len = strlen(out.version);
	return out;
}


/// @brief Pull shaders, populating vertexshader, fragmentshader and geometryshader with the relevant and found shaders.
/// @param filepath The filepath to a shader or multiple shaders
/// @param redo_shaders Choose between the shaders to be reset to NULL.
/// [0] -> vertexshader, [1] -> fragmentshader, [2] ->geometryshader.
void shader_pull(const char *filepath, bool redo_shaders[3]){
	if(redo_shaders[0] == true && vertexshader != NULL){
		free(vertexshader);
    	vertexshader = NULL;
	}
	if(redo_shaders[1] == true && fragmentshader != NULL){
		free(fragmentshader);
    	fragmentshader = NULL;
	}
	if(redo_shaders[2] == true && geometryshader != NULL){
		free(geometryshader);
    	geometryshader = NULL;
	}

	char *settings = shadersettings_rw(filepath, NULL);
	settings[settings_len] = '\0';
	FILE *text = fopen(filepath, "rb");
	if (text == NULL) {
		fprintf(stderr, ANSI_RED("Error: Failed to open shader file at '%s'.\n"), filepath);
		return;
	}else{fseek(text, settings_len, SEEK_SET);}
	const dsetting_t decoded = settings_decode(settings);


    int vs_cc = -1, fs_cc = -1, gs_cc = -1;
    char line_buffer[256];
    size_t start_pos, end_pos;
	while(fgets(line_buffer, sizeof(line_buffer), text)){
		for(size_t cc = 0; cc < sizeof(line_buffer); ++cc){
			if(line_buffer[cc] == '\n' || line_buffer[cc] == '\0'){break;}
			if(line_buffer[cc] == '#'){
				const bool which_[3] = {
					strncmp(&line_buffer[cc], vs_start, sizeof(vs_start) - 1) == 0 && redo_shaders[0] == true,
					strncmp(&line_buffer[cc], fs_start, sizeof(vs_start) - 1) == 0 && redo_shaders[1] == true,
					strncmp(&line_buffer[cc], gs_start, sizeof(vs_start) - 1) == 0 && redo_shaders[2] == true
				};
				// Return once all shaders don't need to be re-done.
				if(redo_shaders[0] == false && redo_shaders[1] == false && redo_shaders[2] == false){return;}
				if(which_[0] == true ||which_[1] == true ||which_[2] == true ||which_[3] == true ||which_[4] == true){
					start_pos = ftell(text);
					cc+=sizeof(vs_start);
					size_t cc_ =cc+2;
					// while(line_buffer[cc_+1] != '#' && cc_ < 255){if(line_buffer[cc_] == '\n'){break;}else{++cc_;}}
					// if(cc_ < 255 && line_buffer[cc_] == '#'){if(strcmp(&line_buffer[cc_], shader_end)){end_pos = ftell(text)+cc_;}}
					// else{fgets(line_buffer, sizeof(line_buffer), text);}
					while(fgets(line_buffer, sizeof(line_buffer), text)){
						cc_ =0;
						while(line_buffer[cc_] != '#' && cc_ < 255){
							if(line_buffer[cc_] == '\n'){
								break;
							}else{++cc_;}
						}
						// cc_--;
						if(cc_ == 255 || line_buffer[cc_] == '\n'){continue;}
						if(strncmp(&line_buffer[cc_], shader_end, sizeof(shader_end) - 1) == 0){
							end_pos = ftell(text)+cc_;
							break;
						}
						memset(line_buffer, 0, 256);
					}
					if(end_pos > start_pos){
						size_t offs= (end_pos -start_pos);
						fseek(text, start_pos, SEEK_SET);
						if(which_[0] && redo_shaders[0]){
							//VERTEX.
							vs_cc++;
							if(vs_cc == decoded.vsindex){
								vertexshader = malloc(sizeof(char)* (offs + decoded.version_len + 1));
								strncpy(vertexshader, decoded.version, decoded.version_len);
								// while(vertexshader == NULL){vertexshader = realloc(vertexshader, );}
								fread(vertexshader + decoded.version_len - 1, sizeof(char), offs, text);
								vertexshader[decoded.version_len + offs - 1] = '\0';
								vertexshader[decoded.version_len - 1] = '\n';
								printf(ANSI_YELLOW("vertexshader:\n %s"), vertexshader);
								redo_shaders[0] = false;
								continue;
							}
						}else if(which_[1] && redo_shaders[1]){
							//FRAGMENT.
							fs_cc++;
							if(fs_cc == decoded.fsindex){
								fragmentshader = malloc(sizeof(char)* (offs + decoded.version_len + 1));
								strncpy(fragmentshader, decoded.version, decoded.version_len);
								fread(fragmentshader + decoded.version_len - 1, sizeof(char), offs, text);
								fragmentshader[decoded.version_len + offs - 1] = '\0';
								vertexshader[decoded.version_len - 1] = '\n';
								printf(ANSI_YELLOW("fragmentshader:\n %s"), fragmentshader);
								redo_shaders[1] = false;
								continue;
							}
						}else if(which_[2] && redo_shaders[2]){
							//GEOMETRY.
							gs_cc++;
							if(gs_cc == decoded.gsindex){
								geometryshader = malloc(sizeof(char)* (offs + decoded.version_len + 1));
								strncpy(geometryshader, decoded.version, decoded.version_len);
								fread(geometryshader + decoded.version_len - 1, sizeof(char), offs, text);
								geometryshader[decoded.version_len + offs - 1] = '\0';
								vertexshader[decoded.version_len - 1] = '\n';
								printf(ANSI_YELLOW("geometryshader:\n %s"), geometryshader);
								redo_shaders[2] = false;
								continue;
							}
						}
					}
				}
			}
		}
		memset(line_buffer, 0, 256);
	}
    if (vertexshader == NULL){
		fprintf(stderr, ANSI_YELLOW("Warning: Vertex shader with index %d not found, using default: \n\t\" %s \t\".\n"), decoded.vsindex, vertexshader_default);
		vertexshader = strdup(vertexshader_default);
	}
    if (fragmentshader == NULL){
		printf(ANSI_YELLOW("Warning: Fragment shader with index %d not found, using default: \n\t\" %s \n\".\n"), decoded.fsindex, fragmentshader_default);
		fragmentshader = strdup(fragmentshader_default);
	}
}

/// @brief Parse the global shader (char *) arrays to retrieve .
/// @param uniform_len_ The output array's length.
/// @param shaderProgram The shaderProgram that should be used to get the uniform's locations.
/// @return The arrk_t output.
arrk_t *uniform_init(size_t *uniform_len_, const GLuint shaderProgram){
	const char *shaders[] = {
		vertexshader ? vertexshader : vertexshader_default,
		fragmentshader ? fragmentshader : fragmentshader_default,
		geometryshader ? geometryshader : NULL
	};
	char** temp_typenames = NULL;
	size_t counter = 0;
	size_t typenames_len = 0;
	
	// if(sb->uniforms != NULL || sb->uniform_len > 0){
	// 	for (size_t i = 0; i < sb->uniform_len; ++i) {destroy_arrkey(&sb->uniforms[i]);}
	// 	free(sb->uniforms);
	// 	sb->uniforms = NULL;
	// 	sb->uniform_len = 0;
	// }
	// size_t sb_uniform_size = 0;
	// for(size_t cc =0; cc < sb->uniform_len; ++cc){sb_uniform_size += strlen(sb->uniforms[sb->uniform_len].name) + strlen(sb->uniforms[sb->uniform_len].type) + sizeof(sb->uniforms[sb->uniform_len].Location);}
	arrk_t *uniforms = calloc(sizeof(arrk_t), 1);
	size_t uniform_len = 0;
	for(size_t cc =0; cc < 3 && shaders[cc] != NULL; ++cc){
		if(shaders[cc] == NULL){continue;}
		const size_t shader_len = strlen(shaders[cc]);
		for(size_t char_cc =0; char_cc < shader_len; ++char_cc){
			while(char_cc < shader_len && isspace(shaders[cc][char_cc])){
				++char_cc;
			}
			// printf(ANSI_COLOR_YELLOW "%c" ANSI_COLOR_RESET, shaders[cc][char_cc]);
			if(shaders[cc][char_cc] == 'u'){
				//Might be uniform.
				if(strncmp(shaders[cc] + char_cc, "uniform", 7) == 0){
					char_cc+= 8;
					while(char_cc < shader_len && isspace(shaders[cc][char_cc])){++char_cc;}
					unsigned int len_char_cc =0;
					//Get type definition.
					while(len_char_cc + char_cc < shader_len && isalnum(shaders[cc][char_cc+len_char_cc])){++len_char_cc;}
					uniforms = realloc(uniforms, sizeof(arrk_t) * (uniform_len + 1));
					uniforms[uniform_len].type = malloc(sizeof(char)* (len_char_cc + 1));
					memcpy(uniforms[uniform_len].type, shaders[cc] + char_cc, len_char_cc);
					uniforms[uniform_len].type[len_char_cc] = '\0';
					char_cc += len_char_cc;
					len_char_cc = 0;
					//Get name definition.
					while(char_cc < shader_len && isspace(shaders[cc][char_cc])){char_cc++;}
					while(len_char_cc + char_cc < shader_len && isalnum(shaders[cc][char_cc+len_char_cc])){++len_char_cc;}
					uniforms[uniform_len].name = malloc(sizeof(char)* (len_char_cc + 1));
					memcpy(uniforms[uniform_len].name, shaders[cc] + char_cc, len_char_cc);
					uniforms[uniform_len].name[len_char_cc] = '\0';
					if((uniforms[uniform_len].Location = glGetUniformLocation(shaderProgram, uniforms[uniform_len].name)) == -1){
						printf(ANSI_RED("Uh-oh!, I couldn't access the uniform %s of type %s and at the index: %d"), uniforms[uniform_len].name, uniforms[uniform_len].type, char_cc);
					}else{printf(ANSI_GREEN("Found uniform %s of type %s with Shader location: %d and at the index: %d"), uniforms[uniform_len].name, uniforms[uniform_len].type, uniforms[uniform_len].Location, char_cc + counter);}
					uniform_len++;
					counter += strlen(shaders[cc]);
				}
			}// else if(shaders[cc][char_cc] == 's'){continue;}
		}
	}
	*uniform_len_ = uniform_len++;
	return uniforms;
}

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
bool uniform_write(shaderblock_t* shader, const char* type, const char* name, const void* property_, const bool transpose, void* value, const size_t num_elements){
	GLUseProgram(shader->shaderProgram);
#ifdef _DEBUG_
	for(uint8_t cc =0; cc < shader->uniform_len; ++cc){
		printf(ANSI_BLUE("\n%u: \n  Name: %s \n  Type: %s \n  Location: %d"), 
			cc, 
			shader->uniforms[cc].name,
			shader->uniforms[cc].type,
			shader->uniforms[cc].Location
		);
	}
#endif
	assert(shader != NULL);
	assert(shader->uniforms != NULL);
	assert(shader->uniform_len < 1000);
	if (shader == NULL || name == NULL){return false;}
	size_t *typehash_ = str_hash(str_normalise(type, true, true));
	const uint128_t typehash = {typehash_[0], typehash_[1]};
	GLint location = -1;
	// Find the uniform location
	printf(ANSI_YELLOW("\nShader Program ID: %u\n"), shader->shaderProgram);
	// GLUseProgram(shader->shaderProgram);
	for (size_t cc = 0; cc < shader->uniform_len; ++cc) {
		if (strcmp(name, shader->uniforms[cc].name) == 0) {
			location = shader->uniforms[cc].Location;
			if((location = glGetUniformLocation(shader->shaderProgram, shader->uniforms[cc].name)) != shader->uniforms[cc].Location){
				shader->uniforms[cc].Location = location;
				if(location == -1){printf(ANSI_RED("\tUniform %s not found, \n\t\tLocation %u returned"), shader->uniforms[cc].name, location);}
			}
			printf(ANSI_YELLOW("\nLocation: %d"), location);
			break;
		}
	}

	if (location == -1) {
		fprintf(stderr, ANSI_YELLOW("Warning: Uniform '%s' not found.\n"), name);
		return false;
	}
	// GLCallUseProgram(shader->shaderProgram);
	for(size_t cc =0; cc < 37; ++cc){
		if(uint128_t_comp(str_hash(str_normalise(builtin_shader_typenames[cc], true, true)), typehash) == true){
			// Use a switch statement for dispatching GL uniform functions
			// Note: The original switch had incorrect pointer casts and logic for single vs. multiple elements.
			GLUseProgram(shader->shaderProgram);
			switch (cc) {
				case 0: // GL_BOOL
				case 1: // GL_INT
					GLCall(glUniform1i(location, *((GLint*)value)));
					break;
				case 2: // GL_UNSIGNED_INT
					GLCall(glUniform1uiv(location, num_elements,  (GLuint*)value));
					break;
				case 3: // GL_FLOAT
					GLfloat tempf = *((GLfloat*)value);
					GLCall(glUniform1f(location, tempf));
					break;
				case 4: // float vec2
					GLfloat *tempf2 = (GLfloat*)value;
					GLCall(glUniform2f(location, 
						tempf2[0], 
						(num_elements >= 2? tempf2[1]: 0)
					));
					break;
				case 5: // float vec3
					GLfloat *tempf3 = (GLfloat*)value;
					GLCall(glUniform3f(location, 
						tempf3[0], 
						(num_elements >= 2? tempf3[1]: 0), 
						(num_elements >= 3? tempf3[2]: 0)
					));
					break;
				case 6: // float vec4
					GLfloat *tempf4 = (GLfloat*)value;
					GLCall(glUniform4f(location, 
						tempf4[0], 
						(num_elements >= 2? tempf4[1]: 0), 
						(num_elements >= 3? tempf4[2]: 0), 
						(num_elements >= 4? tempf4[3]: 0)
					));
					break;
				case 7: // int vec2
					GLint *tempi2 = (GLint*)value;
					GLCall(glUniform2i(location, 
						tempi2[0], 
						(num_elements >= 2? tempi2[1]: 0)
					));
					break;
				case 8: // int vec3
					GLint *tempi3 = (GLint*)value;
					GLCall(glUniform3i(location, 
						tempi3[0], 
						(num_elements >= 2? tempi3[1]: 0), 
						(num_elements >= 3? tempi3[2]: 0)
					));
					break;
				case 9: // int vec4
					GLint *tempi4 = (GLint*)value;
					GLCall(glUniform4i(location, 
						tempi4[0], 
						(num_elements >= 2? tempi4[1]: 0), 
						(num_elements >= 3? tempi4[2]: 0), 
						(num_elements >= 4? tempi4[3]: 0)
					));
					break;
				case 10: // unsigned int vec2
					GLuint *tempui2 = (GLuint*)value;
					GLCall(glUniform2ui(location, 
						tempui2[0], 
						(num_elements >= 2? tempui2[1]: 0)
					));
				case 11: // unsigned int vec3
					GLuint *tempui3 = (GLuint*)value;
					GLCall(glUniform3ui(location, 
						tempui3[0], 
						(num_elements >= 2? tempui3[1]: 0), 
						(num_elements >= 3? tempui3[2]: 0)
					));
				case 12: // unsigned int vec4
					GLuint *tempui4 = (GLuint*)value;
					GLCall(glUniform4ui(location, 
						tempui4[0], 
						(num_elements >= 2? tempui4[1]: 0), 
						(num_elements >= 3? tempui4[2]: 0), 
						(num_elements >= 4? tempui4[3]: 0)
					));
				case 13: // Boolean vec2
					GLint *tempb2 = (GLint *)value;
					GLCall(glUniform2i(location,
						tempb2[0],
						(num_elements >= 2? tempb2[1]: 0)
					));
				case 14: // Boolean vec3
					GLint *tempb3 = (GLint *)value;
					GLCall(glUniform3i(location,
						tempb3[0],
						(num_elements >= 2? tempb2[1]: 0),
						(num_elements >= 3? tempb2[2]: 0)
					));
				case 15: // Boolean vec4
					GLint *tempb4 = (GLint *)value;
					GLCall(glUniform4i(location,
						tempb4[0],
						(num_elements >= 2? tempb2[1]: 0), 
						(num_elements >= 3? tempb2[2]: 0), 
						(num_elements >= 4? tempb2[3]: 0)
					));
				case 16: // mat2
					GLCall(glUniformMatrix2fv(location, num_elements, transpose, (const GLfloat*)value));
					break;
				case 17: // mat3
					GLCall(glUniformMatrix3fv(location, num_elements, transpose, (const GLfloat*)value));
					break;
				case 18: // mat4
					GLCall(glUniformMatrix4fv(location, num_elements, transpose, (const GLfloat*)value));
					break;
				case 19: // mat2x3
					GLCall(glUniformMatrix2x3fv(location, num_elements, transpose, (const GLfloat*)value));
					break;
				case 20: // mat3x2
					GLCall(glUniformMatrix3x2fv(location, num_elements, transpose, (const GLfloat*)value));
					break;
				case 21: // mat2x4
					GLCall(glUniformMatrix2x4fv(location, num_elements, transpose, (const GLfloat*)value));
					break;
				case 22: // mat4x2
					GLCall(glUniformMatrix4x2fv(location, num_elements, transpose, (const GLfloat*)value));
					break;
				case 23: // mat3x4
					GLCall(glUniformMatrix3x4fv(location, num_elements, transpose, (const GLfloat*)value));
					break;
				case 24: // mat4x3
					GLCall(glUniformMatrix4x3fv(location, num_elements, transpose, (const GLfloat*)value));
					break;
				case 25: // Sampler 1D
				case 26: // Sampler 2D
				case 27: // Sampler 3D
				case 28: // Sampler Cube
				case 29: // Sampler 1D Shadow
				case 30: // Sampler 2D Shadow
				case 31: // Sampler 2D Array
				case 32: // Sampler 2D Array Shadow
				case 33: // Integer Sampler 1D
				case 34: // Integer Sampler 2D
				case 35: // Unsigned Integer Sampler 1D
				case 36: // Unsigned Integer Sampler 2D
					const GLenum property_enum = *((GLenum *)property_);
					switch(property_enum){
						case GL_TEXTURE_1D:
						case GL_TEXTURE_2D:
						case GL_TEXTURE_3D:
						case GL_TEXTURE_CUBE_MAP:
						case GL_TEXTURE_1D_ARRAY:
						case GL_TEXTURE_2D_ARRAY:
							glUniform1i(location, *((const GLint*)value));
							break;
					}
					break;
				default:
					//Just attempt writing something ngl.
					// fprintf(stderr, "Error: Unsupported shader type or uniform '%s' not found.\n", type);
					const char *property = property_;
					size_t full_len = strlen(name) + (property == NULL? 0: strlen(property)) + 3 + 1;
					char *full_access = malloc(full_len);
					if (!full_access){break;}
					snprintf(full_access, full_len, "%s->%s", name, property);

					GLint out = glGetUniformLocation(shader->shaderProgram, full_access);
					if (out == -1) {
						if(property != NULL){snprintf(full_access, full_len, "%s.%s", name, property);}
						out = glGetUniformLocation(shader->shaderProgram, full_access);
						if (out == -1) {
							free(full_access);
							break;
						}
					}
					GLCall(glUniform1f(out, ((GLfloat*)value)[0]));
					free(full_access);
					break;
			}
			break;
		}
	}
	free(typehash_);
	return true;
}


// Corrected shader_compile function.
// The original had a few logical errors, incorrect checks, and was missing error handling for `calloc`.
shaderblock_t *shader_compile(bool delete_shaders_on_link){
	shaderblock_t* shaderblock = calloc(1, sizeof(shaderblock_t));
	if (shaderblock == NULL) {
		fprintf(stderr, ANSI_RED("Error: Failed to allocate memory for shaderblock.\n"));
		return NULL;
	}
	// memset(shaderblock, 0, sizeof(shaderblock_t));

	// Compile Vertex Shader.
	shaderblock->vertexshader = glCreateShader(GL_VERTEX_SHADER);
	const char* vs_source = (vertexshader != NULL) ? vertexshader : vertexshader_default;
	glShaderSource(shaderblock->vertexshader, 1, &vs_source, NULL);
	glCompileShader(shaderblock->vertexshader);
	shaderblock->compiled_[1] = shader_error(shaderblock, "VERTEX");
	if(shaderblock->compiled_[1]){shaderblock->vertex = strdup(vs_source);}

	// Compile Fragment Shader.
	shaderblock->fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fs_source = (fragmentshader != NULL) ? fragmentshader : fragmentshader_default;
	glShaderSource(shaderblock->fragmentshader, 1, &fs_source, NULL);
	glCompileShader(shaderblock->fragmentshader);
	shaderblock->compiled_[2] = shader_error(shaderblock, "FRAGMENT");
	if(shaderblock->compiled_[2]){shaderblock->fragment = strdup(fs_source);}

	// Compile Geometry Shader (optional).
	if(geometryshader != NULL){
		shaderblock->geometryshader = glCreateShader(GL_GEOMETRY_SHADER);
		const char *gs_source = geometryshader;
		glShaderSource(shaderblock->geometryshader, 1, &gs_source, NULL);
		glCompileShader(shaderblock->geometryshader);
		shaderblock->compiled_[3] = shader_error(shaderblock, "GEOMETRY");
		if(shaderblock->compiled_[3]){shaderblock->geometry = strdup(geometryshader);}
	}else{
		shaderblock->geometryshader = 0;
		shaderblock->compiled_[3] = false;
	}

	// Cache if needed
	
	// Link Program.
	shaderblock->shaderProgram = glCreateProgram();
	GLCall(glAttachShader(shaderblock->shaderProgram, shaderblock->vertexshader));
	GLCall(glAttachShader(shaderblock->shaderProgram, shaderblock->fragmentshader));
	if(shaderblock->geometryshader != 0){GLCall(glAttachShader(shaderblock->shaderProgram, shaderblock->geometryshader));}
	GLCall(glLinkProgram(shaderblock->shaderProgram));
	shaderblock->compiled_[0] = shader_error(shaderblock, "PROGRAM");
	shaderblock->compiled_[6] = shaderblock->compiled_[0];
	
	// Delete if needed.
	if (delete_shaders_on_link) {
		for(uint8_t cc =1; cc < 6; ++cc){shaderblock->compiled_[cc] = false;}
		glDeleteShader(shaderblock->vertexshader);
		glDeleteShader(shaderblock->fragmentshader);
		if(shaderblock->geometryshader != 0){glDeleteShader(shaderblock->geometryshader);}
	}
	
	// The original code was using `shaderblock->compiled_` and `shaderblock->_compiled`.
	// It's likely a typo. I've assumed one of them is the correct member.
	// I am not fixing this, but instead removing the `_compiled` checks, as they are not 
	// standard practice and the `shader_error` function already reports issues.
	
	return shaderblock;
}
	
	
// 	win->textures_len++;
// }

// Fixed win_flood function.
// The original had no issues, but some minor formatting improvements were made for clarity.
void win_flood(win_t* win, const argb_t c){
	if (win == NULL || win->g_window == NULL) return;
	
	// glViewport(0, 0, win->w, win->h);
	GLCall(glClearColor(1, 0, 0.1, 1));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	// glfwSwapBuffers(win->g_window);
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
			glDeleteBuffer(buffer->VBO);
			return NULL;
		case BUFFER_OPTIONS_CLEAR_EBO:
			glDeleteBuffer(buffer->EBO);
			return NULL;
		case BUFFER_OPTIONS_FREE:
			// Free the buffer object itself.
			buffer_bufferdo(buffer, 1, BUFFER_OPTIONS_CLEAR);
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
				fprintf(stderr, ANSI_RED("Error: Memory allocation failed in BUFFER_OPTIONS_COLLECT_VAO.\n"));
				return NULL;
			}else{
				for(size_t cc =0; cc < len; ++cc){out[cc] = buffer[cc].VAO;}
				return out;
			}
		case BUFFER_OPTIONS_COLLECT_VBO_MULTIPLE:
			GLuint *out_vbo = (GLuint*)malloc(sizeof(GLuint) * len);
			if (out_vbo == NULL) {
				fprintf(stderr, ANSI_RED("Error: Memory allocation failed in BUFFER_OPTIONS_COLLECT_VBO_MULTIPLE.\n"));
				return NULL;
			}else{
				for(size_t cc =0; cc < len; ++cc){out_vbo[cc] = buffer[cc].VBO;}
				return out_vbo;
			}
		case BUFFER_OPTIONS_COLLECT_EBO_MULTIPLE:
			GLuint *out_ebo = (GLuint*)malloc(sizeof(GLuint) * len);
			if (out_ebo == NULL) {
				fprintf(stderr, ANSI_RED("Error: Memory allocation failed in BUFFER_OPTIONS_COLLECT_EBO_MULTIPLE.\n"));
				return NULL;
			}else{
				for(size_t cc =0; cc < len; ++cc){out_ebo[cc] = buffer[cc].EBO;}
				return out_ebo;
			}
		default:
			fprintf(stderr, ANSI_RED("Error: Unknown BUFFER_OPTIONS value %d.\n"), option);
			return NULL;
		
	}
}

shaderblock_t *shaderblock_gen(bool compile, bool clean, bool do_uniforms, char *path){
	if(vertexshader == NULL || fragmentshader == NULL){
		printf(ANSI_YELLOW("Vertex or Fragment Shader not set, resolving.\n"));
		char *temp = NULL;
		if(path){temp = path;
		}else{
			temp = strdup(cwd);
			// default_dir = realloc(default_dir, (cwd_len)* sizeof(char));
			strncat(temp, "\\Resources\\Shaders\\Shaders.glsl", 32);
		}
		shaders_pull(temp);
	}
	if(compile){
		shaderblock_t *shader = shader_compile(clean);
		if(do_uniforms == true){shader->uniforms = uniform_init(&shader->uniform_len, shader->shaderProgram);}
		return shader;
	}
	return NULL;
}

void shaderblock_handle(shaderblock_t *sb, bool clean, bool do_uniforms){
	if(sb == NULL){
		*sb = *shader_compile(clean);
		if(do_uniforms){sb->uniforms = uniform_init(&sb->uniform_len, sb->shaderProgram);}
		return;
	}
	if(glIsVertexArray(sb->shaderProgram) == GL_TRUE && sb->compiled_[6] == true){return;}
	GLint status;
	glGetProgramiv(sb->shaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE){
		sb = shader_compile(clean);
		if(do_uniforms){sb->uniforms = uniform_init(&sb->uniform_len, sb->shaderProgram);}
	}
	// Dump log
	char log[512];
	glGetProgramInfoLog(sb->shaderProgram, 512, NULL, log);
	if(strlen(log) == 0){return;}else{
		printf("\nLink error: %s\n", log);
	}

}

/// @brief Generate a Buffer Object for 1 Mesh.
/// @param mesh The mesh to be given a Buffer.
/// @param draw_format The format to be used when attributing.
/// @param pos_layout The Layout the Position vector should be applied to.
/// @param col_layout The Layout the Colour vector should be applied to.
/// @param tex_layout The Layout the Texture pixel data should be applied to.
/// @return A fully generated texture.
bufferobj_t *bufferobj_gen(mesh_t *mesh, const GLenum draw_format){
	bufferobj_t *out = calloc(1, sizeof(bufferobj_t));
	bufferobject_handle(out, mesh->vertex_data, mesh->data_len, mesh->index_data, mesh->index_len, draw_format, 9);
	mesh_attrlink(out, mesh);
	return out;
}

void bufferobject_handle(bufferobj_t *buffer, GLfloat *vertices, size_t v_len, GLuint *index_order, size_t index_len, GLenum draw_format, uint8_t max_tries){
	bool success = 0;
	uint8_t counter = 0;
	do {
		if(buffer == NULL){break;}
		success = true;
		/*VAO not set-up*/ 
		if (buffer->buffer_[0] == false) {
			if (glIsVertexArray(buffer->VAO) == GL_FALSE){GLCall(glGenVertexArrays(1, &buffer->VAO));}	
			buffer->buffer_[0] = true;
			//Generate VBO.
			GLCall(glBindVertexArray(buffer->VAO)); // Begin recording state.
			if(glIsBuffer(buffer->VBO) == GL_FALSE){
				GLCall(glGenBuffer(buffer->VBO));
				GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer->VBO));
				buffer->buffer_[1] = false;
			}else{GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer->VBO));}
			if(glIsBuffer(buffer->EBO) == GL_FALSE){
				GLCall(glGenBuffer(buffer->EBO));
				GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->EBO));
				buffer->buffer_[2] = false;
			}else{GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->EBO));}
		}
		draw_debug_trace(__FILE__, __LINE__);
		/*VBO not set-up*/
		if(buffer->buffer_[1] == false && vertices != NULL){	
			GLCall(glBufferData(GL_ARRAY_BUFFER, v_len * sizeof(GLfloat), vertices, draw_format));
			DEBUG_BUFFER_STATE(GL_ARRAY_BUFFER, "VBO");
			buffer->buffer_[1] = true;
		} 
		/*EBO not set-up*/ 
		if(buffer->buffer_[2] == false && index_order != NULL){  
			GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_len * sizeof(GLuint), index_order, draw_format));
			DEBUG_BUFFER_STATE(GL_ELEMENT_ARRAY_BUFFER, "EBO");
			buffer->buffer_[2] = true;
		}

		draw_debug_trace(__FILE__, __LINE__);

		success = buffer->buffer_[0] == true && buffer->buffer_[1] == true && buffer->buffer_[2] == true;
		counter++;
	}while (counter < max_tries && success == false);
	GLCall(glBindVertexArray(0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	// draw_debug_trace(__FILE__, __LINE__);
}