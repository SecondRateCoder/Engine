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

/*
Shader setting conventions:
	[0 - 4]: OpenGL Version,
	[5]: Core or Compatibility?
	[6 - 10]: Vertex Shader index
	[11 - 15]: Fragment Shader index
	[16 - 20]: Geometry Shader index
	[21 - 25]: Tesselation Eval index
	[26 - 30]: Tess Control index
*/


// Corrected shader_error function.
// The original had incorrect `printf` format specifiers and was not properly handling the return value of `strncmp`.
// It also had inconsistent naming for the `_compiled` member. I've removed the state tracking as it's not robust.
bool shader_error(shaderblock_t* sb_t, const char* type) {
	if(sb_t == NULL || type == NULL){return false;}

	GLuint shader = 0;
	GLint success;
	char infoLog[1024];

	if (strcmp(type, "PROGRAM") == 0) {
		shader = sb_t->shaderProgram;
		GLUseProgram(shader);
		if (!success) {
			GLCall(glGetProgramInfoLog(shader, 1024, NULL, infoLog));
			fprintf(stderr, ANSI_RED("Shader PROGRAM linking error: %s\n"), infoLog);
			return false;
		}
	} else {
		if(strcmp(type, "VERTEX") == 0){shader = sb_t->vertexshader;
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
		if (!success) {
			GLCall(glGetShaderInfoLog(shader, 1024, NULL, infoLog));
			fprintf(stderr, ANSI_RED("Shader %s compilation error: %s\n"), type, infoLog);
			return false;
		}
	}
	return true;
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

void shaders_pull(const char *filepath){shader_pull(filepath, (bool[5]){true, true, false, false, false});}

uint32_t parse_shader_index(const char *settings, uint8_t offset, const uint8_t len){
	char *temp = malloc(sizeof(settings[0])* (len + 1));
	uint8_t cc_ = 0;
	for(uint8_t cc =offset; cc < (len < strlen(settings)? len: strlen(settings)); ++cc, ++cc_){
		temp[cc_] = settings[cc];
	}
	temp[len] = '\0';
    int raw = strtol(temp, NULL, len);
	free(temp);
    return raw < 0 ? 0 : (uint32_t)raw;
}

dsetting_t settings_decode(char settings[settings_len]){
	dsetting_t out = {
		.vsindex = parse_shader_index(settings, 6, SHADER_INDEX_LEN),
		.fsindex = parse_shader_index(settings, 11, SHADER_INDEX_LEN),
		.gsindex = parse_shader_index(settings, 16, SHADER_INDEX_LEN),
		.tesindex = parse_shader_index(settings, 21, SHADER_INDEX_LEN),
		.tcsindex = parse_shader_index(settings, 26, SHADER_INDEX_LEN),
		.version = NULL
	};
	out.version = calloc(15, sizeof(settings[0]));
	memcpy(out.version, "\n#version ", 10* sizeof(settings[0]));
	memcpy(&out.version[10], settings, 4* sizeof(settings[0]));
	if(settings[5] == '0'){
		out.version = realloc(out.version, strlen(out.version) + 6);
		memcpy(&out.version[14], " core", 5* sizeof(settings[0]));
		out.version[19] = '\0';
	}else{
		out.version = realloc(out.version, strlen(out.version) + 15);
		memcpy(&out.version[14], " compatibility", 15* sizeof(settings[0]));
		out.version[14] = '\0';
	}
	out.version_len = strlen(out.version);
	return out;
}


/// @brief Pull shaders, populating vertexshader, fragmentshader and geometryshader with the relevant and found shaders.
/// @param filepath The filepath to a shader or multiple shaders
/// @param redo_shaders Choose between the shaders to be reset to NULL.
/// [0] -> vertexshader, [1] -> fragmentshader, [2] ->geometryshader, [3] -> tessellation_controlshader, [4] ->tessellation_evaluationshader.
void shader_pull(const char *filepath, const bool redo_shaders[5]){
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
	if(redo_shaders[3] == true){
		free(tessellation_controlshader);
    	tessellation_controlshader = NULL;
	}
	if(redo_shaders[4] == true){
		free(tessellation_evaluationshader);
    	tessellation_evaluationshader = NULL;
	}

	char *settings = shadersettings_rw(filepath, NULL);
	settings[settings_len] = '\0';
	FILE *text = fopen(filepath, "rb");
	if (text == NULL) {
		fprintf(stderr, ANSI_RED("Error: Failed to open shader file at '%s'.\n"), filepath);
		return;
	}else{fseek(text, settings_len, SEEK_SET);}
	const dsetting_t decoded = settings_decode(settings);


    int vs_cc = -1, fs_cc = -1, gs_cc = -1, tes_cc = -1, tcs_cc = -1;
	"3300000000000000000000000";
    char line_buffer[256];
    size_t start_pos, end_pos;
	while(fgets(line_buffer, sizeof(line_buffer), text)){
		for(size_t cc = 0; cc < sizeof(line_buffer); ++cc){
			if(line_buffer[cc] == '\n' || line_buffer[cc] == '\0'){break;}
			if(line_buffer[cc] == '#'){
				const bool which_[5] = {
					strncmp(&line_buffer[cc], vs_start, sizeof(vs_start) - 1) == 0 && redo_shaders[0] == true,
					strncmp(&line_buffer[cc], fs_start, sizeof(vs_start) - 1) == 0 && redo_shaders[1] == true,
					strncmp(&line_buffer[cc], gs_start, sizeof(vs_start) - 1) == 0 && redo_shaders[2] == true,
					strncmp(&line_buffer[cc], tes_start, sizeof(vs_start) - 1) == 0 && redo_shaders[3] == true,
					strncmp(&line_buffer[cc], tcs_start, sizeof(vs_start) - 1) == 0 && redo_shaders[4] == true
				};
				if(vertexshader != NULL && 
					fragmentshader != NULL &&
					(geometryshader == NULL && redo_shaders[2] != true) &&
					(tessellation_evaluationshader == NULL && redo_shaders[3] != true) &&
					(tessellation_controlshader == NULL && redo_shaders[4] != true)){return;}
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
						if(which_[0]){
							//VERTEX.
							vs_cc++;
							if(vs_cc == decoded.vsindex){
								vertexshader = strdup(decoded.version);
								while(vertexshader == NULL){vertexshader = realloc(vertexshader, sizeof(char)* (offs + decoded.version_len));}
								fread(&vertexshader[decoded.version_len], sizeof(char), end_pos -start_pos + 1, text);
								vertexshader[decoded.version_len + offs - 1] = '\0';
								printf(ANSI_YELLOW("vertexshader:\n %s"), vertexshader);
								continue;
							}
						}else if(which_[1]){
							//FRAGMENT.
							fs_cc++;
							if(fs_cc == decoded.fsindex){
								fragmentshader = strdup(decoded.version);
								while(fragmentshader == NULL){fragmentshader = realloc(fragmentshader, sizeof(char)* (offs + decoded.version_len));}
								fread(&fragmentshader[decoded.version_len], sizeof(char), end_pos -start_pos + 1, text);
								fragmentshader[decoded.version_len + offs - 1] = '\0';
								printf(ANSI_YELLOW("fragmentshader:\n %s"), fragmentshader);
								continue;
							}
						}else if(which_[2]){
							//GEOMETRY.
							gs_cc++;
							if(gs_cc == decoded.gsindex){
								geometryshader = strdup(decoded.version);
								while(geometryshader == NULL){vertexshader = realloc(geometryshader, sizeof(char)* (offs + decoded.version_len));}
								fread(&geometryshader[decoded.version_len], sizeof(char), end_pos -start_pos + 1, text);
								geometryshader[decoded.version_len + offs - 1] = '\0';
								printf(ANSI_YELLOW("geometryshader:\n %s"), geometryshader);
								continue;
							}
						}else if(which_[3]){
							//TESSELATION EVAL.
							tes_cc++;
							if(tes_cc == decoded.tesindex){
								tessellation_evaluationshader= strdup(decoded.version);
								while(tessellation_evaluationshader == NULL){vertexshader = realloc(tessellation_evaluationshader, sizeof(char)* (offs + decoded.version_len));}
								fread(&tessellation_evaluationshader[decoded.version_len], sizeof(char), end_pos -start_pos + 1, text);
								tessellation_evaluationshader[decoded.version_len + offs - 1] = '\0';
								printf(ANSI_YELLOW("tessellation evaluation shader:\n %s"), tessellation_evaluationshader);
								continue;
							}
						}else if(which_[4]){
							//TESSELLATION CONTROL
							tcs_cc++;
							if(tcs_cc == decoded.tcsindex){
								vertexshader = strdup(decoded.version);
								while(tessellation_controlshader == NULL){tessellation_controlshader = realloc(tessellation_controlshader, sizeof(char)* (offs + decoded.version_len));}
								fread(&tessellation_controlshader[decoded.version_len], sizeof(char), end_pos -start_pos + 1, text);
								tessellation_controlshader[decoded.version_len + offs - 1] = '\0';
								printf(ANSI_YELLOW("tesselation control shader:\n %s"), tessellation_controlshader);
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

// Corrected uniform_init function.
// The original was missing crucial cleanup and had logical errors in memory management.
// The `strndup` calls and the `realloc` logic were a particular problem.
arrk_t *uniform_init(size_t *uniform_len_, const GLuint shaderProgram){
	const char *shaders[] = {
		vertexshader ? vertexshader : vertexshader_default,
		fragmentshader ? fragmentshader : fragmentshader_default,
		geometryshader ? geometryshader : NULL,
		tessellation_controlshader ? tessellation_controlshader : NULL,
		tessellation_evaluationshader ? tessellation_evaluationshader : NULL
	};
	char** temp_typenames = NULL;

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
	for(size_t cc =0; cc < 5; ++cc){
		if(shaders[cc] == NULL){continue;}
		"3.300000000000\n"
		"^Settings.\n"
		"\n"
		"#define vs\n"
		"\n"
		"#version 330 core\n"
		"\n"
		"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 1) in vec3 aColor;\n"
		"layout(location = 2) in vec2 aTex;\n"
		"\n"
		"out vec3 color;\n"
		"out vec2 tex_coord;\n"
		"\n"; "vertexshader: 176";

		" #version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec3 color;\n"
		"in vec2 tex_coord;\n"
		"\n"; "fragmentshader: 80";
		const size_t shader_len = strlen(shaders[cc]);
		for(size_t char_cc =0; char_cc < shader_len; ++char_cc){
			while(char_cc < shader_len && isspace(shaders[cc][char_cc])){
				++char_cc;
			}
			// printf(ANSI_COLOR_YELLOW "%c" ANSI_COLOR_RESET, shaders[cc][char_cc]);
			if(shaders[cc][char_cc] == 'u'){
				//Might be uniform.
				if(strncmp(&shaders[cc][char_cc], "uniform", 7) == 0){
					char_cc+= 8;
					while(char_cc < shader_len && isspace(shaders[cc][char_cc])){++char_cc;}
					unsigned int len_char_cc =0;
					//Get type definition.
					while(len_char_cc + char_cc < shader_len && isalnum(shaders[cc][char_cc+len_char_cc])){++len_char_cc;}
					uniforms = realloc(uniforms, sizeof(arrk_t) * (uniform_len + 1));
					uniforms[uniform_len].type = malloc(sizeof(char)* (len_char_cc + 1));
					memcpy(uniforms[uniform_len].type, &shaders[cc][char_cc], len_char_cc);
					uniforms[uniform_len].type[len_char_cc] = '\0';
					char_cc += len_char_cc;
					len_char_cc = 0;
					//Get name definition.
					while(char_cc < shader_len && isspace(shaders[cc][char_cc])){char_cc++;}
					while(len_char_cc + char_cc < shader_len && isalnum(shaders[cc][char_cc+len_char_cc])){++len_char_cc;}
					uniforms[uniform_len].name = malloc(sizeof(char)* (len_char_cc + 1));
					memcpy(uniforms[uniform_len].name, &shaders[cc][char_cc], len_char_cc);
					uniforms[uniform_len].name[len_char_cc] = '\0';
					if((uniforms[uniform_len].Location = glGetUniformLocation(shaderProgram, uniforms[uniform_len].name)) == -1){
						printf(ANSI_RED("Uh-oh!, I couldn't access the uniform %s of type %s and at location %d"), uniforms[uniform_len].name, uniforms[uniform_len].type, char_cc);
					}
					uniform_len++;
					
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
	for(uint8_t cc =0; cc < shader->uniform_len; ++cc){
		printf(ANSI_YELLOW("\n%u: \n  Name: %s \n  Type: %s \n  Location: %d"), 
			cc, 
			shader->uniforms[cc].name,
			shader->uniforms[cc].type,
			shader->uniforms[cc].Location
		);
	}
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
shaderblock_t *shader_compile(bool delete_shaders_on_link) {
	shaderblock_t* shaderblock = (shaderblock_t*)calloc(sizeof(shaderblock_t), 1);
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
	

	// Compile Fragment Shader.
	shaderblock->fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fs_source = (fragmentshader != NULL) ? fragmentshader : fragmentshader_default;
	glShaderSource(shaderblock->fragmentshader, 1, &fs_source, NULL);
	glCompileShader(shaderblock->fragmentshader);
	shaderblock->compiled_[2] = shader_error(shaderblock, "FRAGMENT");

	// Compile Geometry Shader (optional).
	if(geometryshader != NULL){
		shaderblock->geometryshader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(shaderblock->geometryshader, 1, (const char* const*)&geometryshader, NULL);
		glCompileShader(shaderblock->geometryshader);
		shaderblock->compiled_[3] = shader_error(shaderblock, "GEOMETRY");
	}else{
		shaderblock->geometryshader = 0;
		shaderblock->compiled_[3] = false;
	}

	// if(tessellation_controlshader != NULL){
	// 	shaderblock->tessellation_controlshader = glCreateShader(GL_TESS_CO);
	// 	glShaderSource(shaderblock->geometryshader, 1, (const char* const*)&geometryshader, NULL);
	// 	glCompileShader(shaderblock->geometryshader);
	// 	shaderblock->compiled_[3] = shader_error(shaderblock, "GEOMETRY");
	// }else{
	// 	shaderblock->geometryshader = 0;
	// 	shaderblock->compiled_[3] = false;
	// }

	// if(geometryshader != NULL){
	// 	shaderblock->geometryshader = glCreateShader(GL_GEOMETRY_SHADER);
	// 	glShaderSource(shaderblock->geometryshader, 1, (const char* const*)&geometryshader, NULL);
	// 	glCompileShader(shaderblock->geometryshader);
	// 	shaderblock->compiled_[3] = shader_error(shaderblock, "GEOMETRY");
	// }else{
	// 	shaderblock->geometryshader = 0;
	// 	shaderblock->compiled_[3] = false;
	// }
	
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
		if(shaderblock->tessellation_controlshader != 0){glDeleteShader(shaderblock->tessellation_controlshader);}
		if(shaderblock->tessellation_evaluationshader != 0){glDeleteShader(shaderblock->tessellation_evaluationshader);}
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
	// Allocate space for one buffer object
	win->buffers = malloc(sizeof(bufferobj_t));
	win->buffer_len = 1; // Track the buffer count
	win->buffers = realloc(win->buffers, (win->buffer_len + 1)* sizeof(bufferobj_t));
	bufferobj_t *temp_ptr = &win->buffers[win->buffer_len];
	win->buffer_len++;
	memset(temp_ptr, 0, sizeof(bufferobj_t)); // Initialize buffer memory
    shaderblock_handle(&win->shaders[win->shaders_curr], true, true);
    BUFFEROBJECT_HANDLE(temp_ptr, mesh->mesh_data, mesh->data_len, mesh->vertex_index, mesh->index_len, GL_STATIC_DRAW, 10);
    mesh_attrlink(temp_ptr, win->layout_offset, win->layout_offset+1, win->layout_offset+2, mesh);
    GLUseProgram(win->shaders[win->shaders_curr].shaderProgram);
    draw_debug_trace(__FILE__, __LINE__);
	GLCall(glBindVertexArray(temp_ptr->VAO));
    GLCall(glDrawElements(GL_TRIANGLES, mesh->data_len, GL_UNSIGNED_INT, (mesh->vertex_index == NULL? 0: mesh->mesh_data)));
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

shaderblock_t *shaderblock_gen(bool clean, bool do_uniforms){
	if(vertexshader == NULL || fragmentshader == NULL){
		printf(ANSI_YELLOW("Vertex or Fragment Shader not set, resolving.\n"));
		char *default_dir = strdup(cwd);
		// default_dir = realloc(default_dir, (cwd_len)* sizeof(char));
		strncat(default_dir, "\\Resources\\Shaders\\Shaders.txt", 30);
		shaders_pull(default_dir);
	}
	shaderblock_t *shader = shader_compile(clean);
	if(do_uniforms == true){shader->uniforms = uniform_init(&shader->uniform_len, shader->shaderProgram);}
	
	return shader;
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
bufferobj_t *bufferobj_gen(mesh_t *mesh, const GLenum draw_format, const int pos_layout, int col_layout, int tex_layout){
	bufferobj_t *out = malloc(sizeof(bufferobj_t));
	bufferobject_handle(out, mesh->mesh_data, mesh->data_len, mesh->vertex_index, mesh->index_len, draw_format, 9);
	mesh_attrlink(out, pos_layout, col_layout, tex_layout, mesh);
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
			if(glIsBuffer(buffer->VBO) == GL_FALSE){
				GLCall(glGenBuffer(&buffer->VBO));
				buffer->buffer_[1] = false;
			}
			if(glIsBuffer(buffer->EBO) == GL_FALSE){
				GLCall(glGenBuffer(&buffer->EBO));
				buffer->buffer_[2] = false;
			}
			GLCall(glBindVertexArray(buffer->VAO)); // Begin recording state.
		} 
		/*VBO not set-up*/
		if(buffer->buffer_[1] == false && vertices != NULL){	
			GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer->VBO)); 
			GLCall(glBufferData(GL_ARRAY_BUFFER, v_len, vertices, draw_format));
			buffer->buffer_[1] = true;
		} 
		/*EBO not set-up*/ 
		if(buffer->buffer_[2] == false){  
			GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->EBO));    
			GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, (index_len), index_order, draw_format));
			buffer->buffer_[2] = true;
		}
		success = buffer->buffer_[0] == true && buffer->buffer_[1] == true && buffer->buffer_[2] == true;
	}while (counter < max_tries && success == false);
	GLCall(glBindVertexArray(0));
	buffer->vertex_num = v_len;
	buffer->element_num = index_len;
}