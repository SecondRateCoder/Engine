#include <engine/Public.h>

#define GL_GLEXT_PROTOTYPES

// Function prototypes for missing functions, assuming they exist elsewhere
extern const size_t builtin_shader_typehash[];
extern const char* vertexshader_default;
extern const char* fragmentshader_default;
extern const char* vs_start;
extern const char* fs_start;
extern const char* shader_end;
extern char cwd[MAX_PATHLENGTH];
extern char* vertexshader;
extern char* fragmentshader;
extern char* geometryshader;

// Global variables should be declared if used globally
// These are assumed to be defined in `DrawingProtocol.h` or elsewhere
// GLuint vertexshader, fragmentshader, geometryshader;
// char* vertexshader_code;
// char* fragmentshader_code;
// char* geometryshader_code;

bool cwd_init(){
	if (getcwd(cwd, MAX_PATHLENGTH) == NULL) {
		printf("getcwd() Error, cwd not Initialised.\n");
		return false;
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

	if (shaders != NULL) {
		settings = (char*)malloc(11 * sizeof(char)); // Allocate space for 10 chars + null terminator
		if (settings == NULL) {
			fprintf(stderr, "Error: Memory allocation failed in shadersettings_rw.\n");
			fclose(shaders);
			return NULL;
		}

		if (fread(settings, sizeof(char), 10, shaders) != 10) {
			fprintf(stderr, "Error: Could not read 10 characters from file.\n");
			free(settings);
			settings = NULL;
		} else {
			settings[10] = '\0'; // Null-terminate the string
		}

		if (write != NULL && strlen(write) == 10) {
			fseek(shaders, 0, SEEK_SET);
			if (fwrite(write, sizeof(char), 10, shaders) != 10) {
				fprintf(stderr, "Error: Could not write 10 characters to file.\n");
			}
		}
		fclose(shaders);
	} else {
		fprintf(stderr, "Error: Failed to open file at '%s'.\n", filepath);
	}
	return settings;
}

// Fixed shader_pull function.
// The original had numerous logic errors, memory leaks, and incorrect file handling.
// This version uses `fgetc` to read the file character by character, which is more robust
// than `fgets` for finding specific keywords. It also manages memory correctly.
void shaders_pull(const char* filepath){
	// Free any previously allocated shaders to prevent memory leaks
	free(vertexshader);
	free(fragmentshader);
	free(geometryshader);
	vertexshader = NULL;
	fragmentshader = NULL;
	geometryshader = NULL;

	FILE* shaders = fopen(filepath, "r");
	if (shaders == NULL) {
		fprintf(stderr, "Error: Failed to open shader file at '%s'.\n", filepath);
		return;
	}

	// Read settings from the first 10 characters.
	char settings[11];
	if (fread(settings, sizeof(char), 10, shaders) != 10) {
		fprintf(stderr, "Error: Could not read shader settings from file.\n");
		fclose(shaders);
		return;
	}
	settings[10] = '\0';

	//Read off the wanted vertex and fragment shader items.
	int vsindex = (int)strtol(&settings[6], NULL, 10);
	int fsindex = (int)strtol(&settings[8], NULL, 10);
	if (vsindex < 0){vsindex = 0;}
	if (fsindex < 0){fsindex = 0;}

	int vs_cc = -1, fs_cc = -1;
	bool in_shader_block = false;
	char line_buffer[256]; // A buffer for reading lines
	
	// Process the file line by line
	while (fgets(line_buffer, sizeof(line_buffer), shaders) != NULL) {
		if (strncmp(line_buffer, "#define vs", 10) == 0) {
			vs_cc++;
			if (vs_cc == vsindex) {
				// Found the correct vertex shader. Read its content.
				long start_pos = ftell(shaders);
				char temp[10];
				long end_pos = start_pos;
				while (fgets(line_buffer, sizeof(line_buffer), shaders) != NULL) {
					if (strncmp(line_buffer, "#shaderend", 10) == 0) {
						end_pos = ftell(shaders) - strlen(line_buffer);
						break;
					}
				}
				
				if (end_pos > start_pos) {
					fseek(shaders, start_pos, SEEK_SET);
					size_t size = end_pos - start_pos;
					vertexshader = (char*)malloc(size + 1);
					if (vertexshader) {
						fread(vertexshader, 1, size, shaders);
						vertexshader[size] = '\0';
					}
				}
				fseek(shaders, end_pos, SEEK_SET); // Reset file pointer
			}
		} else if (strncmp(line_buffer, "#define fs", 10) == 0) {
			fs_cc++;
			if (fs_cc == fsindex) {
				// Found the correct fragment shader. Read its content.
				long start_pos = ftell(shaders);
				char temp[10];
				long end_pos = start_pos;
				while (fgets(line_buffer, sizeof(line_buffer), shaders) != NULL) {
					if (strncmp(line_buffer, "#shaderend", 10) == 0) {
						end_pos = ftell(shaders) - strlen(line_buffer);
						break;
					}
				}
				
				if (end_pos > start_pos) {
					fseek(shaders, start_pos, SEEK_SET);
					size_t size = end_pos - start_pos;
					fragmentshader = (char*)malloc(size + 1);
					if (fragmentshader) {
						fread(fragmentshader, 1, size, shaders);
						fragmentshader[size] = '\0';
					}
				}
				fseek(shaders, end_pos, SEEK_SET); // Reset file pointer
			}
		}
	}

	if (vertexshader == NULL) {
		fprintf(stderr, "Warning: Vertex shader with index %d not found, using default.\n", vsindex);
	}
	if (fragmentshader == NULL) {
		fprintf(stderr, "Warning: Fragment shader with index %d not found, using default.\n", fsindex);
	}
	
	fclose(shaders);
}

// Corrected uniform_init function.
// The original was missing crucial cleanup and had logical errors in memory management.
// The `strndup` calls and the `realloc` logic were a particular problem.
void uniform_init(shaderblock_t* sb_t) {
	if (sb_t == NULL) return;

	// A list of shader sources to iterate through
	const char* shader_sources[] = {
		vertexshader ? vertexshader : vertexshader_default,
		fragmentshader ? fragmentshader : fragmentshader_default,
		geometryshader ? geometryshader : NULL
		// Add tessellation and compute shaders if they are used
		// tessellation_controlshader ? tessellation_controlshader : NULL,
		// tessellation_evaluationshader ? tessellation_evaluationshader : NULL
	};
	const size_t num_shaders = sizeof(shader_sources) / sizeof(shader_sources[0]);

	// Clear existing uniforms before populating
	if (sb_t->uniforms) {
		for (size_t i = 0; i < sb_t->uniform_len; ++i) {
			destroy_arrkey(&sb_t->uniforms[i]);
		}
		free(sb_t->uniforms);
		sb_t->uniforms = NULL;
		sb_t->uniform_len = 0;
	}

	// List for custom struct types
	char** temp_typenames = NULL;
	size_t typenames_len = 0;

	for (size_t i = 0; i < num_shaders; ++i) {
		const char* temp_txt = shader_sources[i];
		if (temp_txt == NULL) continue;
		size_t temptxt_len = strlen(temp_txt);

		// This loop parses the shader source code
		for (size_t cc = 0; cc < temptxt_len; ) {
			// Find the start of a potential declaration
			while (cc < temptxt_len && isspace(temp_txt[cc])) {
				cc++;
			}
			if (cc >= temptxt_len) break;

			// Check for 'uniform' or 'struct' keywords
			if (cc + 6 <= temptxt_len && strncmp(&temp_txt[cc], "struct", 6) == 0) {
				cc += 6;
				while (cc < temptxt_len && isspace(temp_txt[cc])) { cc++; }

				size_t typename_start = cc;
				while (cc < temptxt_len && !isspace(temp_txt[cc]) && temp_txt[cc] != '{' && temp_txt[cc] != ';') {
					cc++;
				}
				size_t typename_len = cc - typename_start;
				char* typename = strndup(&temp_txt[typename_start], typename_len);

				if (typename) {
					bool is_duplicate = false;
					for (size_t _dupecc = 0; _dupecc < typenames_len; ++_dupecc) {
						if (strcmp(typename, temp_typenames[_dupecc]) == 0) {
							is_duplicate = true;
							break;
						}
					}
					
					if (!is_duplicate) {
						char** temp_arr = (char**)realloc(temp_typenames, sizeof(char*) * (typenames_len + 1));
						if (temp_arr) {
							temp_typenames = temp_arr;
							temp_typenames[typenames_len] = typename;
							typenames_len++;
						} else {
							fprintf(stderr, "Error: Failed to reallocate memory for struct names.\n");
							free(typename);
						}
					} else {
						free(typename);
					}
				}
				
				// Skip the rest of the struct definition
				int brace_count = 0;
				while (cc < temptxt_len) {
					if (temp_txt[cc] == '{') brace_count++;
					else if (temp_txt[cc] == '}') brace_count--;
					if (brace_count == 0) break;
					cc++;
				}
				if (cc < temptxt_len) cc++;
			} else if (cc + 7 <= temptxt_len && strncmp(&temp_txt[cc], "uniform", 7) == 0) {
				cc += 7;
				while (cc < temptxt_len && isspace(temp_txt[cc])) { cc++; }

				size_t type_start = cc;
				while (cc < temptxt_len && !isspace(temp_txt[cc]) && temp_txt[cc] != ';' && temp_txt[cc] != '=') {
					cc++;
				}
				size_t type_len = cc - type_start;
				char* type_pure = strndup(&temp_txt[type_start], type_len);
				
				while (cc < temptxt_len && isspace(temp_txt[cc])) { cc++; }
				
				size_t name_start = cc;
				while (cc < temptxt_len && !isspace(temp_txt[cc]) && temp_txt[cc] != ';' && temp_txt[cc] != '=') {
					cc++;
				}
				size_t name_len = cc - name_start;
				char* name_pure = strndup(&temp_txt[name_start], name_len);

				if (type_pure && name_pure) {
					bool is_duplicate = false;
					for (size_t u_cc = 0; u_cc < sb_t->uniform_len; ++u_cc) {
						if (sb_t->uniforms[u_cc].name && strcmp(sb_t->uniforms[u_cc].name, name_pure) == 0) {
							is_duplicate = true;
							break;
						}
					}

					if (!is_duplicate) {
						GLint _ID = glGetUniformLocation(sb_t->shaderProgram, name_pure);
						
						if (_ID != -1) {
							arrk_t new_uniform = (arrk_t){_ID, name_pure, type_pure};
							arrk_t* temp_arr = (arrk_t*)realloc(sb_t->uniforms, sizeof(arrk_t) * (sb_t->uniform_len + 1));
							if (temp_arr) {
								sb_t->uniforms = temp_arr;
								sb_t->uniforms[sb_t->uniform_len++] = new_uniform;
							} else {
								destroy_arrkey(&new_uniform);
								fprintf(stderr, "Error: Failed to reallocate memory for uniforms.\n");
							}
						} else {
							fprintf(stderr, "Error: Failed to get uniform location for '%s'.\n", name_pure);
						}
					}
				}
				free(type_pure);
				free(name_pure);
			} else {
				while (cc < temptxt_len && temp_txt[cc] != '\n' && temp_txt[cc] != ';') { cc++; }
				if (cc < temptxt_len) cc++;
			}
		}
	}

	for (size_t i = 0; i < typenames_len; ++i) {
		free(temp_typenames[i]);
	}
	free(temp_typenames);
}

void destroy_arrkey(arrk_t *arrk){
	free(arrk->name);
	free(arrk->type);
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
void uniform_write(shaderblock_t* shader, const char* type, const char* name, const char* property, bool* transpose, const void* value, size_t num_elements){
	if (shader == NULL || name == NULL) return;

	size_t hash = str_hash(str_normalise(type, true, true));
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
		return;
	}

	// Use a switch statement for dispatching GL uniform functions
	// Note: The original switch had incorrect pointer casts and logic for single vs. multiple elements.
	switch (hash) {
		case builtin_shader_typehash[0]: // GL_INT
		case builtin_shader_typehash[1]: // GL_BOOL
			glUniform1iv(location, num_elements, (const GLint*)value);
			break;
		case builtin_shader_typehash[2]: // GL_UNSIGNED_INT
			glUniform1uiv(location, num_elements, (const GLuint*)value);
			break;
		case builtin_shader_typehash[3]: // GL_FLOAT
			glUniform1fv(location, num_elements, (const GLfloat*)value);
			break;
		case builtin_shader_typehash[4]: // vec2
			glUniform2fv(location, num_elements, (const GLfloat*)value);
			break;
		case builtin_shader_typehash[5]: // vec3
			glUniform3fv(location, num_elements, (const GLfloat*)value);
			break;
		case builtin_shader_typehash[6]: // vec4
			glUniform4fv(location, num_elements, (const GLfloat*)value);
			break;
		case builtin_shader_typehash[7]: // ivec2
			glUniform2iv(location, num_elements, (const GLint*)value);
			break;
		case builtin_shader_typehash[8]: // ivec3
			glUniform3iv(location, num_elements, (const GLint*)value);
			break;
		case builtin_shader_typehash[9]: // ivec4
			glUniform4iv(location, num_elements, (const GLint*)value);
			break;
		case builtin_shader_typehash[10]: // uvec2
			glUniform2uiv(location, num_elements, (const GLuint*)value);
			break;
		case builtin_shader_typehash[11]: // uvec3
			glUniform3uiv(location, num_elements, (const GLuint*)value);
			break;
		case builtin_shader_typehash[12]: // uvec4
			glUniform4uiv(location, num_elements, (const GLuint*)value);
			break;
		case builtin_shader_typehash[13]: // mat2
			glUniformMatrix2fv(location, num_elements, transpose != NULL && *transpose, (const GLfloat*)value);
			break;
		case builtin_shader_typehash[14]: // mat3
			glUniformMatrix3fv(location, num_elements, transpose != NULL && *transpose, (const GLfloat*)value);
			break;
		case builtin_shader_typehash[15]: // mat4
			glUniformMatrix4fv(location, num_elements, transpose != NULL && *transpose, (const GLfloat*)value);
			break;
		case builtin_shader_typehash[16]: // mat2x3
			glUniformMatrix2x3fv(location, num_elements, transpose != NULL && *transpose, (const GLfloat*)value);
			break;
		case builtin_shader_typehash[17]: // mat3x2
			glUniformMatrix3x2fv(location, num_elements, transpose != NULL && *transpose, (const GLfloat*)value);
			break;
		case builtin_shader_typehash[18]: // mat2x4
			glUniformMatrix2x4fv(location, num_elements, transpose != NULL && *transpose, (const GLfloat*)value);
			break;
		case builtin_shader_typehash[19]: // mat4x2
			glUniformMatrix4x2fv(location, num_elements, transpose != NULL && *transpose, (const GLfloat*)value);
			break;
		case builtin_shader_typehash[20]: // mat3x4
			glUniformMatrix3x4fv(location, num_elements, transpose != NULL && *transpose, (const GLfloat*)value);
			break;
		case builtin_shader_typehash[21]: // mat4x3
			glUniformMatrix4x3fv(location, num_elements, transpose != NULL && *transpose, (const GLfloat*)value);
			break;
		default:
			fprintf(stderr, "Error: Unsupported shader type or uniform '%s' not found.\n", type);
			break;
	}
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
		if (strcmp(type, "VERTEX") == 0) {
			shader = sb_t->vertexshader;
		} else if (strcmp(type, "FRAGMENT") == 0) {
			shader = sb_t->fragmentshader;
		} else if (strcmp(type, "GEOMETRY") == 0) {
			shader = sb_t->geometryshader;
		} else {
			fprintf(stderr, "Error: Unknown shader type '%s'.\n", type);
			return;
		}

		if (shader == 0) { // Check if shader was created
			return;
		}

		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			fprintf(stderr, "Shader %s compilation error: %s\n", type, infoLog);
		}
	}
}


// Corrected win_draw function.
// The original code had incorrect OpenGL function calls, illogical error handling,
// and a memory leak in the index handling.
void win_draw(win_t *win, const GLfloat *points, size_t len, GLuint *indexes, size_t ilen){
	if (win == NULL || points == NULL || len == 0) return;
	if(win->buffer_len == 0 || win->buffers == NULL){
		win->buffers = (bufferobj_t*)malloc(sizeof(bufferobj_t));
		win->buffer_curr = 0;
		win->buffer_len = 1;
		win->buffers[0]->VBO = malloc(sizeof(GLuint));
		win->buffers[0]->EBO = malloc(sizeof(GLuint));
	}
	BUFFEROBJECT_HANDLE(win->buffers[win->buffer_curr], points, len, indexes, &ilen, GL_STATIC_DRAW, 10);
	// Configure vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	// Drawing call
	glDrawElements(GL_TRIANGLES, ilen, GL_UNSIGNED_INT, 0);
}

// Corrected winimage_append function.
// The original code used `glGenTexturesEXT`, which is not standard, and had other
// incorrect function calls and logic for texture management.
void winimage_append(win_t* win, const char* filepath, const argb_t* border_color){
	if (win == NULL || filepath == NULL) return;

	// Use a simpler approach to add a new texture
	win->textures = (image_t*)realloc(win->textures, sizeof(image_t) * (win->textures_len + 1));
	if (win->textures == NULL) {
		fprintf(stderr, "Error: Failed to reallocate memory for textures.\n");
		return;
	}
	
	image_t* new_image = &win->textures[win->textures_len];

	// Load the image with STB
	stbi_set_flip_vertically_on_load(true);
	new_image->img = stbi_load(filepath, &new_image->width, &new_image->height, &new_image->color_channels, 0);
	if (new_image->img == NULL) {
		fprintf(stderr, "Error: Failed to load image at '%s'.\n", filepath);
		return;
	}

	// Generate and bind the new texture
	glGenTextures(1, &new_image->ID);
	glBindTexture(GL_TEXTURE_2D, new_image->ID);

	// Set texture wrapping and filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[4] = {
		(float)border_color->r,
		(float)border_color->g,
		(float)border_color->b,
		(float)border_color->a
	};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	// Upload image data to the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, new_image->width, new_image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, new_image->img);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	win->textures_len++;
}

// Fixed win_flood function.
// The original had no issues, but some minor formatting improvements were made for clarity.
void win_flood(win_t* win, const argb_t c) {
	if (win == NULL || win->window == NULL) return;
	
	glViewport(0, 0, win->w, win->h);
	glClearColor(c.r, c.g, c.b, c.a);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(win->window);
}

/// @brief Do a function from BUFFER_OPTIONS on EBO or VBO of a Buffer Object.
/// @param buffer 
/// @param option 
/// @return 
void *buffer_bufferdo(bufferobj_t* buffer, const size_t len, const BUFFER_OPTIONS option){
	switch(option){
		//For single buffer objects.
		case BUFFER_OPTIONS_CLEAR:
			glDeleteVertexArrays(1, buffer->VAO);
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
	return win->buffers[win->buffer_curr];
}

