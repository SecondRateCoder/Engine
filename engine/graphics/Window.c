#include "graphics.h"

void handle_glfw_error_default(int error_code, const char *msg){fprintf(stderr, "GLFW Error: %d\n\t%s", error_code, msg);}

/// @brief Initialise an Un-Resizable GLFW Window.
/// @param name The Name of the Window.
/// @param w The Width of the Window.
/// @param h The Height of the Window.
/// @return An initialised Pointer to the Window.
win_t* win_init(char* name, GLFWerrorfun error_handle, poll_do poll_do_, poll_kill poll_kill_, uint32_t w, uint32_t h){
	//Initialise GLFW.
	glfwSetErrorCallback(error_handle == NULL? handle_glfw_error_default: error_handle);
	if(glfwInit() == GL_FALSE){fprintf(stderr, "YOOOOO!!!! Why is ts failing oml!!!");}
	// glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	//Not Resizable.
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	//Using Legacy, OpenGL 3.3 for my 9800 GT GPU.
	glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	//malloc on heap.
	win_t *win = malloc(sizeof(win_t));
	if(poll_do_ != NULL){win->polld = poll_do_;}else{win->polld = NULL;}
	if(poll_kill_ != NULL){win->pollk = poll_kill_;}else{win->pollk = NULL;}
	win->name = name;
	win->w =w;
	win->h =h;
	win->g_window =glfwCreateWindow(w, h, name, NULL, NULL);
	//Handle errors.
	if(win->g_window == NULL){
		//If nothing was created, free the window and it's attributes.
		printf("Window %s failed to Initialise\nError!", name);
		free(win->name);
		glfwTerminate();
		return NULL;
	}else{
		// Window successfully created.
		printf("Window %s successfully created\n", name);
		glfwMakeContextCurrent(win->g_window);
		gladLoadGL();
	}
	return win;
}

/// @brief Poll the GLFW window's events.
/// @param win The Window to be Polled.
/// @remark If the Window is set to close, It will Kill it after calling a set poll_kill function.
void win_poll(win_t *win){
	size_t cc =0;
	while(!win_shouldclose(win)){
		//Flood color with Orange.
		win_flood(win, (argb_t){0.5f, 0.5f, 0, 1});
		//Ensure that win's Shader's are Handled.
		SHADERBLOCK_HANDLE(win->shaders, true, true);
		//Explicitly define the Shader to be Used.
		glUseProgram(win->shaders->shaderProgram);
		win->polld;
		//Bind win's VAO for Drawing.
		glBindVertexArray(win->buffers[win->buffer_curr].VAO);
		//Draw Triangles with GL_TRIANGLE Primitive.
		glDrawElements(GL_TRIANGLES, win->vert_count, GL_UNSIGNED_INT, 0);
		// glDrawElements(GL);
		glfwSwapBuffers(win->g_window);
		glfwPollEvents();
		++cc;
	}
	win->pollk(win);
	win_kill(win);
}

/// @brief Check if the specified Window was set to be closed.
/// @param win The Window to be checked.
/// @return Was the User trying to close the Window.
bool win_shouldclose(win_t *win){return glfwWindowShouldClose(win->g_window);}

/// @brief Terminate and free the specified Window Pointer.
/// @param win The Window to be terminated.
void win_kill(win_t *win){
	buffer_bufferdo(win->buffers, win->buffer_len, BUFFER_OPTIONS_FREE_ALL);
	glDeleteProgram(win->shaders->shaderProgram);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glfwDestroyWindow(win->g_window);
	GLuint *TEXTURES_ID = (GLuint *)malloc(win->textures_len);
	for(size_t cc =0; cc < win->textures_len; ++cc){TEXTURES_ID[cc] = win->textures[cc].ID;}
	glDeleteTextures(win->textures_len, TEXTURES_ID);
	free(TEXTURES_ID);
	glfwTerminate();
	free(win->name);
	free(win->polld);
	free(win->pollk);
	free(win->textures);
	free(win->shaders);
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

void polld_default(win_t *win, size_t cycles){return;}
void pollk_default(win_t *win){return;}