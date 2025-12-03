#include "../engine/Public.h"

/// @brief My handle for handling glfw errors.
/// @param error_code The error code for the error.
/// @param msg The msg that should be used.
void handle_glfw_error_default(int error_code, const char *msg){fprintf(stderr, ANSI_RED("GLFW Error: %d\n\t%s"), error_code, msg);}

/// @brief Initialise an Un-Resizable GLFW Window.
/// @param name The Name of the Window.
/// @param w The Width of the Window.
/// @param h The Height of the Window.
/// @return An initialised Pointer to the Window.
win_t* win_init(char* name, GLFWerrorfun error_handle, poll_do poll_do_, poll_kill poll_kill_, uint32_t w, uint32_t h){
	//Initialise GLFW.
	glfwSetErrorCallback(error_handle == NULL? handle_glfw_error_default: error_handle);
	if(glfwInit() == GL_FALSE){fprintf(stderr, ANSI_RED("YOOOOO!!!! Why is ts failing oml!!!"));}
	if(error_handle == NULL){error_handle = handle_glfw_error_default;}
	// glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	//Not Resizable.
	// glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	//Using Legacy, OpenGL 3.3 for my 9800 GT GPU.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
	//malloc on heap.
	win_t *win = calloc(1, sizeof(win_t));
	*win = (win_t){
		.g_window =glfwCreateWindow(w, h, name, NULL, NULL),
		.w = w,
		.h = h,
		.polld = poll_do_,
		.pollk = poll_kill_,
		.scenes = NULL,
	};
	// gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	//Handle errors.
	if(win->g_window == NULL){
		//If nothing was created, free the window and it's attributes.
		printf(ANSI_RED("Window %s failed to Initialise\nError!"), name);
		free(win->name);
		glfwTerminate();
		return NULL;
	}else{
		// Window successfully created.
		printf(ANSI_GREEN("Window %s successfully created\n"), name);
		glfwMakeContextCurrent(win->g_window);
		gladLoadGL();
	}
	shaderblock_t *temp0 = shaderblock_gen(true, true, true, NULL);
	image_t *temp_image = NULL;
	temp_image = image_gen(4, "C:\\Users\\olusa\\OneDrive\\Documents\\GitHub\\Engine\\Resources\\Textures\\no_texture.png", (float[]){0, 0, 0, 0}, (texformat_t){
		.target = GL_TEXTURE_2D,
		.pixel_format = GL_RGBA,
		.pixel_type = GL_UNSIGNED_BYTE,
		.level = 0,
		.internalFormat = GL_RGBA8,
		.depth = 0
	});
	mesh_t *temp2 = mesh_gen(
		NULL, (vec3){0, 0, 0}, (vec3){0, 0, 0},
		(GLfloat[]){
			// Position coords                      // Color data           // Texture coords
			-0.5f,   (-0.5f * sqrt3) / 3,    0,      8.0f, 0.3f, 0.2f,    0.0f, 5.0f,
			0.5f,    (-0.5f * sqrt3) / 3,    0,      8.0f, 0.3f, 0.2f,    5.0f, 0.0f,
			-0.5f,   (-0.5f * sqrt3 * 2) / 3,0,      8.0f, 0.3f, 0.2f,    0.0f, 0.0f,
			-0.5f/2, (0.5f * sqrt3) / 6,     0,      8.0f, 0.3f, 0.2f,    2.5f, 5.0f,
			0.5f/2,  (0.5f * sqrt3) / 6,     0,      8.0f, 0.3f, 0.2f,    2.5f, 5.0f,
			0.0f,    (0.5f * sqrt3) / 3,     0,      8.0f, 0.3f, 0.2f,    2.5f, 10.0f // Example texture coords
		},
		(GLuint[]){
			0, 1, 2,
			0, 2, 3,
			0, 1, 4,
			1, 2, 4,
			2, 3, 4,
			3, 0, 4
		},
		(size_t[2]){48, 18}, (uint8_t[4]){8, 3, 2}, (uint8_t[3]){0, 1, 2}, (uint8_t[3]){0, 3, 6},
		GL_TRIANGLES,
		temp_image,
		(collider_shape_t){
			.offs = {0},
			.scale = {0},
			.parent = NULL, 
			.shape = COLLSHAPE_CUBOID
		}
	);
	scene_t *temp3 = scene_gen(
		win->g_window, 
		"New scene", 
		temp2,
		temp0,
		NULL,
		(size_t[2]){1, 0, 0}
	);
	win->scenes = temp3;
	win->loaded_scenes = malloc(sizeof(void *));
	*win->loaded_scenes = 0;
	win->num_loaded = 1;
	return win;
}

// void *win_truepoll(void *arg){
// 	win_t *win_ = (win_t *)arg;
// 	win_t *win = calloc(1, sizeof(win_t));

// 	// Set context hints
// 	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
// 	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
// 	#ifdef __APPLE__
// 	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
// 	#endif
// 	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
// 	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

// 	// Create new window with shared context
// 	*win = *win_;
// 	win->g_window = glfwCreateWindow(win_->w, win_->h, win_->name, NULL, win_->g_window);
// 	glfwMakeContextCurrent(win_->g_window);
// 	// Now copy the rest of the data
// 	// win->g_window = new_window; // preserve the new window pointer
// 	size_t cc =0;
// 	while(!win_shouldclose(win)){
// 		//Flood color with Orange.
// 		if(cc%5 == 0){win_flood(win, (argb_t){1.5f, 0.5f, 0, 1});}
		
// 		//Ensure that win's Shader's are Handled.
// 		//Explicitly define the Shader to be Used.
// 		GLUseProgram(win->shaders[win->shaders_curr].shaderProgram);
// 		if(win->polld != NULL){win->polld(win, cc);}
// 		//Bind win's VAO for Drawing.
// 		// printf(ANSI_YELLOW("Current VAO: %u"), glad_glIsVertexArray(win->buffers[win->buffer_curr].VAO));
		
// 		for(size_t cc_draw =0; cc_draw < win->buffer_len; ++cc_draw){
// 			glBindVertexArray(win->buffers[cc_draw].VAO);
// 			if(win->buffers[cc_draw].buffer_[2] == true){
// 				glDrawElements(GL_TRIANGLES, win->buffers[cc_draw].element_num, GL_UNSIGNED_INT, 0);
// 			}else{glDrawArrays(GL_TRIANGLES, 0, win->buffers[cc_draw].vertex_num);}
// 		}
// 		glfwSwapBuffers(win->g_window);

// 		printf(ANSI_YELLOW("%d"), glGetError());
// 		glfwPollEvents();
// 		++cc;
// 	}
// 	if(win->pollk != NULL){win->pollk(win);}
// 	win_kill(win);
// 	return NULL;
// }

#define MAX_POLLS 0xafffffffffffffffull

/// @brief Poll the GLFW window's events.
/// @param win The Window to be Polled.
/// @remark If the Window is set to close, It will Kill it after calling a set poll_kill function.
void win_poll(win_t *win){
	size_t cc =0, cycles = 0;
	//Spawn a thread that runs the drawing of Meshes.
	while(!win_shouldclose(win)){
		//Flood color with Orange.
		if(cc%5 == 0){win_flood(win, (argb_t){1.5f, 0.5f, 0, 1});}
		
		//Ensure that win's Shader's are Handled.
		//Explicitly define the Shader to be Used.
		GLUseProgram(wscene_curr(win).shaders->shaderProgram);
		size_t cc_ = 0;
		if(win->polld != NULL){
			win->polld(win, cc, cycles);
			// ipoll_t *poll_t = (ipoll_t *){win, cc};
			// pthread_create(&win->poll_thread, thread_attr, win->polld, (void *)poll_t);
			// pthread_join(win->poll_thread, NULL);
		}
		for(size_t cc2 = 0; cc2 < win->num_loaded; ++cc2, cc_ = win->loaded_scenes[cc]){scene_poll(win->scenes + cc_, cc, cycles);}
		//Bind win's VAO for Drawing.
		// printf(ANSI_YELLOW("Current VAO: %u"), glad_glIsVertexArray(win->buffers[win->buffer_curr].VAO));
		glfwSwapBuffers(win->g_window);

		// printf(ANSI_YELLOW("Log: %d"), glGetError());
		glfwPollEvents();
		if(cc > MAX_POLLS){
			cc = 0;
			cycles++;
		}
		++cc;
	}
	if(win->pollk != NULL){win->pollk(win);}
	win_kill(win);
	return;
}


/// @brief Check if the specified Window was set to be closed.
/// @param win The Window to be checked.
/// @return Was the User trying to close the Window.
bool win_shouldclose(win_t *win){return glfwWindowShouldClose(win->g_window);}

/// @brief Terminate and free the specified Window Pointer.
/// @param win The Window to be terminated.
void win_kill(win_t *win){
	for(size_t cc =0; cc < win->scene_num; ++cc){scene_kill(win->scenes + cc, 1);}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glfwDestroyWindow(win->g_window);
	glfwTerminate();
	free(win->name);
	// if(win->polld != NULL){free(win->polld);}
	// if(win->pollk != NULL){free(win->pollk);}
	
	// free(win->textures);
	free(win);
}


// /// @brief Link the win's VAO with the VBO.
// /// @param win The Window containing the VAO and VBO.
// /// @param layout ...
// /// @param component_num ...
// /// @param type ...
// /// @param stride ...
// /// @param offset ...
// void win_attrblink(window *win, size_t curr_buffer, GLuint layout_index, GLuint component_num, GLenum type, GLsizeiptr stride, const void *pointer){
// 	glBindBuffer(GL_ARRAY_BUFFER, win->buffers[win->buffer_curr].VBO[curr_buffer]);
// 	glVertexAttribPointer(layout_index, component_num, type, GL_FALSE, stride, pointer);
// 	glEnableVertexAttribArray(layout_index);
// }